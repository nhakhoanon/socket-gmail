#include "closeFile.h"

bool deleteFileByPath(const std::string& filePath) {
    // Gọi hàm DeleteFile để xóa file
    return DeleteFileA(filePath.c_str()) != 0; // Trả về true nếu xóa thành công, false nếu thất bại
}