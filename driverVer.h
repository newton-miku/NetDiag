//#include <iostream>
//#include <Windows.h>
//#include <vector>
//#include <string>
//#pragma once
//struct NetworkDeviceInfo {
//    std::wstring description;
//    std::wstring driverVersion;
//    std::wstring driverDate;
//    std::wstring deviceID;
//};
//
//std::vector<NetworkDeviceInfo> GetActiveNetworkDevices() {
//    std::vector<NetworkDeviceInfo> activeDevices;
//
//    HKEY hKey;
//    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
//        DWORD index = 0;
//        WCHAR subKeyName[256];
//        DWORD subKeyNameLen = sizeof(subKeyName) / sizeof(WCHAR);
//
//        while (RegEnumKeyEx(hKey, index++, subKeyName, &subKeyNameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
//            HKEY subKey;
//            std::wstring subKeyPath = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}\\" + std::wstring(subKeyName);
//
//            if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, KEY_READ, &subKey) == ERROR_SUCCESS) {
//                WCHAR driverVersion[256];
//                DWORD driverVersionLen = sizeof(driverVersion);
//                WCHAR description[256];
//                DWORD descriptionLen = sizeof(description);
//                WCHAR driverDate[256];
//                DWORD driverDateLen = sizeof(driverDate);
//                WCHAR deviceID[256];
//
//                if (RegQueryValueEx(subKey, L"DriverVersion", NULL, NULL, reinterpret_cast<LPBYTE>(driverVersion), &driverVersionLen) == ERROR_SUCCESS &&
//                    RegQueryValueEx(subKey, L"DriverDate", NULL, NULL, reinterpret_cast<LPBYTE>(driverDate), &driverDateLen) == ERROR_SUCCESS &&
//                    RegQueryValueEx(subKey, L"DriverDesc", NULL, NULL, reinterpret_cast<LPBYTE>(description), &descriptionLen) == ERROR_SUCCESS
//                    ) {
//                    NetworkDeviceInfo deviceInfo;
//                    deviceInfo.description = description;
//                    deviceInfo.driverVersion = driverVersion;
//                    deviceInfo.driverDate = driverDate;
//                    deviceInfo.deviceID = subKeyName;
//
//                    activeDevices.push_back(deviceInfo);
//                }
//
//                RegCloseKey(subKey);
//            }
//
//            subKeyNameLen = sizeof(subKeyName) / sizeof(WCHAR);
//        }
//
//        RegCloseKey(hKey);
//    }
//
//    return activeDevices;
//}
//
//bool ShouldOutputNetworkDevice(const std::wstring& description) {
//    std::vector<std::wstring> keywordsToExclude = { L"virtual", L"vpn", L"tap" };
//    for (const auto& keyword : keywordsToExclude) {
//        if (description.find(keyword) != std::wstring::npos) {
//            return false;
//        }
//    }
//    return true;
//}
//bool ShouldOutputNetworkDevice(const std::string& description) {
//    std::vector<std::string> keywordsToExclude = { "virtual", "vpn", "tap" };
//    for (const auto& keyword : keywordsToExclude) {
//        if (description.find(keyword) != std::string::npos) {
//            return false;
//        }
//    }
//    return true;
//}
