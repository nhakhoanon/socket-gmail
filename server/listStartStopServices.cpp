#include "listStartStopServices.h"


void listServices() {
    system("net start > services.txt");  // Redirect output to a file instead of the console
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
