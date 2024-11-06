#include "closeApp.h"

bool closeApplication(DWORD pid) {
    // Mở tiến trình với quyền terminate
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == nullptr) {
        std::cerr << "Could not open process for PID: " << pid << std::endl;
        return false; // Không thể mở tiến trình
    }

    // Kết thúc tiến trình
    BOOL result = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess); // Đóng handle

    if (result) {
        std::cout << "Process " << pid << " terminated successfully." << std::endl;
        return true; // Thành công
    } else {
        std::cerr << "Failed to terminate process " << pid << "." << std::endl;
        return false; // Thất bại
    }
}