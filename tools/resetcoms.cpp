// resetcoms.cpp

// 0.1 send reboot command to all com ports with path "USB\\VID_2E8" 

#include <iostream>
#include "tools.h"

inline byteData toBytes(const std::string& str) {
    return byteData(str.begin(), str.end());
}

int main() {
	std::cout << "resetcoms 0.2 looking for \"USB\\VID_2E8\" or \"USB\\VID_1209\""<<std::endl;
	auto ports = enumerateComPorts();
	for (const auto& port : ports) {
		bool isPico=(port.devicePath.rfind("USB\\VID_2E8",0)==0) || (port.devicePath.rfind("USB\\VID_1209",0)==0);
		if(isPico){
			byteData boot = toBytes("\nBOOT\n");
			comHandle h=openComPort(port.portName);
			if(h){
				std::cout << "resetting " << port.portName << " handle:" << h << std::endl;
				writeComPort(h,boot);
//				closeComPort(h);
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
