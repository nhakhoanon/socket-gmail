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
