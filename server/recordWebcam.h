#pragma once

#include <winsock2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <iostream>
#include <windows.h>
#include <fstream>


#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool isRecording = false;
IMFSinkWriter* pSinkWriter = nullptr;
DWORD streamIndex = 0;
string videoFileName = "recorded_video.mp4";


bool startRecording();
void stopRecording();
void sendVideoFile(const string& filename, SOCKET clientSocket);