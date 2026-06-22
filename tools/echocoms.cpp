// echocoms.cpp

// 0.1 sends <ping> prints reply

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

void readerThread(comHandle h, const std::string& portName){
	const size_t bufSize = 4096;
	auto buffer = std::make_unique<uint8_t[]>(bufSize);
	std::cout << "[thread] starting reader for " << portName << std::endl;
	while (true){
		int n = readComPort(h, buffer.get(), bufSize - 1);
		if (n > 0){
			buffer[n] = 0;
			std::cout << portName << " received: " << (char*)buffer.get() << std::endl;
		}
		else if (n == 0)
		{
			std::cout << portName << " disconnected." << std::endl;
			break;
		}
		else
		{
			std::cout << portName << " read error." << std::endl;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void echoComPort(comHandle handle, const char *portName){
	std::thread reader(readerThread, handle, portName);
	reader.detach();
	printComPort(handle,"<ping>\n");
}

int main() {
	std::cout << "echocoms 0.1 looking for \"USB\\VID_2E8\""<<std::endl;
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

	std::this_thread::sleep_for(std::chrono::seconds(10));

	return 0;
}
