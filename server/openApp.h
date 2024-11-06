#pragma once

// #include <windows.h>
// #include <iostream>
// #include <tlhelp32.h>

#include <windows.h>
#include <string>
#include <iostream>

using namespace std;

// // Kiểm tra nếu tiến trình đã mở
// bool isProcessRunning(const std::string& processName);

// // Mở ứng dụng và kiểm tra trạng thái
// int openApp(const std::string& imageName);

int openApplicationByName(const std::string& fileName);