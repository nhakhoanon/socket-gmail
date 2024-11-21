#include "recordWebcam.h"


// bool isRecording = false;
// IMFSinkWriter* pSinkWriter = nullptr;
// DWORD streamIndex = 0;
// string videoFileName = "recorded_video.mp4";
std::atomic<bool> stopFlag(false);

// void sendVideoFile(const char* filename, int Socket) {
//     // Mở file video để đọc
//     std::ifstream file(filename, std::ios::binary);
//     if (!file) {
//         std::cerr << "Không thể mở file!" << std::endl;
//         return;
//     }

//     // Đọc nội dung file video và gửi qua socket
//     char buffer[1024];
//     while (file.read(buffer, sizeof(buffer))) {
//         int bytesRead = file.gcount();
//         if (send(Socket, buffer, bytesRead, 0) == -1) {
//             std::cerr << "Lỗi khi gửi dữ liệu!" << std::endl;
//             break;
//         }
//     }

//     // Gửi phần dữ liệu còn lại nếu có
//     if (file.gcount() > 0) {
//         send(Socket, buffer, file.gcount(), 0);
//     }

//     std::cout << "File video đã được gửi thành công!" << std::endl;
//     file.close();
// }

void sendFile(const std::string& videoFilename, SOCKET clientSocket) {
    std::ifstream inFile(videoFilename, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Không thể mở file video!" << std::endl;
        return;
    }

    char buffer[CHUNK_SIZE];
    while (inFile.read(buffer, CHUNK_SIZE) || inFile.gcount() > 0) {
        int bytesSent = send(clientSocket, buffer, inFile.gcount(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Error!" << std::endl;
            break;
        }
    }

    std::cout << "Send file successfully!" << std::endl;
    inFile.close();
}


void recordVideo(const std::string& outputFilename, int width = 640, int height = 480) {
    cv::VideoCapture cap(0);  // Mở camera
    if (!cap.isOpened()) {
        std::cerr << "Không thể mở camera" << std::endl;
        return;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);  // Thiết lập chiều rộng
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);  // Thiết lập chiều cao

    int codec = cv::VideoWriter::fourcc('M', 'P', '4', 'V');  // Codec cho .mp4
    cv::VideoWriter out(outputFilename, codec, 15, cv::Size(width, height));  // Mở VideoWriter

    if (!out.isOpened()) {
        // std::cerr << "Không thể mở file video để ghi." << std::endl;
        return;
    }

    cv::Mat frame;
    while (true) {
        cap >> frame;  // Lấy khung hình từ camera
        if (frame.empty()) {
            // std::cerr << "Không thể lấy khung hình từ camera." << std::endl;
            break;
        }

        // Lật khung hình để giống như app camera mặc định
        cv::flip(frame, frame, 1);

        out.write(frame);  // Ghi khung hình vào file video

        cv::imshow("Recording", frame);  // Hiển thị khung hình trên cửa sổ

        // Kiểm tra điều kiện dừng bằng cờ stopFlag
        if (stopFlag.load()) {  // Nếu cờ dừng là true, thoát khỏi vòng lặp
            std::cout << "Stop recoring!" << std::endl;
            break;
        }

        // Tăng thời gian chờ để giảm tốc độ
        if (cv::waitKey(1) == 'q') {  // Đặt waitKey lớn hơn 1 để giảm tốc độ video
            break;
        }
    }

    cap.release();  // Giải phóng camera
    out.release();  // Giải phóng file video
    cv::destroyAllWindows();  // Đóng tất cả cửa sổ OpenCV
}

void stopRecord() {
    stopFlag = true;  // Đặt cờ dừng thành true
}

void resetFlag(){
    stopFlag = false;
}