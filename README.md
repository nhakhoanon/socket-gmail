# 📡 Remote Control of Computer via Email

## 📘 Project Introduction

This project was developed by a group of students from the University of Science – VNUHCM, Faculty of Information Technology.

We collaborated on all aspects of the project including design, implementation, testing, and documentation. The group worked in a synchronized and agile manner, ensuring each member contributed their strengths effectively.

**Student Information:**

| Student ID | Full Name             |
| ---------- | --------------------- |
| 23127015   | Nguyễn Hoàng Anh Khoa |
| 23127166   | Nguyễn Hoàng Đăng     |
| 23127341   | Ngô Trần Quang Đạt    |
| 23127371   | Lê Võ Xuân Hưng       |

The application was developed with the goal of enabling **remote system monitoring, management, and control** through email-based communication.

Built on a **Client-Server architecture**, the system allows the **Client** to send commands via Gmail, which the **Server** receives, processes, and then executes accordingly.

The application prioritizes **efficiency and simplicity**, eliminating the need for complex remote-control platforms or third-party software. Instead, it leverages **email as the main communication channel**, making it easy to integrate into existing infrastructures—even in **restricted or unstable network environments**.

---

## 🚀 Getting Started

After downloading the `Socket_Gmail` project folder, you should see the following structure:

```
Socket_Gmail/
├── client/
│   └── curl/
│       ├── bin/
│       └── ...
├── server/
│   └── *.cpp
├── OpenCV-MinGW-Build-OpenCV-4.5.5-x64/
│   ├── include/
│   └── x64/mingw/bin/
```

---

## ⚙️ Setup Instructions

### 1. Environment Setup

Before running the application, please ensure:

**Add these paths to your Environment Variables (**``**)**:

- `<...>\Socket_Gmail\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\x64\mingw\bin`
- `<...>\Socket_Gmail\client\curl\bin`

> Replace `<...>` with the actual path where you extracted the project.

**Install MinGW-w64 (64-bit) compiler**

- Required to compile C++ code and work with OpenCV, CURL, and WinAPI libraries.

---

### 2. Running the Server

1. Open **Command Prompt** in the `server/` directory.
2. Compile the server source files using:
   ```bash
   g++ *.cpp -o server.exe ^
       -I..\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\include ^
       -L..\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\x64\mingw\lib ^
       -lopencv_highgui455 -lopencv_videoio455 -lopencv_core455 ^
       -lws2_32 -lgdi32
   ```
3. Run `server.exe` as administrator.

---

### 3. Running the Client

1. Open **Command Prompt** in the `client/` directory.
2. Compile the client source files using:
   ```bash
   g++ -I./curl/include -L./curl/lib *.cpp -llibcurl -o client.exe -lwinmm -lws2_32
   ```
3. Run `client.exe` file.

---

## ✉️ Command Syntax via Gmail

To control the system remotely, send an email from the client to the server with the following formats:

| 🔧 Feature        | ✉️ Subject   | 📝 Body Format                               |
| ----------------- | ------------ | -------------------------------------------- |
| Shutdown          | PROJECT\_MMT | shutdown\nIP: {IP}                           |
| Restart           | PROJECT\_MMT | restart\nIP: {IP}                            |
| List Applications | PROJECT\_MMT | listapp\nIP: {IP}                            |
| Open Application  | PROJECT\_MMT | openapp\nIP: {IP}\nApp name: {App name}      |
| Close Application | PROJECT\_MMT | closeapp\nIP: {IP}\nApp name: {App name}     |
| Get File          | PROJECT\_MMT | getfile\nIP: {IP}\nFile path: {File path}    |
| Delete File       | PROJECT\_MMT | deletefile\nIP: {IP}\nFile path: {File path} |
| List Services     | PROJECT\_MMT | listservices\nIP: {IP}                       |
| Start Service     | PROJECT\_MMT | startservice\nIP: {IP}\nService name: {Name} |
| Stop Service      | PROJECT\_MMT | stopservice\nIP: {IP}\nService name: {Name}  |
| Keylogger         | PROJECT\_MMT | keylogger\nIP: {IP}\nTime: {Time in seconds} |

**Note:** Replace placeholders like `{IP}`, `{App name}`, `{File path}`, `{Service name}`, and `{Time}` with actual values when sending commands.

---

## ✨ Project Highlights

- 🛡️ Secure communication via email
- 🚀 Lightweight and easy deployment
- ⚙️ Full control over system apps, services, and files
- 📊 Effective in low or unstable network environments

---

## 🙏 Acknowledgments

Special thanks to our professors and peers at the University of Science – VNUHCM for their support and guidance throughout this project.

