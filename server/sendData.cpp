#include "sendData.h"

bool sendStringVector(SOCKET socket, const std::vector<std::string>& vec) {
    // Gửi kích thước của vector
    int size = vec.size();
    if (send(socket, reinterpret_cast<char*>(&size), sizeof(size), 0) == SOCKET_ERROR) {
        return false; // Nếu gửi thất bại, trả về false
    }

    // Gửi từng chuỗi trong vector
    for (const auto& str : vec) {
        int strLength = str.length();
        if (send(socket, reinterpret_cast<char*>(&strLength), sizeof(strLength), 0) == SOCKET_ERROR) {
            return false; // Nếu gửi độ dài thất bại, trả về false
        }
        if (send(socket, str.c_str(), strLength, 0) == SOCKET_ERROR) {
            return false; // Nếu gửi nội dung thất bại, trả về false
        }
    }
    return true; // Nếu gửi thành công, trả về true
}

bool sendMap(SOCKET sock, const std::map<DWORD, std::string>& data) {
    // Gửi kích thước của map
    size_t mapSize = data.size();
    if (send(sock, reinterpret_cast<const char*>(&mapSize), sizeof(mapSize), 0) == SOCKET_ERROR) {
        return false; // Gửi kích thước thất bại
    }

    // Gửi từng phần tử trong map
    for (const auto& pair : data) {
        DWORD key = pair.first;
        const std::string& value = pair.second;

        // Gửi key
        if (send(sock, reinterpret_cast<const char*>(&key), sizeof(key), 0) == SOCKET_ERROR) {
            return false; // Gửi key thất bại
        }

        // Gửi độ dài của value
        size_t valueLength = value.size();
        if (send(sock, reinterpret_cast<const char*>(&valueLength), sizeof(valueLength), 0) == SOCKET_ERROR) {
            return false; // Gửi độ dài value thất bại
        }

        // Gửi value
        if (send(sock, value.c_str(), valueLength, 0) == SOCKET_ERROR) {
            return false; // Gửi value thất bại
        }
    }

    return true; // Gửi thành công
}

vector<char> SerializeApplications(const std::vector<Application>& apps) {
    size_t totalSize = sizeof(size_t); // Kích thước cho số lượng ứng dụng
    for (const auto& app : apps) {
        totalSize += sizeof(DWORD) + app.fileName.size() + app.title.size() + 2;
    }

    std::vector<char> buffer(totalSize);
    char* ptr = buffer.data();

    // Lưu số lượng ứng dụng
    size_t count = apps.size();
    memcpy(ptr, &count, sizeof(size_t));
    ptr += sizeof(size_t);

    // Lưu từng ứng dụng
    for (const auto& app : apps) {
        memcpy(ptr, &app.pid, sizeof(DWORD));
        ptr += sizeof(DWORD);
        strcpy(ptr, app.fileName.c_str()); // Sao chép tên ứng dụng vào buffer
        ptr += app.fileName.size() + 1;
        strcpy(ptr, app.title.c_str()); // Sao chép tên ứng dụng vào buffer
        ptr += app.title.size() + 1; // Tiến đến vị trí tiếp theo (bao gồm null terminator)
    }

    return buffer;
}

bool sendApplications(SOCKET socket, const std::vector<Application>& apps) {
    auto buffer = SerializeApplications(apps);
    int bytesSent = send(socket, buffer.data(), buffer.size(), 0);
    return bytesSent != SOCKET_ERROR; // Trả về true nếu gửi thành công
}