#pragma once

#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <string>
#include "ui.h"

using namespace std;

bool closeApplication(DWORD pid);
DWORD FindPIDByImageName(const std::string &imageName);
