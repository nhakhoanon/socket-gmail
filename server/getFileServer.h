#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "ui.h"

#pragma comment(lib, "Ws2_32.lib")
#define PORT 8080
#define BUFFER_SIZE 1024

#define CHUNK_SIZE 1024
void sendFile(const std::string& fileName, SOCKET clientSocket);
    