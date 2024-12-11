#pragma once

#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <map>

using namespace std;

struct Application {
    string title;
    string fileName;
    DWORD pid; // Process ID
};


bool sendStringVector(SOCKET socket, const std::vector<std::string>& vec);
bool sendMap(SOCKET sock, const std::map<DWORD, std::string>& data);
vector<char> SerializeApplications(const std::vector<Application>& apps);
bool sendApplications(SOCKET socket, const std::vector<Application>& apps);