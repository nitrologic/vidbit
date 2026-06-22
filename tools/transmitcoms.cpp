// transmitcoms.cpp

// 0.1 transmit command to all com ports with path "USB\\VID_2E8" 

#include <iostream>
#include "tools.h"

inline byteData toBytes(const std::string& str) {
    return byteData(str.begin(), str.end());
}

int main(int argc, char* argv[]) {
    if(argc<2){
    	std::cout << "usage: transmitcoms payload"<<std::endl;
        return 1;
    }
    std::string payload=std::string("\n")+argv[1]+"\n";
	std::cout << "transmitcoms 0.21 looking for \"USB\\VID_2E8\" or \"USB\\VID_1209\""<<std::endl;
	auto ports = enumerateComPorts();
	for (const auto& port : ports) {
		bool isPico=(port.devicePath.rfind("USB\\VID_2E8",0)==0) || (port.devicePath.rfind("USB\\VID_1209",0)==0);
		if(isPico){
			byteData bytes = toBytes(payload);
			comHandle h=openComPort(port.portName);
			if(h){
				std::cout << "sending " << port.portName << " handle:" << h << " " << payload << std::endl;
				writeComPort(h,bytes);
				closeComPort(h);
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
