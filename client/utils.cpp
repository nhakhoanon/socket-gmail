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

// Create HTML content
string createHTML(string str) {
    string strHTML1 = "<div dir=\"ltr\"><div dir=\"ltr\"></div><blockquote style=\"margin:0; border-left:1px solid #ccc; padding-left:1ex;\"><div dir=\"ltr\"><div style=\"margin:0 auto; max-width:600px;\"><table align=\"center\" width=\"600\" style=\"border-collapse:collapse;\"><tbody><tr><td style=\"border-bottom:5px solid #333957; padding:20px 0; text-align:center;\"></td></tr></tbody></table></div><div style=\"margin:0 auto; max-width:600px;\"><table align=\"center\" width=\"600\" style=\"border-collapse:collapse;\"><tbody><tr><td style=\"padding:20px; text-align:left; border:1px solid #ddd;\"><div style=\"font-family:'Helvetica Neue', Arial, sans-serif; font-size:16px; line-height:22px; color:#555;\"><h1>PROJECT_MMT</h1>";
    string strHTML2 = "</div><div style=\"font-family:'Helvetica Neue', Arial, sans-serif; font-size:14px; line-height:20px; color:#525252;\"><p>Trân trọng,</p><p>PROJECT_MMT Team</p></div></td></tr></tbody></table></div></div></blockquote></div>";

    string tmp = "";
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '\n') tmp += "<br>";
        else tmp += str[i];
    }

    return strHTML1 + tmp + strHTML2;
}

// Create HTML table
string createHtmlTable(vector<string> headers, vector<vector<string>> data) {
    string strHTML = "<table style=\"border-collapse: collapse; width: 100%;\">"; // "<table border=\"1\" style=\"width:100%\"><tr>";
    strHTML += "<tr>";
    for (int i = 0; i < headers.size(); i++) {
        strHTML += "<th style=\"border: 1px solid #dddddd; text-align: left; padding: 8px;\">" + headers[i] + "</th>";
    }
    strHTML += "</tr>";

    for (int i = 0; i < data.size(); i++) {
        if (i % 2 == 0) strHTML += "<tr style=\"background-color: #f2f2f2;\">"; else strHTML += "<tr>";
        // strHTML += "<tr>";
        for (int j = 0; j < data[i].size(); j++) {
            strHTML += "<td style=\"border: 1px solid #dddddd; text-align: left; padding: 8px;\">" + data[i][j] + "</td>";
        }
        strHTML += "</tr>";
    }

    strHTML += "</table>";

    return strHTML;
}