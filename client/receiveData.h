#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include <map>

using namespace std;

struct Application {
    string title;
    DWORD pid; // Process ID
};

bool receiveStringVector(SOCKET socket, vector<string>& vec);
vector<Application> DeserializeApplications(const char* data);
bool receiveApplications(SOCKET socket, std::vector<Application>& apps);
bool receiveMap(SOCKET sock, std::map<DWORD, std::string>& data);