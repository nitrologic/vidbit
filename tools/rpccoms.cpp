// rpccoms.cpp
// 0.1 reset clock

// JSON RPC https://www.jsonrpc.org/specification

// method params under implementation

// host
// device
// reset
// restart 
// 

#include <nitrohost.h>

#include <iostream>
#include "tools.h"

#include <thread>
#include <atomic>
#include <chrono>

#include <windows.h>

bool printComPort(comHandle handle,const std::string &text);

struct comPort{
	std::string name;
	comHandle handle;
	comPort(std::string portName,comHandle portHandle): name(std::move(portName)), handle(portHandle){
	}
	void disconnect(){
		handle=nullptr;
	}
	void connect(comHandle portHandle){
		handle=portHandle;
	}
	void print(std::string line){
		if(handle){
			printComPort(handle,line);
		}
	}
};



bool anyKeyDown();
void pollMessages(HWND targetWindow);

inline byteData toBytes(const std::string& str) {
	return byteData(str.begin(), str.end());
}

bool printComPort(comHandle handle,const std::string &text){
	const byteData data=toBytes(text+"\r\n");
	bool success=writeComPort(handle,data);
	return success;
}

int readComPort(comHandle handle, uint8_t* buffer, size_t maxSize){
	HANDLE h = (HANDLE)handle;
	DWORD bytesRead = 0;
	if (!ReadFile(h, buffer, (DWORD)maxSize, &bytesRead, NULL)) return -1;
	if(bytesRead) std::cout << "[RPC] readfile bytesRead:" << bytesRead << std::endl;
	return (int)bytesRead;
}

#include <mutex>
#include <optional>

class RpcFifo {
private:
	std::string buffer;
	std::mutex mutex;
public:
	void onReceive(uint8_t *bytes, int count) {
		std::lock_guard<std::mutex> lock(mutex);
		std::string payload=std::string(reinterpret_cast<char*>(bytes), count);
		buffer.append(payload);
//		std::cout << "[RPC] onReceive count:" << count << std::endl;		
	}

	std::optional<std::string> readLine() {
		std::lock_guard<std::mutex> lock(mutex);
		size_t pos = buffer.find("\n");
		if (pos != std::string::npos) {
			std::string line = buffer.substr(0, pos);
			buffer.erase(0, pos + 1);
			return line;
		}
		return std::nullopt;
	}

	// readLine() skips empty lines
	std::optional<std::string> readLine2() {
		std::lock_guard<std::mutex> lock(mutex);
		size_t pos = buffer.find("\r\n",2);
		while(pos==0){
			buffer.erase(0, pos + 2);
			return ":()";
//			pos = buffer.find("\r\n",2);
		}
		if (pos != std::string::npos) {
			std::string line = buffer.substr(0, pos);
			buffer.erase(0, pos + 2);
			return line;
		}
		return std::nullopt;
	}

	void clear() {
		std::lock_guard<std::mutex> lock(mutex);
		buffer.clear();
	}
};

RpcFifo rpcFifo;

void rpcReceive(uint8_t *bytes,int count){
	rpcFifo.onReceive(bytes,count);
}

