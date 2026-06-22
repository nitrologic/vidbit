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

bool anyKeyDown();
void pollMessages(HWND targetWindow);

inline byteData toBytes(const std::string& str) {
	return byteData(str.begin(), str.end());
}

bool printComPort(comHandle handle,const std::string &text){
	const byteData data=toBytes(text);
	bool success=writeComPort(handle,data);
	return success;
}

int readComPort(comHandle handle, uint8_t* buffer, size_t maxSize){
	HANDLE h = (HANDLE)handle;
	DWORD bytesRead = 0;
	if (!ReadFile(h, buffer, (DWORD)maxSize, &bytesRead, NULL)) return -1;
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
		buffer.append(reinterpret_cast<char*>(bytes), count);
	}

	// readLine() skips empty lines
	std::optional<std::string> readLine() {
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

void rpcThread(comHandle h, const std::string& portName){
	const size_t bufSize = 4096;
	auto buffer = std::make_unique<uint8_t[]>(bufSize);
	std::cout << "[thread] starting reader for " << portName << std::endl;
	while (true){
		auto bptr=buffer.get();
		int n = readComPort(h, bptr, bufSize - 1);
		if (n > 0){
			rpcReceive(bptr,n);
//			buffer[n] = 0;
//			std::cout << portName << " received: " << (char*)buffer.get() << std::endl;
		}
		else if (n == -1)
		{
			std::cout << portName << " disconnected." << std::endl;
			exit(0);
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

std::vector<comHandle> comHandles={};

void echoComPort(comHandle handle, const char *portName){
	comHandles.push_back(handle);
	std::thread reader(rpcThread, handle, portName);
	reader.detach();
	printComPort(handle,"<ping>\n");
}

int main() {
	HWND consoleWindow=GetConsoleWindow();
	std::cout << "rpccoms 0.2 looking for \"USB\\VID_2E8A\" from "<<((int64_t)consoleWindow)<<std::endl;
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
	std::string setRTC = "{\"jsonrpc\":\"2.0\",\"method\":\"rtc.set\",\"params\":{\"time\":" + rtc + "},\"id\":1}\n";
//	std::string setRTC="{\"jsonrpc\":\"2.0\",\"method\":\"rtc.set\",\"params\":{\"time\":359155200},\"id\":1}\n";
	std::cout << "[RPC] " << setRTC << std::endl;
	for (const auto& handle : comHandles) {
//		printComPort(handle,"<ping>\n");
		printComPort(handle,setRTC);
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

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
		bool escape=GetAsyncKeyState(VK_ESCAPE)!=0;
		if(escape) break;
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
