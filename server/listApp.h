#pragma once

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include "sendData.h"
#include <array>
#include <memory>
#include <sstream>
#include <cstdio>
#include <windows.h>
#include <cwchar>

using namespace std;

string SanitizeWindowTitle(const std::string& title);
string wcharToUtf8(const wchar_t* wstr);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
vector<Application> GetOpenApplications();
vector<Application> DeserializeApplications(const char* data);
string getImageNameFromPID(DWORD pid);