void rpcThread(comPort &port){
	const size_t bufSize = 4096;
	auto buffer = std::make_unique<uint8_t[]>(bufSize);
	std::cout << "[thread] starting reader for " << port.name << std::endl;
	while (true){
		auto bptr=buffer.get();
		int n = readComPort(port.handle, bptr, bufSize - 1);
		if (n > 0){
			rpcReceive(bptr,n);
//			std::cout << "[RPC] rpcReceive n:" << n << std::endl;
		}
		else if (n == -1)
		{
			std::cout << port.name << " disconnected." << std::endl;
//			exit(0);
			break;
		}
		else
		{
//			std::cout << portName << " read error." << std::endl;
//			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


void rpcThread2(comHandle h, const std::string portName){
	const size_t bufSize = 4096;
	auto buffer = std::make_unique<uint8_t[]>(bufSize);
	std::cout << "[thread] starting reader for " << portName << std::endl;
	while (true){
		auto bptr=buffer.get();
		int n = readComPort(h, bptr, bufSize - 1);
		if (n > 0){
			rpcReceive(bptr,n);
//			std::cout << "[RPC] rpcReceive n:" << n << std::endl;
		}
		else if (n == -1)
		{
			std::cout << portName << " disconnected." << std::endl;
//			exit(0);
			break;
		}
		else
		{
//			std::cout << portName << " read error." << std::endl;
//			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

std::vector<comPort> comHandles={};

int scanPorts(){	
	auto ports = enumerateComPorts();
	for (const auto& portinfo : ports) {
		const bool isPico=(portinfo.devicePath.rfind("USB\\VID_2E8A",0)==0);
		if(isPico){
			bool anon=true;
			std::string name=portinfo.portName;
			for(comPort &port:comHandles){
				if(port.name==name){
					anon=false;
					if(port.handle==nullptr){
						comHandle handle=openComPort(name);
						if(handle){
							port.handle=handle;
							std::cout << "[RPC] reconnected port:" << name << std::endl;
						}else{
							std::cout << "[RPC] scanPorts reconnect failed for " << name << std::endl;
						}
					}
					break;
				}
			}
			if(anon){
				comHandle handle=openComPort(name);
				if(handle){
					comHandles.emplace_back(name,handle);
					std::cout << "[RPC] reconnected port:" << name << std::endl;
				}else{
					std::cout << "[RPC] scanPorts connection fail name:" << name << std::endl;
				}
			}
		}
	}
	return 0;
}

void echoComPort(comHandle handle, const char *portName){
	std::string name=portName;
	comPort &port=comHandles.emplace_back(name,handle);
	std::thread reader(rpcThread, std::ref(port));//handle, portName);
	reader.detach();
//	printComPort(handle,"<ping>");
}

int enumeratePorts(){	
	auto ports = enumerateComPorts();
	for (const auto& port : ports) {
		const bool isPico=(port.devicePath.rfind("USB\\VID_2E8A",0)==0);
		if(isPico){
			comHandle h=openComPort(port.portName);
			if(h){
				std::cout << "[RPC] echoing " << port.portName << " handle:" << h << " path:" << port.devicePath << std::endl;
				echoComPort(h,port.portName.c_str());
			}else{
				std::cout << "[RPC] openComPort failure for " << port.portName << std::endl;
			}
		}else{
			std::cout << "[RPC] ignoring {";
			std::cout << "port:" << port.portName;
			std::cout << ",path:" << port.devicePath;
			std::cout << ",info:" << port.portDescription;
			std::cout << "}" << std::endl;
		}
	}
	return 0;
}
void printPorts(std::string line){
	for (auto& port : comHandles) {
//		printComPort(port.handle,line);
		port.print(line);
	}

}
int main() {
	HWND consoleWindow=GetConsoleWindow();
	std::cout << "rpccoms 0.3 looking for \"USB\\VID_2E8A\" from "<<((int64_t)consoleWindow)<<std::endl;
	enumeratePorts();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	TIME_ZONE_INFORMATION timezoneInformation;
	DWORD result = GetTimeZoneInformation(&timezoneInformation);
	LONG offsetMinutes = -timezoneInformation.Bias;
	long timezoneDelta = offsetMinutes * 60;
//	std::cout << "timezoneDelta:" << timezoneDelta << std::endl;

// setRTC jsonrp method
	auto now = std::chrono::system_clock::now();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
	std::string rtc = std::to_string(seconds+timezoneDelta);
	std::string setRTC = "{\"jsonrpc\":\"2.0\",\"method\":\"rtc.set\",\"params\":{\"time\":" + rtc + "},\"id\":1}";
//	std::string setRTC="{\"jsonrpc\":\"2.0\",\"method\":\"rtc.set\",\"params\":{\"time\":359155200},\"id\":1}\n";
	std::cout << "[RPC] setRTC:" << setRTC << std::endl;

	printPorts(setRTC);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	bool inReset=false;
	std::optional<std::string> lineValue;
	while(true){
		lineValue=rpcFifo.readLine();
		if(lineValue.has_value()){
			std::cout << "[RPC] line:" << (lineValue.value()) << std::endl;
		}else{
			std::this_thread::sleep_for(std::chrono::milliseconds(5));			
			pollMessages(consoleWindow);
		}
		bool shorty=anyKeyDown();
		if(shorty){
			std::cout << "[RPC] anykey:" << (shorty?1:0) << std::endl;
		}
		bool escape=GetAsyncKeyState(VK_ESCAPE)<0;
		if(escape) break;
		bool reset=GetAsyncKeyState(VK_SPACE)<0;
		if(reset){
			if(!inReset){
				std::cout << "[RPC] reset" << std::endl;
				inReset=true;
				scanPorts();
			}
		}else{
			inReset=false;
		}
	}

	std::cout << "[RPC] done" << std::endl;

	return 0;
}

bool anyKeyDown(){
	BYTE keys[256];
	if(!GetKeyboardState(keys)) return false;
	for(int i = 0; i < 256; i++){
		if(keys[i] & 0x80) return true;
	}
	return false;
}

void pollMessages(HWND hwnd){
	MSG msg;
	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
		std::cout << "[RPC] PeekMessage" << std::endl;
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	}
}
