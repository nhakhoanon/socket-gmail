#include "openApp.h"

// bool isProcessRunning(const std::string& processName) {
//     PROCESSENTRY32 entry;
//     entry.dwSize = sizeof(PROCESSENTRY32);
    
//     HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//     if (hProcessSnap == INVALID_HANDLE_VALUE) return false;

//     if (Process32First(hProcessSnap, &entry)) {
//         do {
//             if (processName == entry.szExeFile) {
//                 CloseHandle(hProcessSnap);
//                 return true;
//             }
//         } while (Process32Next(hProcessSnap, &entry));
//     }

//     CloseHandle(hProcessSnap);
//     return false;
// }

// int openApp(const std::string& imageName) {
//     // Kiểm tra xem app đã mở chưa
//     if (isProcessRunning(imageName)) {
//         return -1; // Ứng dụng đã mở
//     }

//     // Cấu hình thông tin tiến trình
//     STARTUPINFO si = { sizeof(si) };
//     PROCESS_INFORMATION pi;

//     // Mở ứng dụng
//     if (CreateProcess(imageName.c_str(),   // Image Name
//                       NULL,                // Command line arguments (NULL nếu không có)
//                       NULL,                // Process security attributes
//                       NULL,                // Thread security attributes
//                       FALSE,               // Inherit handles?
//                       0,                   // Creation flags
//                       NULL,                // Environment variables
//                       NULL,                // Current directory
//                       &si,                 // Startup info
//                       &pi)) {              // Process info
//         // Đợi tiến trình kết thúc (không bắt buộc, có thể bỏ qua)
//         WaitForSingleObject(pi.hProcess, INFINITE);
//         CloseHandle(pi.hProcess);
//         CloseHandle(pi.hThread);
//         return 1; // Mở thành công
//     }

//     return 0; // Không mở được ứng dụng
// }

int openApplicationByName(const std::string& fileName) {
    // Hàm ShellExecute mở ứng dụng dựa trên tên file.
    HINSTANCE result = ShellExecute(NULL, "open", fileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
    
    // Kiểm tra kết quả trả về của ShellExecute
    if ((uintptr_t)result > 32) {
        return 1; // Mở thành công
    } else {
        return 0; // Không mở được ứng dụng
    }
}