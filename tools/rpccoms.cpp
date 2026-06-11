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

inline byteData toBytes(const std::string& str) {
	return byteData(str.begin(), str.end());
}

void printComPort(comHandle handle,const std::string &text){
	const byteData data=toBytes(text);
	writeComPort(handle,data);
}

int readComPort(comHandle handle, uint8_t* buffer, size_t maxSize){
	HANDLE h = (HANDLE)handle;
	DWORD bytesRead = 0;
	// Set a short timeout (100 ms)
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	SetCommTimeouts(h, &timeouts);
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
			pos = buffer.find("\r\n",2);
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
			buffer[n] = 0;
			std::cout << portName << " received: " << (char*)buffer.get() << std::endl;
		}
		else if (n == -1)
		{
			std::cout << portName << " disconnected." << std::endl;
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
	std::cout << "rpccoms 0.1 looking for \"USB\\VID_2E8\""<<std::endl;
	auto ports = enumerateComPorts();
	for (const auto& port : ports) {
		if(port.devicePath.rfind("USB\\VID_2E8",0)==0){
			comHandle h=openComPort(port.portName);
			if(h){
				std::cout << "echoing " << port.portName << " handle:" << h << std::endl;
				echoComPort(h,port.portName.c_str());
			}else{
				std::cout << "openComPort failure for " << port.portName << std::endl;
			}
		}else{
			std::cout << "ignoring {";
			std::cout << "port:" << port.portName;
			std::cout << ",path:" << port.devicePath;
			std::cout << ",info:" << port.portDescription;
			std::cout << "}" << std::endl;
		}
	}

   	std::this_thread::sleep_for(std::chrono::seconds(1));

	for (const auto& handle : comHandles) {
		printComPort(handle,"<ping>\n");
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));

	std::optional<std::string> line;
	while(line=rpcFifo.readLine()){        
		std::cout << "readline:" << line.value() << std::endl;
	}
	return 0;
}
