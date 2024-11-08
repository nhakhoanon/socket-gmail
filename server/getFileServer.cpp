#include "getFileServer.h"

void sendFile2(SOCKET clientSocket, const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::string response = "NOT_FOUND";
        send(clientSocket, response.c_str(), response.size(), 0);
        return;
        // return false;
    }

    // Send FOUND message
    std::string response = "FOUND";
    send(clientSocket, response.c_str(), response.size(), 0);

    // Send file name
    std::string fileName = filePath.substr(filePath.find_last_of("/\\") + 1);
    send(clientSocket, fileName.c_str(), fileName.size(), 0);
    Sleep(100); // Small delay to ensure client processes the file name

    // Send file content
    char buffer[BUFFER_SIZE];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        send(clientSocket, buffer, file.gcount(), 0);
    }
    file.close();
    // return true;
}



    // clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    // std::cout << "Client connected.\n";

    // char filePath[BUFFER_SIZE];
    // int bytesRead = recv(clientSocket, filePath, BUFFER_SIZE, 0);
    // if (bytesRead > 0) {
    //     filePath[bytesRead] = '\0';
    //     std::cout << "Client requested file: " << filePath << "\n";
    //     sendFile(clientSocket, filePath);
    // }
    