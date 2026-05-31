#include "udev.h"

Devices devices;
int main(){

//	devices.jsonifyDevices(Strings attributeMask,std::string& result){

	int error=devices.enumInputs("input");
	std::cout << "enuminputs : " << error << std::endl;

	return 0;
}