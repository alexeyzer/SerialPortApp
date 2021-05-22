#pragma once

#include "framework.h"
#include <vector>
#include <string>

#define LOG_SIZE_DEFAULT 150

class Logger {
private:
	HDC parentHDC;
	HWND parentHWND;
	std::vector<LPCWSTR> logHist;
	void redrawLog();
	void drawLast();
public:
	Logger();
	Logger(HWND parentWindow, int posX, int posY);
	~Logger();
	void addMessage(LPCWSTR message);
	
};