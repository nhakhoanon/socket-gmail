#include "client.h"
using namespace std;

int main()
{
    cout << "========== CLIENT ==========" << endl;
    cout << "=== Step 1 - Set up DLL" << endl;
    SOCKET clientSocket;
    int port = 55555;
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0)
    {
        cout << "The Winsock dll not found" << endl;
        // return 0;
    }
    else
    {
        cout << "The Winsock dll found!" << endl;
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }
    cout << "\n=== Step 2 - Set up Client Socket===" << endl;
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        // return 0;
    }
    else
        cout << "socket() is OK!" << endl;
    cout << "\n===Step 3 - Connect with Server" << endl;
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1"); //InetPton(AF_INET, L"127.0.0.1", &clientService.sin_addr.s_addr);
    clientService.sin_port = htons(port);
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
    {
        cout << "Client: connect() - Failed to connect." << endl;
        WSACleanup();
        // return 0;
    }
    else
    {
        cout << "Client: connect() is OK" << endl;
        cout << "Client: Can start sending and receiving data..." << endl;
    }
	cout << "\n=== Step 4 - Chat to the Server ===\n\n";

    while (1)  
    {
        char messageFromServer[bufferSize] = {};
        char messageFromClient[bufferSize] = {};

        cout << "Please enter a message to send to the Server: ";
        cin.getline(messageFromClient, 1024);
        int byteCount = send(clientSocket, messageFromClient, bufferSize, 0);
        if (byteCount > 0) {
            cout << "Message sent: " << messageFromClient << endl;
        }
        else WSACleanup();
        if (string(messageFromClient) == "capturescreen")
        {
            // Step 1: Receive the file size
            streamsize fileSize;
            recv(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
            // cout << "Expected file size: " << fileSize << " bytes" << endl;

            // Step 2: Prepare to receive the file in chunks
            ofstream outputFile("received_image.bmp", ios::binary);
            if (!outputFile.is_open()) {
                cout << "Failed to open file for writing" << endl;
                break;
            }

            char fileBuffer[bufferSize];
            streamsize totalReceived = 0;

            while (totalReceived < fileSize) {
                int bytesReceived = recv(clientSocket, fileBuffer, bufferSize, 0);
                if (bytesReceived > 0) {
                    outputFile.write(fileBuffer, bytesReceived);
                    totalReceived += bytesReceived;
                    cout << "Received " << bytesReceived << " bytes (" << totalReceived << "/" << fileSize << ")" << endl;
                }
                else if (bytesReceived == 0) {
                    cout << "Connection closed by server" << endl;
                    break;
                }
                else {
                    cout << "Error receiving file data" << endl;
                    break;
                }
            }

            outputFile.close();
            cout << "File received and saved as 'received_image.bmp'" << endl;
        }
        byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);

        if (byteCount > 0) {
            cout << "Message received: " << messageFromServer << endl;
        }
        else WSACleanup();
    }

    system("pause");
    WSACleanup();
    return 0;
}