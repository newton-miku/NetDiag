#include <iostream>
#include <vector>
#include <stdexcept>
#include <WinSock2.h>
#include <Windows.h>
#include <IPHlpApi.h>
#include <Wlanapi.h>
#include <string>
#include <fstream>
#include <codecvt>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wlanapi.lib")

struct NetworkDeviceInfo {
	std::wstring description;
	std::wstring driverVersion;
	std::wstring driverDate;
	std::wstring deviceID;
};

std::vector<NetworkDeviceInfo> GetActiveNetworkDevices() {
	std::vector<NetworkDeviceInfo> activeDevices;

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD index = 0;
		WCHAR subKeyName[256];
		DWORD subKeyNameLen = sizeof(subKeyName) / sizeof(WCHAR);

		while (RegEnumKeyEx(hKey, index++, subKeyName, &subKeyNameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
			HKEY subKey;
			std::wstring subKeyPath = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}\\" + std::wstring(subKeyName);

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, KEY_READ, &subKey) == ERROR_SUCCESS) {
				WCHAR driverVersion[256];
				DWORD driverVersionLen = sizeof(driverVersion);
				WCHAR description[256];
				DWORD descriptionLen = sizeof(description);
				WCHAR driverDate[256];
				DWORD driverDateLen = sizeof(driverDate);
				WCHAR deviceID[256];

				if (RegQueryValueEx(subKey, L"DriverVersion", NULL, NULL, reinterpret_cast<LPBYTE>(driverVersion), &driverVersionLen) == ERROR_SUCCESS &&
					RegQueryValueEx(subKey, L"DriverDate", NULL, NULL, reinterpret_cast<LPBYTE>(driverDate), &driverDateLen) == ERROR_SUCCESS &&
					RegQueryValueEx(subKey, L"DriverDesc", NULL, NULL, reinterpret_cast<LPBYTE>(description), &descriptionLen) == ERROR_SUCCESS
					) {
					NetworkDeviceInfo deviceInfo;
					deviceInfo.description = description;
					deviceInfo.driverVersion = driverVersion;
					deviceInfo.driverDate = driverDate;
					deviceInfo.deviceID = subKeyName;

					activeDevices.push_back(deviceInfo);
				}

				RegCloseKey(subKey);
			}

			subKeyNameLen = sizeof(subKeyName) / sizeof(WCHAR);
		}

		RegCloseKey(hKey);
	}

	return activeDevices;
}

bool ShouldOutputNetworkDevice(const std::wstring& description) {
	std::vector<std::wstring> keywordsToExclude = { L"virtual", L"vpn", L"tap" };
	for (const auto& keyword : keywordsToExclude) {
		if (description.find(keyword) != std::wstring::npos) {
			return false;
		}
	}
	return true;
}
bool ShouldOutputNetworkDevice(const std::string& description) {
	std::vector<std::string> keywordsToExclude = { "virtual", "vpn", "tap" };
	for (const auto& keyword : keywordsToExclude) {
		if (description.find(keyword) != std::string::npos) {
			return false;
		}
	}
	return true;
}


std::string toLowerCase(std::string str) {
	for (char& c : str) {
		c = tolower(c);
	}
	return str;
}

int compareStringAndWstring(const std::string& str, const std::wstring& wstr) {
	// 将 std::string 转换为 std::wstring
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring convertedStr = converter.from_bytes(str);

	// 比较转换后的 std::wstring 和给定的 std::wstring
	return convertedStr.compare(wstr);
}
std::string formatSpeed(DWORD speed) {
	const char* units[] = { "bps", "Kbps", "Mbps", "Gbps" };
	int unitIndex = 0;

	double convertedSpeed = static_cast<double>(speed);

	while (convertedSpeed >= 1000 && unitIndex < 3) {
		convertedSpeed /= 1000;
		unitIndex++;
	}

	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%.1f %s", convertedSpeed, units[unitIndex]);

	return std::string(buffer);
}

int getMaskBits(const std::string& subnetMask) {
	int bits = 0;
	unsigned long mask = inet_addr(subnetMask.c_str());

	while (mask) {
		bits += mask & 1;
		mask >>= 1;
	}

	return bits;
}

int freqTochannel(__in ULONG freq)
{
	int channel = 0;
	if ((freq >= 57240) && (freq <= 70200)) {    // # 60G
		channel = (freq - 58320) / 2160 + 1;
	}
	else if ((freq >= 5925) && (freq <= 7125)) { // : # 6G
		//# 6G, 5.925~7.125, 5955 = ch1, 5975 = ch5
		channel = (freq - 5950) / 5;
	}
	else if ((freq >= 5180) && (freq <= 5825)) { // : # 5G
		channel = (freq - 5150) / 5 + 30;
	}
	else if ((freq >= 2412) && (freq <= 2477)) { // : # 2.4G
		channel = (freq - 2412) / 5 + 1;
	}
	else {
		std::wcout << "Can not conver frequence " << freq << " to channel";
	}
	return channel;
}

