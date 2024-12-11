#include "receiveData.h"

bool receiveStringVector(SOCKET socket, std::vector<std::string>& vec) {
    // Nhận kích thước của vector
    int size = 0;
    if (recv(socket, reinterpret_cast<char*>(&size), sizeof(size), 0) == SOCKET_ERROR) {
        return false; // Nếu nhận kích thước thất bại, trả về false
    }

    vec.resize(size); // Resize vector để chứa dữ liệu

    // Nhận từng chuỗi trong vector
    for (int i = 0; i < size; ++i) {
        int strLength = 0;
        if (recv(socket, reinterpret_cast<char*>(&strLength), sizeof(strLength), 0) == SOCKET_ERROR) {
            return false; // Nếu nhận độ dài chuỗi thất bại, trả về false
        }
        std::vector<char> buffer(strLength + 1); // Tạo buffer cho chuỗi
        if (recv(socket, buffer.data(), strLength, 0) == SOCKET_ERROR) {
            return false; // Nếu nhận nội dung thất bại, trả về false
        }
        buffer[strLength] = '\0'; // Đảm bảo chuỗi kết thúc đúng cách
        vec[i] = std::string(buffer.data()); // Chuyển đổi thành std::string
    }
    return true; // Nếu nhận thành công, trả về true
}

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
        app.fileName = ptr;
        ptr += app.fileName.size() + 1;
        app.title = ptr; // Lấy tên ứng dụng
        ptr += app.title.size() + 1; // Di chuyển con trỏ tới vị trí tiếp theo

        apps[i] = app; // Lưu ứng dụng vào vector
    }

    return apps;
}

bool receiveApplications(SOCKET socket, std::vector<Application>& apps) {
    char buffer[40960]; // Kích thước buffer để nhận dữ liệu
    int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
    
    if (bytesReceived > 0) {                                                               
        apps = DeserializeApplications(buffer); // Gán giá trị cho vector apps
        return true; // Trả về true nếu nhận thành công
    }
    return false; // Trả về false nếu không nhận được dữ liệu
}

bool receiveMap(SOCKET sock, std::map<DWORD, std::string>& data) {
    // Nhận kích thước của map
    size_t mapSize;
    if (recv(sock, reinterpret_cast<char*>(&mapSize), sizeof(mapSize), 0) == SOCKET_ERROR) {
        return false; // Nhận kích thước thất bại
    }

    // Nhận từng phần tử trong map
    for (size_t i = 0; i < mapSize; ++i) {
        DWORD key;
        if (recv(sock, reinterpret_cast<char*>(&key), sizeof(key), 0) == SOCKET_ERROR) {
            return false; // Nhận key thất bại
        }

        // Nhận độ dài của value
        size_t valueLength;
        if (recv(sock, reinterpret_cast<char*>(&valueLength), sizeof(valueLength), 0) == SOCKET_ERROR) {
            return false; // Nhận độ dài value thất bại
        }

        // Nhận value
        std::vector<char> buffer(valueLength + 1); // Thêm 1 cho ký tự NULL
        if (recv(sock, buffer.data(), valueLength, 0) == SOCKET_ERROR) {
            return false; // Nhận value thất bại
        }
        buffer[valueLength] = '\0'; // Đảm bảo chuỗi kết thúc bằng NULL

        data[key] = std::string(buffer.data()); // Thêm vào map
    }

    return true; // Nhận thành công
}

// void receiveVideoFile(const char* filename, int clientSocket) {
//     // Mở file để ghi
//     std::ofstream file(filename, std::ios::binary);
//     if (!file) {
//         std::cerr << "Không thể mở file để ghi!" << std::endl;
//         return;
//     }

//     char buffer[1024];
//     int bytesReceived;

//     // Nhận dữ liệu từ server và ghi vào file
//     while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
//         file.write(buffer, bytesReceived);
//     }

//     std::cout << "Successfully!" << std::endl;
//     file.close();
// }

void receiveFile(SOCKET serverSocket, const std::string& outputFilename) {
    char buffer[CHUNK_SIZE];
    std::ofstream outFile("./output/" + outputFilename, std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Cannot open file!" << std::endl;
        return;
    }

    int bytesReceived;
    char command[CHUNK_SIZE] {'\0'};
    strcpy(command, "close");
    while ((bytesReceived = recv(serverSocket, buffer, CHUNK_SIZE, 0)) > 0 && strcmp(buffer, command) != 0) {
        outFile.write(buffer, bytesReceived);
        // cout << bytesReceived << endl;
    }

    cout << "Get file successfully!" << endl;
    // if (bytesReceived == 0) {
    //     std::cout << "Get file successfully" << std::endl;
    // } else if (bytesReceived == SOCKET_ERROR) {
    //     std::cerr << "Error!" << std::endl;
    // }
    outFile.close();
}

bool comparePID(Application a, Application b) {
    return a.pid < b.pid;
}