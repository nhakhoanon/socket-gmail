#include "listApp.h"

// Hàm để loại bỏ các ký tự không mong muốn
string SanitizeWindowTitle(const std::string& title) {
    string sanitized;
    copy_if(title.begin(), title.end(), std::back_inserter(sanitized), [](char c) {
        return c != '?'; // Loại bỏ ký tự ?
    });
    return sanitized;
}

// Hàm callback để lấy danh sách các cửa sổ
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (IsWindowVisible(hwnd)) {
        char windowTitle[256];
        GetWindowText(hwnd, windowTitle, sizeof(windowTitle));
        if (strlen(windowTitle) > 0) {
            std::vector<Application>* apps = reinterpret_cast<std::vector<Application>*>(lParam);
            Application app;
            app.title = SanitizeWindowTitle(windowTitle);
            GetWindowThreadProcessId(hwnd, &app.pid); // Lấy PID của ứng dụng
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