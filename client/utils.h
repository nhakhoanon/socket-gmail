#pragma once
#include <string>
#include <cctype>
#include <vector>

std::string stripString(std::string str); // Remove leading and trailing spaces
std::string stripAndLowerString(std::string str); // Remove leading and trailing spaces and convert to lowercase
std::string createHTML(std::string str); // Create HTML content
std::string createHtmlTable(std::vector<std::string> headers, std::vector<std::vector<std::string>> data); // Create HTML table