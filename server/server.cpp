#include "server.h"
using namespace std;

bool CaptureScreenshot(const char* filename) {
    // Lấy kích thước màn hình
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    // Tạo hDC cho màn hình và hDC cho ảnh lưu tạm
    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    // Chụp ảnh màn hình vào hMemoryDC
    BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);

    // Lưu ảnh thành file BMP
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    BITMAP bitmap;

    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    DWORD bmpSize = bitmap.bmWidth * bitmap.bmHeight * 4;

    fileHeader.bfType = 0x4D42;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = bitmap.bmWidth;
    infoHeader.biHeight = bitmap.bmHeight;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Không thể tạo file.\n";
        return false;
    }

    DWORD written = 0;
    WriteFile(hFile, &fileHeader, sizeof(fileHeader), &written, nullptr);
    WriteFile(hFile, &infoHeader, sizeof(infoHeader), &written, nullptr);

    BYTE* bmpData = new BYTE[bmpSize];
    GetDIBits(hMemoryDC, hBitmap, 0, (UINT)bitmap.bmHeight, bmpData, (BITMAPINFO*)&infoHeader, DIB_RGB_COLORS);

    WriteFile(hFile, bmpData, bmpSize, &written, nullptr);
    CloseHandle(hFile);

    // Giải phóng bộ nhớ
    delete[] bmpData;
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);

    return true;
}

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
    service.sin_addr.s_addr = INADDR_ANY;  //InetPton(AF_INET,"127.0.0.1", &service.sin_addr.s_addr);
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
        else if (string(messageFromClient) == "capturescreen") // Send the bitmap file
        {
            CaptureScreenshot("screenshot.bmp");
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
        cout << "Please enter a message to send to the Client: ";
        cin.getline(messageFromServer, 1024);
        cout << messageFromServer << endl;
        byteCount = send(acceptSocket, messageFromServer, 1024, 0);
        if (byteCount > 0) 
            cout << "Message sent: " << messageFromServer << endl;
        else
            cout << "Error sending message" << endl;
    }
    system("pause");
    WSACleanup();
    return 0;
}