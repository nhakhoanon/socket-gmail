#pragma once

#include <winsock2.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <atomic>
#include <thread>



#define CHUNK_SIZE 1024

using namespace std;

extern std::atomic<bool> stopFlag;  // Được khai báo ngoài, trong phần khác của chương trình

void sendFile(const std::string& videoFilename, SOCKET clientSocket);
void recordVideo(const std::string& outputFilename, int width = 640, int height = 480, int fps = 30);
void stopRecord();
void resetFlag();