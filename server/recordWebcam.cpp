#include "recordWebcam.h"

// Khởi tạo Media Foundation và thiết lập ghi hình
bool startRecording() {
    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
        cout << "Khởi tạo Media Foundation thất bại." << endl;
        return false;
    }

    // Thiết lập Sink Writer để ghi vào file video
    hr = MFCreateSinkWriterFromURL(L"recorded_video.mp4", nullptr, nullptr, &pSinkWriter);
    if (FAILED(hr)) {
        cout << "Không thể tạo Sink Writer." << endl;
        MFShutdown();
        return false;
    }

    // Thiết lập video format
    IMFMediaType* pMediaTypeOut = nullptr;
    hr = MFCreateMediaType(&pMediaTypeOut);
    hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
    hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, 800000);
    hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, 640, 480);
    hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, 30, 1);
    hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    hr = pSinkWriter->AddStream(pMediaTypeOut, &streamIndex);
    pMediaTypeOut->Release();

    // Thiết lập định dạng đầu vào từ webcam
    IMFMediaType* pMediaTypeIn = nullptr;
    hr = MFCreateMediaType(&pMediaTypeIn);
    hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, 640, 480);
    hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, 30, 1);
    hr = pSinkWriter->SetInputMediaType(streamIndex, pMediaTypeIn, nullptr);
    pMediaTypeIn->Release();

    hr = pSinkWriter->BeginWriting();
    if (FAILED(hr)) {
        cout << "Không thể bắt đầu ghi video." << endl;
        pSinkWriter->Release();
        MFShutdown();
        return false;
    }

    isRecording = true;
    cout << "Bắt đầu quay video..." << endl;
    return true;
}

void stopRecording() {
    if (isRecording) {
        pSinkWriter->Finalize();
        pSinkWriter->Release();
        MFShutdown();
        isRecording = false;
        cout << "Dừng quay video." << endl;
    }
}

// Hàm gửi file video qua socket
void sendVideoFile(const string& filename, SOCKET clientSocket) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "Không thể mở file để gửi." << endl;
        return;
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        send(clientSocket, buffer, file.gcount(), 0);
    }

    if (file.gcount() > 0) {
        send(clientSocket, buffer, file.gcount(), 0);
    }

    file.close();
    cout << "File đã được gửi tới client." << endl;
}