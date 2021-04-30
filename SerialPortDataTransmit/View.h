#pragma once

#include "framework.h"
#include <math.h>
#include <tuple>

#define COLOR_RED RGB(255,0,0)
#define COLOR_BLUE RGB(0,0,255)

#define ANIMATION_WAVE 1000

#define ANIMATIONPERCENT 0.95
#define WAVEBALLSIZE 10


#define CHOOSECOMPORT_RECIEVING_VIEW 1
#define CHOOSECOMPORT_SENDING_VIEW 2
#define START_VIEW 3 


class ViewHandler{
private: 
	COLORREF backgroundColor;
	LPWSTR Message;
	HANDLE viewThread;
	HANDLE viewThreadRedraw;
	DWORD WINAPI renderView(LPVOID t);
	HWND parentHWND;
	int currentView;
	void changeColorToSmooth(std::tuple<double, double, double>);
	void startView();
public:
	int getCurrentView();
	void changeCurrentView(int newView);
	ViewHandler();
	ViewHandler(HWND hWnd, COLORREF backgroundColor, int animtationType, LPWSTR startMessage);
	void updateMessage(LPWSTR newMessage);
	void changeStatusToError();
	void changeStatusToWaiting();
	void changeStatusToSuccess();
	void changeStatusToRainbow();
	void createChooseOneList();
	void addValueToList(const char* value);
	void changeActiveListItemTo(size_t pos);
	void sendMousePos(int posX, int posY);
	size_t getActiveListItemID();
	const char* getActiveListItemText();
};