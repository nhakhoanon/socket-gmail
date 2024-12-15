#include "closeApp.h"

bool closeApplication(DWORD pid) {
    FrameMenu fram;
    double width, height;
    fram.getWidthAndHeight(width, height);
    // Mở tiến trình với quyền terminate
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == nullptr) {
        printCenteredInRectangle(width, height, "Cannot open process for PID: ", 4);
        // std::cerr << "Could not open process for PID: " << pid << std::endl;
        return false; // Không thể mở tiến trình
    }

    // Kết thúc tiến trình
    BOOL result = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess); // Đóng handle

    if (result) {
        printCenteredInRectangle(width, height, "Process terminated sucessfully", 5);
        // std::cout << "Process " << pid << " terminated successfully." << std::endl;
        return true; // Thành công
    } else {
        printCenteredInRectangle(width, height, "Failed to terminate process", 6);
        // std::cerr << "Failed to terminate process " << pid << "." << std::endl;
        return false; // Thất bại
    }
}

DWORD FindPIDByImageName(const std::string &imageName) {
    // Tạo snapshot của tất cả các tiến trình đang chạy trong hệ thống
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Unable to create process snapshot." << std::endl;
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    DWORD processID = 0;

    // Lặp qua các tiến trình trong snapshot
    if (Process32First(hSnapshot, &pe)) {
        do {
            // So sánh không phân biệt hoa thường
            if (_stricmp(imageName.c_str(), pe.szExeFile) == 0) {
                processID = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    } else {
        std::cerr << "Failed to retrieve process information." << std::endl;
    }

    CloseHandle(hSnapshot);
    return processID;
}