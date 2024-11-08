#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
#define PORT 8080
#define BUFFER_SIZE 1024

void sendFile2(SOCKET clientSocket, const std::string& filePath);
    