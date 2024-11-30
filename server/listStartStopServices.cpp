#include "listStartStopServices.h"


// void listServices() {
//     system("net start > services.txt");  // Redirect output to a file instead of the console
// }

void listServices() {
    // Chạy lệnh sc query để lấy danh sách các dịch vụ đang chạy và lưu vào file services.txt
    system("sc query type= service > services.txt");
    // Chạy lệnh sc query để lấy ra danh sách tất cả các dịch vụ và lưu vào file services.txt
    // system("sc query type= service state= all> services.txt");
    // Mở file để ghi kết quả
    ofstream outFile("services2.txt");
    if (!outFile) {
        cerr << "Không thể mở file services2.txt để ghi.\n";
        return;
    }

    ifstream inputFile("services.txt");
    if (!inputFile) {
        cerr << "Không thể mở file services.txt.\n";
        return;
    }

    string line;
    string serviceName, displayName;

    while (getline(inputFile, line)) {
        // Kiểm tra và lấy SERVICE_NAME
        if (line.find("SERVICE_NAME:") != string::npos) {
            serviceName = line.substr(line.find(":") + 1); // Lấy phần sau dấu :
            serviceName.erase(0, serviceName.find_first_not_of(" \t")); // Xóa khoảng trắng thừa
        }

        // Kiểm tra và lấy DISPLAY_NAME
        if (line.find("DISPLAY_NAME:") != string::npos) {
            displayName = line.substr(line.find(":") + 1); // Lấy phần sau dấu :
            displayName.erase(0, displayName.find_first_not_of(" \t")); // Xóa khoảng trắng thừa

            // Ghi vào file SERVICE_NAME và DISPLAY_NAME
            outFile << "SERVICE_NAME: " << serviceName << "\n";
            outFile << "DISPLAY_NAME: " << displayName << "\n\n";

            // Reset giá trị để chuẩn bị cho dịch vụ tiếp theo
            serviceName.clear();
            displayName.clear();
        }
    }

    inputFile.close();
    outFile.close();
}

// void listServices() {
    
//     // Open the file for writing
//     std::ofstream outFile("services.txt");
//     if (!outFile) {
//         std::cerr << "Failed to open services.txt for writing.\n";
//         return;
//     }

//     // Run the "net start" command and capture the output
//     FILE* pipe = _popen("net start", "r");
//     if (!pipe) {
//         std::cerr << "Failed to run net start command.\n";
//         outFile.close();
//         return;
//     }

//     // Read the output from the command and write it to the file
//     char buffer[256];
//     while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
//         outFile << buffer;
//     }

//     // Close the pipe and the file
//     _pclose(pipe);
//     outFile.close();

//     // std::cout << "Services list saved to services.txt.\n";
// }

bool startService(const std::string& serviceName) {
    std::string command = "net start " + serviceName;
    return !system(command.c_str());
}

bool stopService(const std::string& serviceName) {
    std::string command = "net stop " + serviceName;
    return !system(command.c_str());
}

//wisvc
// int main() {
//     std::string serviceName;
//     int choice;

//     while (true) {
//         std::cout << "\n1. List Services\n2. Start Service\n3. Stop Service\n4. Exit\n";
//         std::cout << "Enter your choice: ";
//         std::cin >> choice;
        
//         switch (choice) {
//             case 1:
//                 listServices();
//                 break;
//             case 2:
//                 std::cout << "Enter the service name to start: ";
//                 std::cin >> serviceName;
//                 startService(serviceName);
//                 break;
//             case 3:
//                 std::cout << "Enter the service name to stop: ";
//                 std::cin >> serviceName;
//                 stopService(serviceName);
//                 break;
//             case 4:
//                 std::cout << "Exiting...\n";
//                 return 0;
//             default:
//                 std::cout << "Invalid choice. Please try again.\n";
//                 break;
//         }
//     }
// }
