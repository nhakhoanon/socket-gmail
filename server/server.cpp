#include "server.h"


int main(int argc, char *argv[])
{
    cout << "==========SERVER==========" << endl;
    cout << "===Step 1 - Set up DLL===" << endl;
    SOCKET serverSocket, acceptSocket;
    int port = 55555;
    WSADATA wsaData;
    int wsaerr;
    WORD wVErsionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVErsionRequested, &wsaData);
    if (wsaerr != 0)
    {
        cout <<"The Winsock dll not found!" << endl;
        return 0;
    }
    else
    {
        cout << "The Winsock dll found!" << endl;
        cout << "THe status: " << wsaData.szSystemStatus << endl;
    }
    cout << "\n=== Step 2 - Set up the Server Socket===" << endl;
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket  == INVALID_SOCKET)
    {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else
    {
        cout << "socket() is OK!" << endl;
    }
    cout << "\n===Step 3 - Bind Socket ===" << endl;
    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET,"0.0.0.0", &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        cout << "bind() failer: " << WSAGetLastError << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else
    {
        cout << "bind() is OK!" << endl;
    }
    cout << "\n=== Step 4===" << endl;
    if (listen(serverSocket, 1) == SOCKET_ERROR)
        cout << "listen(): Error listening on socket" << WSAGetLastError() << endl;
    else
        cout << "listen() is OK. I waiting for connections..." << endl;
    cout << "===Step 5: Accept connection===" << endl;
    acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET)
    {
        cout << "accept failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }
    cout << "Accept connection" << endl;
	cout << "\n=== Step 6 - Chat to the Client ===\n\n";
    while (true) // server.cpp
    {
        char messageFromServer[bufferSize] = {};
        char messageFromClient[bufferSize] = {};
        int byteCount = recv(acceptSocket, messageFromClient, bufferSize, 0);
        if (byteCount > 0) {
            cout << "Message received: " << messageFromClient << endl;
        }
        else WSACleanup();
        if (string(messageFromClient) == "shutdown") 
                system("shutdown /s /f /t 0");
        else if (string(messageFromClient) == "restart")
        {
            #ifdef _WIN32
                system("shutdown /r /t 0");  // Lệnh restart cho Windows
            #endif
        }
        else if (string(messageFromClient) == "captureScreen") // Send the bitmap file
        {
            captureScreen("screenshot.bmp");
            // Open the bitmap file
            ifstream file("screenshot.bmp", ios::binary | ios::ate);
            if (!file.is_open()) {
                cout << "Failed to open screenshot.bmp" << endl;
                continue;
            }

            // Get file size and allocate buffer
            streamsize fileSize = file.tellg();
            file.seekg(0, ios::beg);
            char *fileBuffer = new char[fileSize];
            if (!file.read(fileBuffer, fileSize)) {
                cout << "Failed to read screenshot.bmp" << endl;
                delete[] fileBuffer;
                file.close();
                continue;
            }
            file.close();

            // Send file size first
            send(acceptSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

            // Send file content
            int sentBytes = send(acceptSocket, fileBuffer, fileSize, 0);
            if (sentBytes == SOCKET_ERROR) 
                cout << "Error sending file data: " << WSAGetLastError() << endl;
            delete[] fileBuffer;
        }
        else if (string(messageFromClient) == "listService")
        {
            listServices();
            ifstream file("services.txt", ios::binary | ios::ate);
            if (!file.is_open()) {
                cout << "Failed to open service.txt" << endl;
                continue;
            }

            // Get file size and allocate buffer
            streamsize fileSize = file.tellg();
            file.seekg(0, ios::beg);
            char *fileBuffer = new char[fileSize];
            if (!file.read(fileBuffer, fileSize)) {
                cout << "Failed to read service.txt" << endl;
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
                    cout << "Error sending file data: " << WSAGetLastError() << endl;
                    break;
                }
                sentBytes += result;
            }
            delete[] fileBuffer;
            // cout << "File example.txt sent successfully." << endl;
        }
        else if (string(messageFromClient) == "keylogger")
        {
            char time[bufferSize] = {};
            recv(acceptSocket, time, bufferSize, 0);       
            keylogger(stoi(time));
            ifstream file("keylogger.txt", ios::binary | ios::ate);
            if (!file.is_open()) {
                cout << "Failed to open keylogger.txt" << endl;
                continue;
            }

            // Get file size and allocate buffer
            streamsize fileSize = file.tellg();
            file.seekg(0, ios::beg);
            char *fileBuffer = new char[fileSize];
            if (!file.read(fileBuffer, fileSize)) {
                cout << "Failed to read keylogger.txt" << endl;
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
                    cout << "Error sending file data: " << WSAGetLastError() << endl;
                    break;
                }
                sentBytes += result;
            }
            delete[] fileBuffer;         
        }
        else if (string(messageFromClient) == "startService")
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
        }
        else if (string(messageFromClient) == "stopService")
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
        }
        else if (string(messageFromClient) == "listApp")
        {   
            vector<Application> gotApp = GetOpenApplications();
            map<DWORD, string> Apps;
            for (int i = 0; i < gotApp.size(); i++)
            {
                Apps.insert({gotApp[i].pid, gotApp[i].title});
            }
            byteCount = sendMap(acceptSocket, Apps);
            // byteCount = send(acceptSocket, messageFromServer, bufferSize, 0);
            if (byteCount > 0)
                cout << "Message sent: " << "Sent apps list successfully!" << endl;
            else
                WSACleanup();
        }
        else if (string(messageFromClient).substr(0, 7) == "openApp")
        {
            string nameApp = string(messageFromClient).substr(8);
            int check = openApplicationByName(nameApp);
            string announcement = "";
            if (check == 0)
                announcement = "Open required app unsuccessfully!";
            else if (check == 1)
                announcement = "Open required app successfully!";
            strcpy(messageFromServer, announcement.c_str());
            byteCount = send(acceptSocket, messageFromServer, 1024, 0);
            if (byteCount > 0)
                cout << "Message sent: " << messageFromServer << endl;
            else 
                WSACleanup();
        }
        else if (string(messageFromClient).substr(0, 8) == "closeApp")
        {
            vector<Application> gotApp = GetOpenApplications();
            map<DWORD, string> Apps;
            for (int i = 0; i < gotApp.size(); i++)
            {
                Apps.insert({gotApp[i].pid, gotApp[i].title});
            }
            vector<pair<DWORD, string>> remove;
            int cnt = 0;
            for (auto x : Apps)
            {
                remove.push_back({x.first, x.second});
            }
            string appName = string(messageFromClient).substr(9);
            int index = stoi(appName);
            string announcement = "";
            if (closeApplication(remove[index - 1].first))
            {
                announcement = "Terminate required application successfully!";
            }
            else
                announcement = "Terminate unsuccessfully!";
            strcpy(messageFromServer, announcement.c_str());
            byteCount = send(acceptSocket, messageFromServer, 1024, 0);
            if (byteCount > 0)
                cout << "Message sent: " << messageFromServer << endl;
            else 
                WSACleanup();
        }
        else if (string(messageFromClient).substr(0, 10) == "deleteFile")
        {
            string announcement = "";
            if (deleteFileByPath(string(messageFromClient).substr(11)))
                announcement = "Delete required file successfully!";
            else 
                announcement = "Delete required file unsuccessfully!";
            strcpy(messageFromServer, announcement.c_str());
            byteCount = send(acceptSocket, messageFromServer, 1024, 0);
            if (byteCount > 0)
                cout << "Message sent: " << messageFromServer << endl;
            else 
                WSACleanup();
        }
        else if (string(messageFromClient).substr(0, 7) == "getFile")
        {
            char filePath[BUFFER_SIZE];
            int bytesRead = recv(acceptSocket, filePath, BUFFER_SIZE, 0);
            if (bytesRead > 0) {
                filePath[bytesRead] = '\0';
                std::cout << "Client requested file: " << filePath << "\n";
                sendFile2(acceptSocket, filePath);
                // string announcement = "";
                // if (sendFile(acceptSocket, filePath))
                //     announcement = "Get required file successfully!";
                // else 
                //     announcement = "Get required file unsuccessfully!";
                // strcpy(messageFromServer, announcement.c_str());
                // byteCount = send(acceptSocket, messageFromServer, 1024, 0);
            }
        }
        else if (string(messageFromClient) == "startWebcam")
        {
            if (startRecording()) {
                cout << "Recording....!" << endl;
            }
        }
        else if (string(messageFromClient) == "stopWebcam")
        {
            stopRecording();
            sendVideoFile(videoFileName, acceptSocket);
        }
        // cout << "Please enter a message to send to the Client: ";
        // cin.getline(messageFromServer, 1024);
        // cout << messageFromServer << endl;
        // byteCount = send(acceptSocket, messageFromServer, 1024, 0);
    }
    system("pause");
    WSACleanup();
    return 0;
}