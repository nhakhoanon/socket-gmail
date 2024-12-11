#pragma once

#include <winsock2.h>
#include <vector>
#include <string>
#include <windows.h>
#include <map>
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

#define CHUNK_SIZE 1024

struct Application {
    string title;
    string fileName;
    DWORD pid; // Process ID
};

bool receiveStringVector(SOCKET socket, vector<string>& vec);
vector<Application> DeserializeApplications(const char* data);
bool receiveApplications(SOCKET socket, std::vector<Application>& apps);
bool receiveMap(SOCKET sock, std::map<DWORD, std::string>& data);
void receiveFile(SOCKET serverSocket, const std::string& outputFilename);
bool comparePID(Application a, Application b);