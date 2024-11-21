#pragma once

#include <winsock2.h>
// #include <mfapi.h>
// #include <mfidl.h>
// #include <mfobjects.h>
// #include <mfplay.h>
// #include <mfreadwrite.h>
// #include <mferror.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <atomic>
#include <thread>



// #pragma comment(lib, "mfplat.lib")
// #pragma comment(lib, "mfreadwrite.lib")
// #pragma comment(lib, "mfuuid.lib")
// #pragma comment(lib, "mf.lib")
// #pragma comment(lib, "ws2_32.lib")
#define CHUNK_SIZE 1024

using namespace std;

// extern bool isRecording;
// extern IMFSinkWriter* pSinkWriter;
// extern DWORD streamIndex;
// extern string videoFileName;

extern std::atomic<bool> stopFlag;  // Được khai báo ngoài, trong phần khác của chương trình

void sendFile(const std::string& videoFilename, SOCKET clientSocket);
void recordVideo(const std::string&, int, int);
void stopRecord();
void resetFlag();