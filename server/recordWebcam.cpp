#include "recordWebcam.h"

std::atomic<bool> stopFlag(false);

void sendFile(const std::string& videoFilename, SOCKET clientSocket) {
    std::ifstream inFile(videoFilename, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Cannot open file!" << std::endl;
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


void recordVideo(const std::string& outputFilename, int width, int height, int fps) {
    cv::VideoCapture cap(0);  // Mở camera
    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera!" << std::endl;
        return;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);  // Thiết lập chiều rộng
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);  // Thiết lập chiều cao
    cap.set(cv::CAP_PROP_FPS, fps);  // Thiết lập tốc độ khung hình của camera (nếu hỗ trợ)

    int codec = cv::VideoWriter::fourcc('M', 'P', '4', 'V');  // Codec cho .mp4
    cv::VideoWriter out(outputFilename, codec, fps, cv::Size(width, height));  // Mở VideoWriter

    if (!out.isOpened()) {
        // std::cerr << "Không thể mở file video để ghi." << std::endl;
        return;
    }

    cv::Mat frame;
    int delay = 1000 / fps;  // Độ trễ giữa các khung hình tính bằng mili giây
    cout << "Delay: " << delay << endl;
    while (true) {
        auto startTime = std::chrono::steady_clock::now();
        cap >> frame;  // Lấy khung hình từ camera
        if (frame.empty()) {
            break;
        }
        
        cv::flip(frame, frame, 1);

        out.write(frame); 

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

        // Đồng bộ hóa khung hình
        auto frameEndTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEndTime - startTime).count();
        if (elapsed < delay) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay - elapsed));
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