// windows com port junk 
// thanks to gemini for assists

#include "tools.h"

#include <iostream>

#include <windows.h>
#include <setupapi.h>
#include <devguid.h>

#include <initguid.h>
#include <usbdi.h>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "ole32.lib")

void *NULL_HANDLE=0;

bool initComPort(HANDLE hSerial){
	DCB dcb={sizeof(dcb)};
	if (!GetCommState(hSerial, &dcb)) {
		std::cerr << "[configureComPort] GetCommState failed, err=" << GetLastError() << std::endl;
		return false;
	}
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	BOOL ok=SetCommState(hSerial,&dcb);
	if(!ok){
		std::cerr << "[configureComPort] GetCommState failed, err=" << GetLastError() << std::endl;
		return false;
	}
	return true;
}

/*
	// Disable flow control unless explicitly required
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fTXContinueOnXoff = TRUE;

	// Keep binary mode, no parity checking
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;

	if (!SetCommState(hSerial, &dcb)) {
			std::cerr << "[configureComPort] SetCommState failed, err=" << GetLastError() << std::endl;
			return false;
	}
	COMMTIMEOUTS timeouts;
	ZeroMemory(&timeouts, sizeof(timeouts));

	// Read config (not critical for write-only use, but sane defaults)
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.ReadTotalTimeoutConstant = 100;

	// Important: bounded write time
	timeouts.WriteTotalTimeoutMultiplier = 10;   // ms per byte
	timeouts.WriteTotalTimeoutConstant = 500;    // base ms

	if (!SetCommTimeouts(hSerial, &timeouts)) {
			std::cerr << "[configureComPort] SetCommTimeouts failed, err=" << GetLastError() << std::endl;
			return false;
	}

	// Clear any stale buffers/state
	if (!PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT)) {
			std::cerr << "[configureComPort] PurgeComm failed, err=" << GetLastError() << std::endl;
	}

	// Explicitly apply modem control lines now
	if (!EscapeCommFunction(hSerial, SETDTR)) {
			std::cerr << "[configureComPort] SETDTR failed, err=" << GetLastError() << std::endl;
	}
	if (!EscapeCommFunction(hSerial, SETRTS)) {
			std::cerr << "[configureComPort] SETRTS failed, err=" << GetLastError() << std::endl;
	}


}
*/

void closeComPort(HANDLE hSerial) {
	FlushFileBuffers(hSerial);
	CloseHandle(hSerial);
}

void writeComPort(HANDLE hSerial,const byteData &data){
	DWORD bytesWritten = 0;
	BOOL success = WriteFile(hSerial,data.data(),static_cast<DWORD>(data.size()),&bytesWritten,NULL);
	if(success){
		std::cout << "[writeComPort] bytesWritten " << bytesWritten << std::endl;
	}else{
		std::cout << "[writeComPort] failure" << std::endl;
	}
}

HANDLE openComPort(const std::string& portName) {
	// Format as "\\.\COM11" to handle port numbers > 9 safely
	std::string formattedPath = "\\\\.\\" + portName;
	HANDLE hSerial = CreateFileA(formattedPath.c_str(),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if (hSerial == INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		std::cerr << "Error opening port " << portName << ". Code: " << error << std::endl;
		return NULL_HANDLE;
	}	

	bool init=initComPort(hSerial);
	return hSerial;
}

std::string ConvertToString(const wchar_t* wstr) {
	if (!wstr) return "";
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	std::string str(sizeNeeded - 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], sizeNeeded, NULL, NULL);
	return str;
}

std::vector<ComPortInfo> enumerateComPorts() {
	std::vector<ComPortInfo> ports;
	HDEVINFO deviceInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, NULL, NULL, DIGCF_PRESENT);
	if (deviceInfo == INVALID_HANDLE_VALUE) {
		return ports;
	}
	SP_DEVINFO_DATA devInfoData;
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfo, i, &devInfoData); i++) {
		wchar_t instanceId[MAX_PATH];       
		if (SetupDiGetDeviceInstanceIdW(deviceInfo, &devInfoData, instanceId, MAX_PATH, NULL)) {
			ComPortInfo portInfo;
			portInfo.devicePath = ConvertToString(instanceId);
			wchar_t friendlyNameBuf[MAX_PATH];
			if (SetupDiGetDeviceRegistryPropertyW(deviceInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)friendlyNameBuf, sizeof(friendlyNameBuf), NULL)) {
				portInfo.portDescription = ConvertToString(friendlyNameBuf);
			}

			HKEY hDeviceKey = SetupDiOpenDevRegKey(deviceInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
			if (hDeviceKey != INVALID_HANDLE_VALUE) {
				wchar_t portNameBuf[MAX_PATH];
				DWORD dwSize = sizeof(portNameBuf);
				DWORD dwType = 0;			
				LSTATUS status = RegQueryValueExW(hDeviceKey, L"PortName", NULL, &dwType, (PBYTE)portNameBuf, &dwSize);
				if (status == ERROR_SUCCESS && dwType == REG_SZ) {
					portInfo.portName = ConvertToString(portNameBuf);
				}
				RegCloseKey(hDeviceKey);
			}
			ports.push_back(portInfo);
		}
	}
	SetupDiDestroyDeviceInfoList(deviceInfo);
	return ports;
}
