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

// Create HTML table for syntax
string createSyntaxHtmlTable(SyntaxType type) {
    vector<string> headers;
    vector<vector<string>> data;
    switch (type) {
    case LISTAPP:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "List Applications", "PROJECT_MMT listapp", "IP: {IP}"});
        break;
    case OPENAPP:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Open Application", "PROJECT_MMT openapp", "IP: {IP}\nApp name: {App name}"});
        break;
    case CLOSEAPP:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Close Application", "PROJECT_MMT closeapp", "IP: {IP}\nApp name: {App name}"});
        break;
    case CLOSEFILE:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Close File", "PROJECT_MMT closefile", "IP: {IP}\nFile name: {File name}"});
        break;
    case GETFILE:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Get File", "PROJECT_MMT getfile", "IP: {IP}\nFile path: {File path}"});
        break;
    case STARTSERVICE:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Start Service", "PROJECT_MMT startservice", "IP: {IP}\nService name: {Service name}"});
        break;
    case STOPSERVICE:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Stop Service", "PROJECT_MMT stopservice", "IP: {IP}\nService name: {Service name}"});
        break;
    case LISTSERVICES:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "List Services", "PROJECT_MMT listservices", "IP: {IP}"});
        break;
    case KEYLOGGER:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Key Logger", "PROJECT_MMT keylogger", "IP: {IP}"});
        break;
    case CAPTURESCREEN:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Capture Screen", "PROJECT_MMT capturescreen", "IP: {IP}"});
        break;
    case STARTWEBCAM:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Start Webcam", "PROJECT_MMT startwebcam", "IP: {IP}"});
        break;
    case STOPWEBCAM:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Stop Webcam", "PROJECT_MMT stopwebcam", "IP: {IP}"});
        break;
    case SHUTDOWN:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Shut down", "PROJECT_MMT shutdown", "IP: {IP}"});
        break;
    case RESTART:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Restart", "PROJECT_MMT restart", "IP: {IP}"});
        break;
    case ALL:
        headers.push_back("Feature");
        headers.push_back("Subject");
        headers.push_back("Body");
        data.push_back({ "Shut down", "PROJECT_MMT shutdown", "IP: {IP}"});
        data.push_back({ "Restart", "PROJECT_MMT restart", "IP: {IP}"});
        data.push_back({ "List Applications", "PROJECT_MMT listapp", "IP: {IP}"});
        data.push_back({ "Open Application", "PROJECT_MMT openapp", "IP: {IP}\nApp name: {App name}"});
        data.push_back({ "Close Application", "PROJECT_MMT closeapp", "IP: {IP}\nApp name: {App name}"});
        data.push_back({ "Get File", "PROJECT_MMT getfile", "IP: {IP}\nFile path: {File path}"});
        data.push_back({ "Close File", "PROJECT_MMT closefile", "IP: {IP}\nFile name: {File name}"});
        data.push_back({ "List Services", "PROJECT_MMT listservices", "IP: {IP}"});
        data.push_back({ "Start Service", "PROJECT_MMT startservice", "IP: {IP}\nService name: {Service name}"});
        data.push_back({ "Stop Service", "PROJECT_MMT stopservice", "IP: {IP}\nService name: {Service name}"});
        data.push_back({ "Key Logger", "PROJECT_MMT keylogger", "IP: {IP}"});
        data.push_back({ "Capture Screen", "PROJECT_MMT capturescreen", "IP: {IP}"});
        data.push_back({ "Start Webcam", "PROJECT_MMT startwebcam", "IP: {IP}"});
        data.push_back({ "Stop Webcam", "PROJECT_MMT stopwebcam", "IP: {IP}"});
        break;
    default:
        break;
    }

    return createHtmlTable(headers, data);
}