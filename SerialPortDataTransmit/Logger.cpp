#include "Logger.h"
#include <string>
#include "Source.h"


HFONT hFont;

Logger::Logger() {

}

Logger::Logger(HWND parentWindow, int posX, int posY) {
	hFont = CreateFont(30, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"Consolas");
	
	parentHDC = GetDC(parentWindow);
	
	HBRUSH solid = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(parentHDC, solid);
	
	SetTextColor(parentHDC, RGB(0, 255, 0));
	SetBkMode(parentHDC, TRANSPARENT);
	SelectObject(parentHDC, hFont);
	parentHWND = parentWindow;
	redrawLog();
}

void Logger::redrawLog() {
	int a = Rectangle(parentHDC, 300, 0, 1000, 1000);
	for (int i = 0; i < logHist.size(); i++) {
		LPCWSTR temp = logHist[i];
		TextOut(parentHDC, 300, 30 * i, temp, wcslen(temp));
	}
}

Logger::~Logger() {

}

void Logger::drawLast() {
	LPCWSTR last = logHist.back();
	TextOut(parentHDC, 300,100+(50*logHist.size()), last,wcslen(last));
}

void Logger::addMessage(LPCWSTR message) {
	
	logHist.push_back(message);
	redrawLog();
}