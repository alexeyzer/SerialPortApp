#pragma once

#include "framework.h"
#include <math.h>
#include <tuple>

#define COLOR_RED RGB(255,0,0)
#define COLOR_BLUE RGB(0,0,255)

#define ANIMATION_WAVE 1000

#define ANIMATIONPERCENT 0.95
#define WAVEBALLSIZE 10

class ViewHandler{
private: 
	COLORREF backgroundColor;
	LPWSTR Message;
	HANDLE viewThread;
	DWORD WINAPI renderView(LPVOID t);
	HWND parentHWND;
	void changeColorToSmooth(std::tuple<double, double, double>);
	void startView();
public:
	ViewHandler();
	ViewHandler(HWND hWnd, COLORREF backgroundColor, int animtationType, LPWSTR startMessage);
	void updateMessage(LPWSTR newMessage);
	void changeStatusToError();
	void changeStatusToWaiting();
	void changeStatusToSuccess();
	void changeStatusToRainbow();
};