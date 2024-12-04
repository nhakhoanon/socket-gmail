#include "utils.h"

using namespace std;

// Remove leading and trailing spaces
string stripString(string str) {
    string res = "";
    int start = 0, end = str.length() - 1;
    while (start < str.length() && str[start] == ' ') start++;
    while (end >= 0 && str[end] == ' ') end--;

    for (int i = start; i <= end; i++) {
        if (isprint(str[i])) res += str[i];
    }
    return res;
}

// Remove leading and trailing spaces and convert to lowercase
string stripAndLowerString(string str) {
    string res = "";
    int start = 0, end = str.length() - 1;
    while (start < str.length() && str[start] == ' ') start++;
    while (end >= 0 && str[end] == ' ') end--;

    for (int i = start; i <= end; i++) {
        if (isprint(str[i])) res += tolower(str[i]);
    }
    return res;
}