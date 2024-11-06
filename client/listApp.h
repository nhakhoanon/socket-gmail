#pragma once

#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <sstream>
#include <cstdio>

using namespace std;


std::string getImageNameFromPID(DWORD pid) {
    // Tạo lệnh CMD để lấy thông tin về tiến trình
    std::string command = "tasklist /FI \"PID eq " + std::to_string(pid) + "\"";

    // Khởi tạo cấu trúc PROCESS_INFORMATION và STARTUPINFO
    PROCESS_INFORMATION processInfo;
    STARTUPINFO startupInfo;
    ZeroMemory(&processInfo, sizeof(processInfo));
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Tạo pipe để đọc đầu ra
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE; // Cho phép truyền handle
    sa.lpSecurityDescriptor = NULL;

    // Tạo pipe cho đầu ra
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        std::cerr << "Failed to create pipe. Error: " << GetLastError() << std::endl;
        return "";
    }

    // Tạo tiến trình CMD
    startupInfo.hStdOutput = hWritePipe; // Ghi vào pipe
    startupInfo.hStdError = hWritePipe;  // Ghi lỗi vào pipe
    if (!CreateProcess(NULL, const_cast<LPSTR>(command.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) {
        std::cerr << "Failed to create process. Error: " << GetLastError() << std::endl;
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return "";
    }

    // Đóng handle ghi của pipe
    CloseHandle(hWritePipe);

    // Đọc đầu ra từ pipe
    std::string result;
    char buffer[128];
    DWORD bytesRead;

    // Đọc dữ liệu từ pipe
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Đảm bảo chuỗi kết thúc bằng NULL
        result += buffer; // Thêm vào kết quả
    }

    // Đóng handle đọc
    CloseHandle(hReadPipe);
    // Đợi tiến trình hoàn thành
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    
    // Đóng handle tiến trình
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    // Phân tích kết quả để lấy tên hình ảnh
    std::istringstream iss(result);
    std::string line;

    // Bỏ qua dòng đầu tiên (tiêu đề)
    if (std::getline(iss, line)) {
        std::getline(iss, line);
        std::getline(iss, line);
        // Đọc dòng tiếp theo, nơi chứa thông tin tiến trình
        if (std::getline(iss, line)) {
            std::istringstream lineStream(line);
            std::string imageName;
            lineStream >> imageName; // Lấy tên hình ảnh

            // Trả về tên hình ảnh
            return imageName;
        }
    }

    // Nếu không tìm thấy tiến trình, trả về chuỗi rỗng
    return "";
}