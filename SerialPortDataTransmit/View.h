#pragma once

#include "framework.h"
#include <math.h>
#include <tuple>
#include "Source.h"
#include <string>

#define COLOR_RED RGB(255,0,0)
#define COLOR_BLUE RGB(0,0,255)

#define ANIMATION_WAVE 1000

#define ANIMATIONPERCENT 0.95
#define WAVEBALLSIZE 10



#define CONNECTION_ESTABLISHED 11

#define CHOOSECOMPORT_RECIEVING_VIEW 1
#define CHOOSECOMPORT_SENDING_VIEW 2
#define START_VIEW 3
#define CONNECT_USING_SELECTEDCOMPORTS_VIEW 4
#define CONNECTING_VIEW 5
#define CONNECT_SUCCESS_VIEW 6
#define CONNECT_SUCCESS_VIEW_MAIN 12
#define CONNECT_FAILURE_VIEW 7
#define CHOOSE_FILE_VIEW 8
#define CHOOSE_DIR_VIEW 9
#define CONNECTED_VIEW 10
#define INPUTNICKNAME_VIEW 13

#define SEND_FILE_CHOICE 1
#define OPEN_DIR_CHOICE 2
#define OPEN_FILE_CHOICE 3

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
	void updateNickname(char* name, size_t len);
	size_t getSelectedPC();
	void addPC(LPCWSTR nickname, uint16_t length, uint16_t id);
	void DeleteItemFromList(size_t itemID);
	const char* getActiveListItemText();
};