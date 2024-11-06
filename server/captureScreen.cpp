#include "captureScreen.h"

bool captureScreen(const char* filename) {
    // Lấy kích thước màn hình
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    // Tạo hDC cho màn hình và hDC cho ảnh lưu tạm
    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    // Chụp ảnh màn hình vào hMemoryDC
    BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);

    // Lưu ảnh thành file BMP
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    BITMAP bitmap;

    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    DWORD bmpSize = bitmap.bmWidth * bitmap.bmHeight * 4;

    fileHeader.bfType = 0x4D42;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = bitmap.bmWidth;
    infoHeader.biHeight = bitmap.bmHeight;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Không thể tạo file.\n";
        return false;
    }

    DWORD written = 0;
    WriteFile(hFile, &fileHeader, sizeof(fileHeader), &written, nullptr);
    WriteFile(hFile, &infoHeader, sizeof(infoHeader), &written, nullptr);

    BYTE* bmpData = new BYTE[bmpSize];
    GetDIBits(hMemoryDC, hBitmap, 0, (UINT)bitmap.bmHeight, bmpData, (BITMAPINFO*)&infoHeader, DIB_RGB_COLORS);

    WriteFile(hFile, bmpData, bmpSize, &written, nullptr);
    CloseHandle(hFile);

    // Giải phóng bộ nhớ
    delete[] bmpData;
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);

    return true;
}

// int main() {
//     if (CaptureScreen("screenshot.bmp")) {
//         std::cout << "Đã chụp màn hình và lưu thành công.\n";
//     } else {
//         std::cout << "Chụp màn hình thất bại.\n";
//     }
//     return 0;
// }
