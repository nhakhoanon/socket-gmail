#include "listApp.h"

// Hàm để loại bỏ các ký tự không mong muốn
string SanitizeWindowTitle(const std::string& title) {
    string sanitized;
    copy_if(title.begin(), title.end(), std::back_inserter(sanitized), [](char c) {
        return c != '?'; // Loại bỏ ký tự ?
    });
    return sanitized;
}

string wcharToUtf8(const wchar_t* wstr) {
    if (!wstr) {
        return "";
    }

    std::string utf8Str;
    while (*wstr) {
        wchar_t wc = *wstr++;
        if (wc < 0x80) {
            utf8Str.push_back(static_cast<char>(wc));
        } else if (wc < 0x800) {
            utf8Str.push_back(static_cast<char>((wc >> 6) | 0xC0));
            utf8Str.push_back(static_cast<char>((wc & 0x3F) | 0x80));
        } else if (wc < 0x10000) {
            utf8Str.push_back(static_cast<char>((wc >> 12) | 0xE0));
            utf8Str.push_back(static_cast<char>(((wc >> 6) & 0x3F) | 0x80));
            utf8Str.push_back(static_cast<char>((wc & 0x3F) | 0x80));
        } else if (wc < 0x110000) {
            utf8Str.push_back(static_cast<char>((wc >> 18) | 0xF0));
            utf8Str.push_back(static_cast<char>(((wc >> 12) & 0x3F) | 0x80));
            utf8Str.push_back(static_cast<char>(((wc >> 6) & 0x3F) | 0x80));
            utf8Str.push_back(static_cast<char>((wc & 0x3F) | 0x80));
        }
    }

    return utf8Str;
}

// Hàm callback để lấy danh sách các cửa sổ
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (IsWindowVisible(hwnd)) {
        wchar_t windowTitle[256];
        GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle));
        if (wcslen(windowTitle) > 0) {
            std::vector<Application>* apps = reinterpret_cast<std::vector<Application>*>(lParam);
            Application app;
            app.title = wcharToUtf8(windowTitle);
            GetWindowThreadProcessId(hwnd, &app.pid); // Lấy PID của ứng dụng
            app.fileName = getImageNameFromPID(app.pid);
            apps->push_back(app);
        }
    }
    return TRUE; // Tiếp tục enum
}

// Hàm lấy danh sách các ứng dụng đang mở
vector<Application> GetOpenApplications() {
    std::vector<Application> openApps;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&openApps));
    return openApps;
}

// Hàm phi tuần tự hóa
vector<Application> DeserializeApplications(const char* data) {
    const char* ptr = data;

    size_t count;
    memcpy((void*)&count, ptr, sizeof(size_t)); // Lấy số lượng ứng dụng
    ptr += sizeof(size_t);

    std::vector<Application> apps(count);

    for (size_t i = 0; i < count; ++i) {
        Application app;
        memcpy((void*)&app.pid, ptr, sizeof(DWORD)); // Lấy PID
        ptr += sizeof(DWORD);

        app.title = ptr; // Lấy tên ứng dụng
        ptr += app.title.size() + 1; // Di chuyển con trỏ tới vị trí tiếp theo

        apps[i] = app; // Lưu ứng dụng vào vector
    }

    return apps;
}

#include "listApp.h"

string getImageNameFromPID(DWORD pid) {
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