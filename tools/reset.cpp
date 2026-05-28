// reset.cpp

#include "tools.h"
#include <iostream>

inline byteData toByteData(const std::string& str) {
    return byteData(str.begin(), str.end());
}

int main() {
	auto ports = enumerateComPorts();

	for (const auto& port : ports) {
//		if(port.portName.rfind("USB\\VID_2E8",0)==0){
		if(port.devicePath.find("USB\\VID_2E8")==0){
			byteData boot = toByteData("\nBOOT\n");
			comHandle h=openComPort(port.portName);
			if(h){
				std::cout << "RESETTING " << port.portName << " handle:" << h << std::endl;
				writeComPort(h,boot);
//				closeComPort(h);
			}else{
				std::cout << "openComPort failure for " << port.portName << std::endl;
			}
		}else{
			std::cout << "IGNORING" << std::endl;
			std::cout << "Port: " << port.portName << std::endl;
			std::cout << "Path: " << port.devicePath << std::endl;
			std::cout << "Info: " << port.portDescription << std::endl;
			std::cout << std::endl;
		}
	}

	return 0;
}
