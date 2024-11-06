#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

using namespace std;

void listServices();
bool startService(const std::string& serviceName);
bool stopService(const std::string& serviceName);