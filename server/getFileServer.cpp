#include "getFileServer.h"

void sendFile(const std::string& fileName, SOCKET clientSocket) {
    FrameMenu fram;
    double width, height;
    fram.getWidthAndHeight(width, height);
    std::ifstream inFile(fileName, std::ios::binary);
    if (!inFile.is_open()) {
        printCenteredInRectangle(width, height, "Cannot open file!", 4);
        // std::cerr << "Cannot open file!" << std::endl;
        return;
    }

    char buffer[CHUNK_SIZE];
    while (inFile.read(buffer, CHUNK_SIZE) || inFile.gcount() > 0) {
        int bytesSent = send(clientSocket, buffer, inFile.gcount(), 0);
        if (bytesSent == SOCKET_ERROR) {
            printCenteredInRectangle(width, height, "Error!", 5);
            std::cerr << "Error!" << std::endl;
            break;
        }
    }
    printCenteredInRectangle(width, height, "Send file successfully!", 6);
    std::cout << "Send file successfully!" << std::endl;
    inFile.close();
}