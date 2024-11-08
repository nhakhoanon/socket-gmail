#pragma once

#include <winsock2.h>
#include <vector>
#include <string>
#include <windows.h>
#include <map>
#include <fstream>
#include <iostream>

using namespace std;

struct Application {
    string title;
    DWORD pid; // Process ID
};

bool receiveStringVector(SOCKET socket, vector<string>& vec);
vector<Application> DeserializeApplications(const char* data);
bool receiveApplications(SOCKET socket, std::vector<Application>& apps);
bool receiveMap(SOCKET sock, std::map<DWORD, std::string>& data);
void receiveVideoFile(SOCKET serverSocket, const string& filename);