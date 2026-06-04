// resetcoms.cpp

// 0.1 send reboot command to all com ports with path "USB\\VID_2E8" 

#include <iostream>
#include "tools.h"




void echoComPort(comHandle h){

}

inline byteData toBytes(const std::string& str) {
	return byteData(str.begin(), str.end());
}

int main() {
	std::cout << "echocoms 0.1 looking for \"USB\\VID_2E8\""<<std::endl;
	auto ports = enumerateComPorts();
	for (const auto& port : ports) {
		if(port.devicePath.rfind("USB\\VID_2E8",0)==0){
			comHandle h=openComPort(port.portName);
			if(h){
				std::cout << "echoing " << port.portName << " handle:" << h << std::endl;
				echoComPort(h);
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
	return 0;
}
