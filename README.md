This project was developed by a group of students from the University of Science – VNUHCM, Faculty of Information Technology.

Student ID	Full Name
23127015	Nguyễn Hoàng Anh Khoa
23127166	Nguyễn Hoàng Đăng
23127341	Ngô Trần Quang Đạt
23127371	Lê Võ Xuân Hưng

We collaborated on all aspects of the project including design, implementation, testing, and documentation. The group worked in a synchronized and agile manner, ensuring each member contributed their strengths effectively.

# 📡 Remote Control of Computer via Email

## 📘 Project Introduction

This application was developed with the goal of enabling **remote system monitoring, management, and control** through email-based communication.

Built on a **Client-Server architecture**, the system allows the **Client** to send commands via Gmail, which the **Server** receives, processes, and then executes accordingly.

The application prioritizes **efficiency and simplicity**, eliminating the need for complex remote-control platforms or third-party software. Instead, it leverages **email as the main communication channel**, making it easy to integrate into existing infrastructures—even in **restricted or unstable network environments**.

---

## 🚀 1. Getting Started

After downloading the `Socket_Gmail` project folder, you should see the following structure:

Socket_Gmail/
│
├── client/
│ └── curl/
│ ├── bin/
│ └── ...
│
├── server/
│ └── *.cpp
│
├── OpenCV-MinGW-Build-OpenCV-4.5.5-x64/
│ ├── include/
│ └── x64/mingw/bin/

---

## ⚙️ 2. Setup Instructions

### 2.1. Environment Setup

Before running the application, please ensure:

✅ **Add these paths to your Environment Variables (`Path`):**

- `<...>\Socket_Gmail\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\x64\mingw\bin`
- `<...>\Socket_Gmail\client\curl\bin`

> Replace `<...>` with the actual path where you extracted the project.

✅ **Install MinGW-w64 (64-bit) compiler**

- Required to compile C++ code and work with OpenCV, CURL, and WinAPI libraries.

---

### 2.2. Running the Server

📍 **Steps:**

1. Open **Command Prompt** in the `server/` directory.
2. Compile the server source files using:
g++ *.cpp -o server.exe ^
 -I..\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\include ^
 -L..\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\x64\mingw\lib ^
 -lopencv_highgui455 -lopencv_videoio455 -lopencv_core455 ^
 -lws2_32 -lgdi32
3. Run server.exe file as administrator.
### 2.2. Running the Client
📍 **Steps:**

1. Open Command Prompt in the client/ directory.

2. Compile the client source files using:

g++ -I./curl/include -L./curl/lib *.cpp -llibcurl -o client.exe -lwinmm -lws2_32
3. Run client.exe file.
✉️ 3. Command Syntax via Gmail
To control the system remotely, send an email from the client to the server with the following Subject and Body formats:

🔧 Feature	        ✉️ Subject	            📝 Body Format
Shutdown	          PROJECT_MMT             shutdown	IP: {IP}
Restart	            PROJECT_MMT             restart	IP: {IP}
List Applications	  PROJECT_MMT             listapp	IP: {IP}
Open Application	  PROJECT_MMT             openapp	IP: {IP}
                                            App name: {App name}
Close Application	  PROJECT_MMT             closeapp	IP: {IP}
                                            App name: {App name}
Get File	          PROJECT_MMT             getfile	IP: {IP}
                                            File path: {File path}
Delete File	        PROJECT_MMT             deletefile	IP: {IP}
                                            File path: {File path}
List Services	      PROJECT_MMT             listservices	IP: {IP}
Start Service	      PROJECT_MMT             startservice	IP: {IP}
                                            Service name: {Service name}
Stop Service	      PROJECT_MMT             stopservice	IP: {IP}
                                            Service name: {Service name}
Keylogger	          PROJECT_MMT             keylogger	IP: {IP}
                                            Time: {Time in seconds}

📌 Note: Replace placeholders like {IP}, {App name}, {File path}, {Service name}, and {Time} with actual values when sending commands.
