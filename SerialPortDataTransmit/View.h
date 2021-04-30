#pragma once

#include "framework.h"
#include <math.h>
#include <tuple>
#include "Source.h"

#define COLOR_RED RGB(255,0,0)
#define COLOR_BLUE RGB(0,0,255)

#define ANIMATION_WAVE 1000

#define ANIMATIONPERCENT 0.95
#define WAVEBALLSIZE 10


#define CHOOSECOMPORT_RECIEVING_VIEW 1
#define CHOOSECOMPORT_SENDING_VIEW 2
#define START_VIEW 3
#define CONNECT_USING_SELECTEDCOMPORTS_VIEW 4
#define CONNECTING_VIEW 5
#define CONNECT_SUCCESS_VIEW 6
#define CONNECT_FAILURE_VIEW 7


class ViewHandler{
private: 
	COLORREF backgroundColor;
	LPWSTR Message;
	HANDLE viewThread;
	HANDLE viewThreadRedraw;
	DWORD WINAPI renderView(LPVOID t);
	HWND parentHWND;
	int currentView;
	void changeColorToSmooth(float RValue,float GValue,float BValue);
	void startView();
public:
	int getCurrentView();
	void changeCurrentView(int newView,const char* param1,const char* param2);
	ViewHandler();
	ViewHandler(HWND hWnd, COLORREF backgroundColor, int animtationType, LPWSTR startMessage);
	void updateMessage(const wchar_t* text, size_t length);
	void changeStatusToError();
	void changeStatusToWaiting();
	void changeStatusToSuccess();
	void changeStatusToRainbow();
	void createChooseOneList(int posX);
	void deleteChooseOneList();
	void addValueToList(const char* value);
	void changeActiveListItemTo(size_t pos);
	void sendMousePos(int posX, int posY);
	size_t getActiveListItemID();
	void DeleteItemFromList(size_t itemID);
	const char* getActiveListItemText();
};