#include "getFileClient.h"

void receiveFile(SOCKET serverSocket) {
    char buffer[BUFFER_SIZE];
    int bytesRead = recv(serverSocket, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string response(buffer);

        if (response == "NOT_FOUND") {
            std::cout << "File not found on server.\n";
            return;
        }

        // Receive file name
        bytesRead = recv(serverSocket, buffer, sizeof(buffer), 0);
        buffer[bytesRead] = '\0';
        std::string fileName(buffer);

        std::ofstream file(fileName, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to create file : " << fileName << "\n";
            return;
        }

        // Receive file content
        while ((bytesRead = recv(serverSocket, buffer, sizeof(buffer), 0)) > 0) {
            file.write(buffer, bytesRead);
        }
        file.close();
        std::cout << "File received and saved as " << fileName << "\n";
    }
}

std::string escapeBackslashes(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c == '\\') { 
            result += "\\\\";  // Thêm \\ nếu gặp "\"
        } else {
            result += c;
        }
    }
    return result;
}


// std::string filePath;
// std::cout << "Enter file path to request: ";
// std::getline(std::cin, filePath);
// std::string _filePath = escapeBackslashes(filePath);
// send(clientSocket, _filePath.c_str(), _filePath.size(), 0);
// receiveFile(clientSocket);

    
