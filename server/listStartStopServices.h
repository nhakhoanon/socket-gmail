#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "ui.h"
using namespace std;

void listServices();
bool startService(const std::string& serviceName);
bool stopService(const std::string& serviceName);