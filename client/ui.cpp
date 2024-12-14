#include "ui.h"

void gotoxy(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
    COORD cursorPosition; 
    cursorPosition.X = x; 
    cursorPosition.Y = y; 
    SetConsoleCursorPosition(hConsole, cursorPosition); 
}

void hideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo); 
}

void getConsoleSize(int &width, int &height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;  // Chiều rộng
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1; // Chiều cao
    } else {
        width = 80;  // Mặc định
        height = 25; // Mặc định
    }
}

void FrameMenu::printRectangleInCenter() {
    istringstream stream(this->rectangle);
    vector<std::string> lines;
    string line;
    int maxWidth = 0;

    while (getline(stream, line)) {
        lines.push_back(line);
        if (line.length() > maxWidth) {
            maxWidth = line.length();
        }
    }

    int rectHeight = lines.size();

    int consoleWidth, consoleHeight;
    getConsoleSize(consoleWidth, consoleHeight);

    int startX = (consoleWidth - maxWidth) / 2; 
    int startY = (consoleHeight - rectHeight) / 2;

    for (int i = 0; i < rectHeight; ++i) {
        gotoxy(startX, startY + i);
        cout << lines[i];
    }
}

void setTextColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void resetTextColor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7); 
}

FrameMenu::FrameMenu() {
        rectangle = R"(
        ---------------------------------------------------------------------
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        |                                                                   |
        ---------------------------------------------------------------------)"; 
        width = 69;
        height = 20;
}

void FrameMenu::displayAnimation1(int delay, string s) {
    hideCursor();
    system("cls");
    int cnt = 0;
    int color = 1;
    int rot = 0;
    bool decrease = 0;
    int x, y;
    getConsoleSize(x, y);
    while (cnt < 5) {
        setTextColor(color);
        this->printRectangleInCenter();
        printCenteredInRectangle(68, 20, s, 0);
        if (rot == 5) {
            decrease = 1;
            rot = 0;
        }
        else 
            rot++;

        if (decrease == 1) {
            color--;
            if (color == 1)
                decrease = 0;
        } 
        else 
            color++;
        cnt++;
        Sleep(delay);
        system("cls");
    }
    resetTextColor();
    this->printRectangleInCenter();
    printCenteredInRectangle(68, 20, s, 0);
}

void FrameMenu::printContentInRectangle(const vector<string> listOfSentences, int x, int y) {
    for (const auto& line : listOfSentences) {
        printCentered(line, y++); 
    }
}

void printCentered(const std::string& str, int y) {
    int consoleWidth, consoleHeight;
    getConsoleSize(consoleWidth, consoleHeight); // Lấy kích thước console

    int x = (consoleWidth - str.length()) / 2;   // Tính tọa độ x (chính giữa theo chiều ngang)

    gotoxy(x, y);                               // Di chuyển con trỏ đến vị trí chính giữa
    std::cout << str;                           // In chuỗi
}

void printCenteredInRectangle(int width, int height, const std::string& str, int delta) {

    int widthConsole, heightConsole;
    getConsoleSize(widthConsole, heightConsole);
    int startX = (widthConsole - width) / 2;
    int startY = (heightConsole - height) / 2;
    double centerX = startX + (width - str.length()) / 2.0;
    double centerY = startY + height / 2.0 + delta;

    // In chuỗi ở giữa hình chữ nhật
    gotoxy(centerX, centerY);
    std::cout << str;
}