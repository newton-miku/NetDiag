//#include <iostream>
//#include <vector>
//#include <string>
//#include <stdexcept>
//#include <WinSock2.h>
//#include <IPHlpApi.h>
//#include <Ws2tcpip.h>
//
//#pragma comment(lib, "iphlpapi.lib")
//#pragma comment(lib, "ws2_32.lib")
//
//std::string toLowerCase(std::string str) {
//    for (char& c : str) {
//        c = tolower(c);
//    }
//    return str;
//}
//
//std::string formatSpeed(DWORD speed) {
//    const char* units[] = { "bps", "Kbps", "Mbps", "Gbps" };
//    int unitIndex = 0;
//
//    double convertedSpeed = static_cast<double>(speed);
//
//    while (convertedSpeed >= 1000 && unitIndex < 3) {
//        convertedSpeed /= 1000;
//        unitIndex++;
//    }
//
//    char buffer[20];
//    snprintf(buffer, sizeof(buffer), "%.1f %s", convertedSpeed, units[unitIndex]);
//
//    return std::string(buffer);
//}
//
//
//int getMaskBits(const std::string& subnetMask) {
//    int bits = 0;
//    unsigned long mask = inet_addr(subnetMask.c_str());
//
//    while (mask) {
//        bits += mask & 1;
//        mask >>= 1;
//    }
//
//    return bits;
//}
//
//void getMACandIP() {
//    try {
//        std::vector<IP_ADAPTER_INFO> adapters;
//        DWORD bufferSize = 0;
//
//        // Get the buffer size needed
//        if (GetAdaptersInfo(nullptr, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
//            adapters.resize(bufferSize);
//            PIP_ADAPTER_INFO adapterInfo = adapters.data();
//
//            // Get the adapter information
//            if (GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR) {
//                for (auto adapter = adapterInfo; adapter != nullptr; adapter = adapter->Next) {
//                    MIB_IFROW ifRow;
//                    ifRow.dwIndex = adapter->Index;
//                    if (GetIfEntry(&ifRow) == NO_ERROR) {
//                        if (ifRow.dwOperStatus == IF_OPER_STATUS_OPERATIONAL &&
//                            ifRow.dwType != IF_TYPE_SOFTWARE_LOOPBACK) {
//
//                            std::string descriptionLower = toLowerCase(adapter->Description);
//                            if (descriptionLower.find("virtual") == std::string::npos &&
//                                descriptionLower.find("tap") == std::string::npos &&
//                                descriptionLower.find("vpn") == std::string::npos) {
//                                std::cout << "网卡名称: " << adapter->Description << std::endl;
//                                std::cout << "\tIP地址: " << adapter->IpAddressList.IpAddress.String << std::endl;
//                                std::cout << "\t子网掩码: " << adapter->IpAddressList.IpMask.String << "/"
//                                    << getMaskBits(adapter->IpAddressList.IpMask.String) << std::endl;
//                                printf("\tMAC地址: %02X:%02X:%02X:%02X:%02X:%02X\n",
//                                    adapter->Address[0], adapter->Address[1], adapter->Address[2],
//                                    adapter->Address[3], adapter->Address[4], adapter->Address[5]);
//                                std::cout << "\t网关地址: " << adapter->GatewayList.IpAddress.String << std::endl;
//
//                                std::string speedBuffer = formatSpeed(ifRow.dwSpeed);
//                                std::cout << "\t速度: " << speedBuffer << std::endl;
//
//                                std::cout << "\tMTU: " << ifRow.dwMtu << std::endl << std::endl;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Exception caught: " << e.what() << std::endl;
//    }
//}
//
//int main() {
//    // Initialize Winsock
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        std::cerr << "Failed to initialize Winsock" << std::endl;
//        return 1;
//    }
//
//    getMACandIP();
//
//    // Cleanup Winsock
//    WSACleanup();
//
//    return 0;
//}
//#pragma once
