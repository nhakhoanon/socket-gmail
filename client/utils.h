#pragma once
#include <string>
#include <cctype>
#include <vector>

enum SyntaxType
{
    ALL,
    LISTAPP,
    OPENAPP,
    CLOSEAPP,
    DELETEFILE,
    GETFILE,
    STARTSERVICE,
    STOPSERVICE,
    LISTSERVICES,
    KEYLOGGER,
    CAPTURESCREEN,
    STARTWEBCAM,
    STOPWEBCAM,
    SHUTDOWN,
    RESTART
};

std::string stripString(std::string str); // Remove leading and trailing spaces
std::string stripAndLowerString(std::string str); // Remove leading and trailing spaces and convert to lowercase
std::string createHTML(std::string str); // Create HTML content
std::string createHtmlTable(std::vector<std::string> headers, std::vector<std::vector<std::string>> data); // Create HTML table
std::string createSyntaxHtmlTable(SyntaxType); // Create HTML table for syntax