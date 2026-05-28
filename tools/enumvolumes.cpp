#include <iostream>
#include <vector>
#include <string>

#include <windows.h>
#include <setupapi.h>
#include <devguid.h>

#include <initguid.h>
#include <usbdi.h>


// INFO_UF2.TXT
// INDEX.HTM
// Volume in drive E is RPI-RP2
// Volume Serial Number is 0003-36AB

struct VolumeInfo {
    std::string volumeGuidPath; // e.g., "\\?\Volume{guid}\"
    std::string driveLetter;    // e.g., "C:\" (can be empty if unassigned)
    std::string fileSystem;     // e.g., "NTFS" or "FAT32"
    std::string serialNumber;
    std::string volumeName;
};

// Helper helper to convert Windows WCHAR strings to std::string
std::string WStringToString(const wchar_t* wstr) {
    if (!wstr) return "";
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    std::string str(sizeNeeded - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], sizeNeeded, NULL, NULL);
    return str;
}

std::vector<VolumeInfo> enumerateStorageVolumes() {
    std::vector<VolumeInfo> volumes;
    wchar_t volumeNameBuf[MAX_PATH];
    
    // Begin volume enumeration scanning
    HANDLE hVol = FindFirstVolumeW(volumeNameBuf, MAX_PATH);
    if (hVol == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to start volume scanning. Error: " << GetLastError() << std::endl;
        return volumes;
    }

    do {
        VolumeInfo volInfo;
        volInfo.volumeGuidPath = WStringToString(volumeNameBuf);

        // Fetch associated drive letters/mount points
        wchar_t pathNamesBuf[MAX_PATH];
        DWORD returnLength = 0;
        if (GetVolumePathNamesForVolumeNameW(volumeNameBuf, pathNamesBuf, MAX_PATH, &returnLength)) {
            // pathNamesBuf contains a null-separated multi-string list terminated by a double null.
            // We take the first primary mount point if it exists.
            if (returnLength > 0 && pathNamesBuf[0] != L'\0') {
                volInfo.driveLetter = WStringToString(pathNamesBuf);
            }
        }

        // Fetch File System metadata (e.g., NTFS, FAT32)
        wchar_t fsNameBuf[MAX_PATH] = { 0 };

//        if (GetVolumeInformationW(volumeNameBuf, NULL, 0, NULL, NULL, NULL, fsNameBuf, MAX_PATH)) {
  //          volInfo.fileSystem = WStringToString(fsNameBuf);
    //    }


        wchar_t volumeLabelBuf[MAX_PATH] = { 0 };
        DWORD rawSerialNumber = 0;

        if (GetVolumeInformationW(
            volumeNameBuf,         // Input: Volume GUID path
            volumeLabelBuf,        // Output: Volume Name (e.g., "RPI-RP2")
            MAX_PATH, 
            &rawSerialNumber,      // Output: Numeric Serial Number
            NULL, 
            NULL, 
            fsNameBuf,             // Output: File System (e.g., "FAT")
            MAX_PATH)) 
        {
            volInfo.fileSystem = WStringToString(fsNameBuf);
            volInfo.volumeName = WStringToString(volumeLabelBuf);

            // Format the 32-bit DWORD into standard XXXX-XXXX hex notation
            char serialFormatBuf[10];
            snprintf(serialFormatBuf, sizeof(serialFormatBuf), "%04X-%04X", 
                     HIWORD(rawSerialNumber),  // High 16-bits (e.g., 0003)
                     LOWORD(rawSerialNumber)); // Low 16-bits  (e.g., 36AB)
            volInfo.serialNumber = serialFormatBuf;
        }

        volumes.push_back(volInfo);

    } while (FindNextVolumeW(hVol, volumeNameBuf, MAX_PATH));

    FindVolumeClose(hVol);
    return volumes;
}

int main() {
    auto storageVolumes = enumerateStorageVolumes();
    for (const auto& vol : storageVolumes) {
        std::cout << "Volume: " << vol.volumeGuidPath << "\n";
        std::cout << "  Mount Point: " << (vol.driveLetter.empty() ? "None" : vol.driveLetter) << "\n";
        std::cout << "  File System: " << (vol.fileSystem.empty() ? "Unknown" : vol.fileSystem) << "\n";
        std::cout << "  Volume Name: " << vol.volumeName << "\n";
        std::cout << "  Serial Number: " << vol.serialNumber << "\n";
        std::cout << std::endl;
    }
    return 0;
}