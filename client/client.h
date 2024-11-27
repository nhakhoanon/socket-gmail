#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <map>
#include "receiveData.h"
#include "getFileClient.h"
#include "mail.h"

#define bufferSize 1024
using namespace std;

const string EMAIL_ACCOUNT = "khoanguyen.2005ct@gmail.com";
const string EMAIL_PASSWORD = "pdla nygb assw qxnr";