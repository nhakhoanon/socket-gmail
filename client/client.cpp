#include "client.h"

int main()
{
    hideCursor();
    FrameMenu frame;
    int widthConsole, heightConsole;
    int port = 55555;
    WSADATA wsaData;
    int wsaerr;
    getConsoleSize(widthConsole, heightConsole);
    printCentered("Press Enter to begin!", heightConsole / 2);
    cin.ignore();
    system("cls");
    printCentered("========== CLIENT ==========", heightConsole / 2 - 2);
    printCentered("=== Set up DLL ===", heightConsole / 2 + 2);
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    Sleep(2000);
    system("cls");
    if (wsaerr != 0)
    {
        cout << "The Winsock dll not found" << endl;
        frame.printRectangleInCenter();
        printCenteredInRectangle(68, 20, "The Winsock dll not found", 0);
        // return 0;
    }
    else
    {
        string s1 = "The Winsock dll found!";
        string s2 = "The status: ";
        frame.printRectangleInCenter();
        printCenteredInRectangle(68, 20, s1, -1);
        printCenteredInRectangle(68, 20, s2 + wsaData.szSystemStatus, 0);

    }
    CSMTPClient SMTPClient([](const std::string&){ return; });
    CIMAPClient IMAPClient([](const std::string& strLogMsg) { std::cout << strLogMsg << std::endl;  });
    SMTPClient.InitSession("smtp.gmail.com:465", EMAIL_ACCOUNT, EMAIL_PASSWORD,
                            CMailClient::SettingsFlag::ALL_FLAGS, CMailClient::SslTlsFlag::ENABLE_SSL);
    while (1)  
    {
        //Read from mail
        string IPOfServer = "127.0.0.1";

        char messageFromServer[bufferSize] = {};
        char messageFromClient[bufferSize] = {};

        IMAPClient.InitSession("imap.gmail.com:993", EMAIL_ACCOUNT, EMAIL_PASSWORD,
            CMailClient::SettingsFlag::ALL_FLAGS, CMailClient::SslTlsFlag::ENABLE_SSL);

        string strSearch;
        bool bResRcvStr = IMAPClient.Search(strSearch, CIMAPClient::SearchOption::CUSTOMIZED);
        if (!bResRcvStr) {
            cout << "Search failed\n";
            break;
        }
        // std::cout << strSearch << " " << strSearch.length() << "\n";
        string mailIndex = IMAPClient.GetMailIndex(strSearch);
        // cout << mailIndex  << " " << mailIndex.length() << " mailindex\n";

        if (mailIndex == "") {
            bool bRes = IMAPClient.CleanupSession();
            // cout << "No new mail\n";
            // vector<string> lines;
            // lines.push_back("WAITING FOR NEW MAILS!");
            frame.displayAnimation1(200, "WAITING FOR NEW MAILS!");
            // Sleep(1000);
            continue;
        }

        string strHeader, strBody;
        bResRcvStr = IMAPClient.GetHeaderString(mailIndex, strHeader);
        bResRcvStr = IMAPClient.GetBodyString(mailIndex, strBody);

        IPOfServer = stripString(IMAPClient.GetContent(strBody, "IP:"));
        if (IPOfServer == "") {
            cout << "No IP in mail\n";
            bool bRes = IMAPClient.CleanupSession();
            continue;
        }

        cout << bResRcvStr << "\n" << strHeader << "\n";
        string strSubject = stripAndLowerString(IMAPClient.GetSubject(strHeader));
        cout << "Subject get: " << strSubject << "\n";
        string strSender = stripString(IMAPClient.GetSender(strHeader));
        cout << "Sender get: " << strSender << "\n";
        
        // string tmp = cleanString(strSubject);
        //cout << "Please enter a message to send to the Server: ";
        //cin.getline(messageFromClient, 1024);

        //Create socket
        SOCKET clientSocket;
        clientSocket = INVALID_SOCKET;
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET)
        {
            cout << "Error at socket(): " << WSAGetLastError() << endl;
            bool bRes = IMAPClient.CleanupSession();
            continue;
            // WSACleanup();
            // return 0;
        }
        else
            cout << "socket() is OK!" << endl;
        
        //Connect to server
        cout << "\n=== Connect with Server IP: " << IPOfServer << endl;
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        InetPton(AF_INET, IPOfServer.c_str(), &clientService.sin_addr.s_addr);
        clientService.sin_port = htons(port);
        if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
        {
            cout << "Client: connect() - Failed to connect." << endl;
            bool bRes = IMAPClient.CleanupSession();
            continue;
            // WSACleanup();
            // return 0;
        }
        else
        {
            cout << "Client: connect() is OK" << endl;
            cout << "Client: Can start sending and receiving data..." << endl;
        }

        strcpy(messageFromClient, strSubject.c_str());
        messageFromClient[strSubject.length()] = '\0';
        int byteCount = send(clientSocket, messageFromClient, bufferSize, 0);
        if (byteCount > 0) {
            cout << "Message sent: " << messageFromClient << endl;
            cout << "Message length: " << strlen(messageFromClient) << endl;
            cout << "Subject length: " << strSubject.length() << endl;
        }
        else WSACleanup();
        if (string(messageFromClient) == "syntax")
        {
            string syntax = createSyntaxHtmlTable(ALL);
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Syntax", syntax, "");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "capturescreen")
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
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "listservices")
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

            vector<string> headers;
            headers.push_back("STT");
            headers.push_back("Service name");
            headers.push_back("Display name");
            vector<vector<string>> data;
            ifstream inputFile("./output/received_services.txt");
            if (!inputFile.is_open()) {
                cout << "Failed to open received_services.txt for reading" << endl;
                continue;
            }
            string line;
            int cnt = 1;
            while (getline(inputFile, line)) {
                vector<string> row;
                row.push_back(to_string(cnt));
                row.push_back(IMAPClient.GetContent(line, "SERVICE_NAME:"));
                getline(inputFile, line);
                row.push_back(IMAPClient.GetContent(line, "DISPLAY_NAME:"));
                data.push_back(row);
                cnt++;
                getline(inputFile, line);
            }
            inputFile.close();

            string body = "<p>Got services list successfully!</p>" + createHtmlTable(headers, data);

            // sendMail(strSender, "PROJECT_MMT List Service", "Day la danh sach cac dich vu!", "./output/received_services.txt");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT List Services", body, "");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
            closesocket(clientSocket);
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
            closesocket(clientSocket);
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
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "keylogger")
        {
            string time = stripString(IMAPClient.GetContent(strBody, "Time:"));
            if (time == "") {
                cout << "Keylogger time not found! Please try again!" << endl;
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "ERROR!", "Keylogger time not found! Please try again!", "");
                if (bResSendMail) cout << "Send mail successfully\n";
                else cout << "Send mail failed\n";
                closesocket(clientSocket);
            }
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
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "listapp")
        {
            vector<Application> gotApp;
            // map<DWORD, string> gotApp;
            // byteCount = receiveMap(clientSocket, gotApp);
            byteCount = receiveApplications(clientSocket, gotApp);
            if (byteCount > 0)
            {
                cout << "Message received: " << "Got apps list successfully!" << endl;
            }
            else 
                WSACleanup();
            sort(gotApp.begin(), gotApp.end(), comparePID);
            // vector<string> imageName;
            // receiveStringVector(clientSocket, imageName);
            // int cnt = 1;
            // auto iter = gotApp.begin();
            // string body = "";
            // for (auto x : imageName){
            //     cout << "App "  << cnt << ": " << x << ", PID: " << iter->first << endl;
            //     body += "App " + to_string(cnt) + ": " + x + ", PID: " + to_string(iter->first) + "\n";
            //     iter++; cnt++;
            // }
            vector<string> headers;
            headers.push_back("STT"); 
            headers.push_back("Name");
            headers.push_back("Exe file name");
            headers.push_back("PID");
            vector<vector<string>> data;
            int cnt = 1;
            // auto iter = gotApp.begin();
            for (auto x : gotApp){
                vector<string> row;
                row.push_back(to_string(cnt));
                row.push_back(x.title);
                row.push_back(x.fileName);
                row.push_back(to_string(x.pid));
                data.push_back(row);
                cnt++;
            }
            string body = "<p>Got apps list successfully!</p>" + createHtmlTable(headers, data);
            // sendMail(strSender, "PROJECT_MMT List App", body, "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT List App", body, "");
            if (bResSendMail) cout << "Send mail successfully\n";
            else cout << "Send mail failed\n";
            closesocket(clientSocket);
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
            closesocket(clientSocket);
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
            closesocket(clientSocket);
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
            closesocket(clientSocket);
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
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "startwebcam")
        {
            cout << "Recording...!" << endl;
            cout << "Sending 'stopWebcam' to this IP again to stop!" << endl;
            // sendMail(strSender, "PROJECT_MMT Webcam", "Mo webcam thanh cong!", "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Webcam", "Mo webcam thanh cong!", "");
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "stopwebcam")
        {
            cout << "Stop now!" << endl;
            receiveFile(clientSocket, "output.mp4");
            // sendMail(strSender, "PROJECT_MMT Webcam", "Day la video da duoc ghi!", "./output/output.mp4");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Webcam", "Day la video da duoc ghi!", "./output/output.mp4");
            closesocket(clientSocket);
        }
        bool bRes = IMAPClient.CleanupSession();
        Sleep(1000);
    }

    bool cleanup = SMTPClient.CleanupSession();

    system("pause");
    WSACleanup();
    return 0;
}