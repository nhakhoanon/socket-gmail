#pragma once

#include <iostream>
#include <windows.h>
#include <vector> 
#include <sstream>

using namespace std;

//Fixed------
void gotoxy(int x, int y);
void hideCursor();
void getConsoleSize(int &width, int &height);
void setTextColor(int color);
void resetTextColor();
//-------

class FrameMenu {
    private: //Fixed
        string rectangle; 
        double width;
        double height;
    public:
        FrameMenu();
        void printRectangleInCenter(); //Fixed
        void displayAnimation1(int delay, string s); //Min time: delay * 5
        void printContentInRectangle(vector<string> listOfSentences, int x, int y);
};

void printCentered(const std::string& str, int y);
void printCenteredInRectangle(int width, int height, const std::string& str, int delta);// In content ở giữa hcn
