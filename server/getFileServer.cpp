#include "getFileServer.h"

void sendFile(const std::string& fileName, SOCKET clientSocket) {
    std::ifstream inFile(fileName, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Cannot open file!" << std::endl;
        return;
    }

    char buffer[CHUNK_SIZE];
    while (inFile.read(buffer, CHUNK_SIZE) || inFile.gcount() > 0) {
        int bytesSent = send(clientSocket, buffer, inFile.gcount(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Error!" << std::endl;
            break;
        }
    }

    std::cout << "Send file successfully!" << std::endl;
    inFile.close();
}