int WlanSCAN() {
	DWORD dwResult = 0;
	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;
	WCHAR GuidString[32] = { 0 };
	HANDLE _wlanHandle = NULL;
	PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
	PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;
	DWORD dwCurVersion = 0;
	DWORD dwMaxClient = 2;
	if (_wlanHandle == NULL)
	{
		if ((dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &_wlanHandle)) != ERROR_SUCCESS)
		{
			std::cout << "wlanOpenHandle failed with error: " << dwResult << std::endl;
			_wlanHandle = NULL;
			return false;
		}
	}
	// 释放内存
	if (pBssList != NULL)
	{
		WlanFreeMemory(pBssList);
		pBssList = NULL;
	}

	if (pIfList != NULL)
	{
		WlanFreeMemory(pIfList);
		pIfList = NULL;
	}

	unsigned int i, j;

	dwResult = WlanEnumInterfaces(_wlanHandle, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS)
	{
		return false;
	}
	else
	{
		for (i = 0; i < (int)pIfList->dwNumberOfItems; i++)
		{
			pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];

			dwResult = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
				sizeof(GuidString) / sizeof(*GuidString));
			// 向无线网卡发送探测请求
			dwResult = WlanScan(_wlanHandle, (const GUID*)(&pIfInfo->InterfaceGuid), NULL, NULL, NULL);
			if (dwResult != ERROR_SUCCESS)
			{
				return false;
			}
		}
	}
}
std::string wlan_phy_type(int type) {
	switch (type)
	{
	case dot11_phy_type_eht:return("WIFI 7"); break;
	case dot11_phy_type_he:return("WIFI 6"); break;
	case dot11_phy_type_dmg:return("WiGig(802.11ad/60 GHz)"); break;
	case dot11_phy_type_vht:return("WIFI 5"); break;
	case dot11_phy_type_ht:return("WIFI 4"); break;
	case dot11_phy_type_erp:return("WIFI g"); break;
	case dot11_phy_type_hrdsss:return("802.11b"); break;
	case dot11_phy_type_ofdm:return("802.11a"); break;
	default:char str[15]; sprintf(str, "其他：%lu", type); return(str);
		break;
	}
}
int GetInfo()
{
	int id = 0;
	HANDLE ClientHandle;
	DWORD nv, i, j;
	PWLAN_INTERFACE_INFO_LIST ilist;
	PWLAN_AVAILABLE_NETWORK_LIST nlist;
	//static char ssid[36];
	wchar_t ssid[36]; // 使用宽字符数组存储SSID
	setlocale(LC_ALL, "chs");
	WlanSCAN();
	Sleep(2000);
	if (WlanOpenHandle(1, 0, &nv, &ClientHandle) == 0)
	{
		if (WlanEnumInterfaces(ClientHandle, 0, &ilist) == 0)
		{
			for (i = 0; i < ilist->dwNumberOfItems; i++)
			{
				std::cout << "\n以下是WIFI信息\n" << std::endl;
				wprintf(L"网卡: %s\n\n", ilist->InterfaceInfo[i].strInterfaceDescription);
				if (WlanGetAvailableNetworkList(ClientHandle, &ilist->InterfaceInfo[i].InterfaceGuid, 0, 0, &nlist) == 0)
				{
					for (j = 0; j < nlist->dwNumberOfItems; j++)
					{
						/*memcpy(ssid, nlist->Network[j].dot11Ssid.ucSSID, nlist->Network[j].dot11Ssid.uSSIDLength);
						ssid[nlist->Network[j].dot11Ssid.uSSIDLength] = 0;
						printf("%d. SSID:  %-25s  信号强度:  %5d\n", id++, ssid, nlist->Network[j].wlanSignalQuality);*/
						memset(ssid, 0, sizeof(ssid));
						MultiByteToWideChar(CP_UTF8, 0, (LPCCH)nlist->Network[j].dot11Ssid.ucSSID,
							nlist->Network[j].dot11Ssid.uSSIDLength, ssid, 30);
						ssid[nlist->Network[j].dot11Ssid.uSSIDLength] = L'\0';
						wprintf(L"%-2d. SSID: %-35s\t信号强度:%3d%\t数量:%lu ", id++, ssid, nlist->Network[j].wlanSignalQuality, nlist->Network[j].uNumberOfBssids);
						wprintf(L"[%lu]%-5s", nlist->Network[j].dwFlags, nlist->Network[j].dwFlags == (WLAN_AVAILABLE_NETWORK_CONNECTED | WLAN_AVAILABLE_NETWORK_HAS_PROFILE) ? L"已连接" : (nlist->Network[j].dwFlags == (WLAN_AVAILABLE_NETWORK_HAS_PROFILE) ? L"已保存" : L" "));
						if (nlist->Network[j].bMorePhyTypes)
						{
							std::cout << wlan_phy_type(nlist->Network[j].uNumberOfPhyTypes);
						}
						else
						{
							for (int type : nlist->Network[j].dot11PhyTypes)
							{
								if (type < dot11_phy_type_ofdm || type > dot11_phy_type_eht) continue;
								std::cout << wlan_phy_type(type);
								break;
							}
						}
						printf("\n");
						//可继续获取BSSID
						if (1)
						{
							PWLAN_BSS_LIST ppWlanBssList; //BSS网络列表
							auto dwResult = WlanGetNetworkBssList(
								ClientHandle,
								&ilist->InterfaceInfo[i].InterfaceGuid,
								&nlist->Network[j].dot11Ssid, nlist->Network[j].dot11BssType,
								nlist->Network[j].bSecurityEnabled,
								NULL, &ppWlanBssList);

							if (ERROR_SUCCESS == dwResult)
							{
								for (int k = 0; k < ppWlanBssList->dwNumberOfItems; k++)
								{
									auto listNode = ppWlanBssList->wlanBssEntries[k];
									printf("\t%3 BSSID:");
									printf("%02X:%02X:%02X:%02X:%02X:%02X",
										listNode.dot11Bssid[0],
										listNode.dot11Bssid[1],
										listNode.dot11Bssid[2],
										listNode.dot11Bssid[3],
										listNode.dot11Bssid[4],
										listNode.dot11Bssid[5]
									);
									printf("\tRSSI:%ld dBm,质量:%lu", listNode.lRssi, listNode.uLinkQuality);
									//printf("\t%lu", listNode.ulChCenterFrequency / 1000);
									printf("\t信道:%-4d", freqTochannel(listNode.ulChCenterFrequency / 1000));
									//std::cout << "[" << wlan_phy_type(listNode.dot11BssPhyType) << "]";
									printf("\n");
								}
							}
							printf("\n");
						}
					}
					WlanFreeMemory(nlist);
				}
			}
			WlanFreeMemory(ilist);
		}
		WlanCloseHandle(ClientHandle, 0);
	}
	return true;
}
void getMACandIP() {
	try {
		std::vector<IP_ADAPTER_INFO> adapters;
		DWORD bufferSize = 0;

		// Get the buffer size needed
		if (GetAdaptersInfo(nullptr, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
			adapters.resize(bufferSize);
			PIP_ADAPTER_INFO adapterInfo = adapters.data();
			auto activeDevices = GetActiveNetworkDevices();
			// Get the adapter information
			if (GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR) {
				for (auto adapter = adapterInfo; adapter != nullptr; adapter = adapter->Next) {
					MIB_IFROW ifRow;
					ifRow.dwIndex = adapter->Index;
					if (GetIfEntry(&ifRow) == NO_ERROR) {
						if (ifRow.dwOperStatus == IF_OPER_STATUS_OPERATIONAL &&
							ifRow.dwType != IF_TYPE_SOFTWARE_LOOPBACK) {

							std::string descriptionLower = toLowerCase(adapter->Description);
							std::string DescriptionStr = adapter->Description;
							//if (descriptionLower.find("virtual") == std::string::npos &&
							//	descriptionLower.find("tap") == std::string::npos &&
							//	descriptionLower.find("vpn") == std::string::npos)
							auto it = std::find_if(activeDevices.begin(), activeDevices.end(),
								[&](const NetworkDeviceInfo& device) {
									return compareStringAndWstring(DescriptionStr, device.description) == 0;
								});
							if (ShouldOutputNetworkDevice(descriptionLower))
							{
								std::cout << "网卡名称: " << adapter->Description << std::endl;
								if (it != activeDevices.end()) {
									std::wcout << L"\t驱动版本: " << it->driverVersion << std::endl;
									std::wcout << L"\t驱动日期: " << it->driverDate << std::endl << std::endl;
								}
								switch (ifRow.dwType)
								{
								case IF_TYPE_IEEE80211:
									std::cout << "\t网卡类型: 无线网卡" << std::endl; break;
								case IF_TYPE_ETHERNET_CSMACD:
									std::cout << "\t网卡类型: 有线网卡" << std::endl; break;
								default:
									std::cout << "\t网卡类型: 其他" << std::endl;
									break;
								}
								std::cout << "\tIP地址: " << adapter->IpAddressList.IpAddress.String << std::endl;
								std::cout << "\t子网掩码: " << adapter->IpAddressList.IpMask.String << "/"
									<< getMaskBits(adapter->IpAddressList.IpMask.String) << std::endl;
								printf("\tMAC地址: %02X:%02X:%02X:%02X:%02X:%02X\n",
									adapter->Address[0], adapter->Address[1], adapter->Address[2],
									adapter->Address[3], adapter->Address[4], adapter->Address[5]);
								std::cout << "\t网关地址: " << adapter->GatewayList.IpAddress.String << std::endl;

								std::string speedBuffer = formatSpeed(ifRow.dwSpeed);
								std::cout << "\t速度: " << speedBuffer << std::endl;

								std::cout << "\tMTU: " << ifRow.dwMtu << std::endl;

								// Check if the adapter is a wireless interface
								if (ifRow.dwType == IF_TYPE_IEEE80211) {
									GetInfo();
								}

								std::cout << std::endl;
							}
						}
					}
				}
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	}
}

int main() {
	// Initialize Winsock
	setlocale(LC_ALL, "chs");
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize Winsock" << std::endl;
		return 1;
	}
	getMACandIP();


	// Cleanup Winsock
	WSACleanup();

	return 0;
}
