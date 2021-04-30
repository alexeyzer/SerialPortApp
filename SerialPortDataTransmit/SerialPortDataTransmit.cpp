﻿// SerialPortDataTransmit.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "SerialPortDataTransmit.h"
#include <commdlg.h>
#include <commctrl.h>
#include "Logger.h"
#include "View.h"

#pragma once 

#pragma comment( lib, "comctl32.lib")

#pragma comment( linker, "/manifestdependency:\"type='win32' \
        name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
        processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
        language='*'\"")



#define MAX_LOADSTRING 100
#define M_PI 3.1415926535897932384
#define OPEN_FILE_SUCCESS 1
#define OPEN_FILE_FAILURE 2
#define DEFAULT_BUTTON_HEIGHT 50
#define DEFAULT_BUTTON_WIDTH 150
#define READ_ACTION 3
#define SEND_ACTION 2
#define ESTABLISH_CONNECTION_ACTION 1
#define WM_GETLOGGER 0x1337

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND textbox1;
HWND textbox2;
HWND button;
HWND sendButton;
HWND readButton;

HWND openFileButton;


void openFile(HWND hWnd) {
	OPENFILENAMEA ofn;

}




// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERIALPORTDATATRANSMIT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERIALPORTDATATRANSMIT));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERIALPORTDATATRANSMIT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 800, nullptr, nullptr, hInstance, nullptr);
   /*textbox1 = CreateWindow(_T("EDIT"),
	   _T(""),
	   WS_BORDER | WS_CHILD | WS_VISIBLE,
	   0,
	   50,
	   DEFAULT_BUTTON_WIDTH,
	   DEFAULT_BUTTON_HEIGHT,
	   hWnd,
	   NULL,
	   NULL,
	   NULL
   );
   textbox2 = CreateWindow(_T("EDIT"),
	   _T(""),
	   WS_BORDER | WS_CHILD | WS_VISIBLE,
	   0,
	   100,
	   DEFAULT_BUTTON_WIDTH,
	   DEFAULT_BUTTON_HEIGHT,
	   hWnd,
	   NULL,
	   NULL,
	   NULL
   );
   button = CreateWindow(_T("BUTTON"),
	   _T("Подключить"),
	   WS_CHILD | WS_VISIBLE | WS_BORDER,
	   120,
	   150,
	   DEFAULT_BUTTON_WIDTH,
	   DEFAULT_BUTTON_HEIGHT,
	   hWnd,
	   (HMENU)ESTABLISH_CONNECTION_ACTION,
	   NULL,
	   NULL);
   sendButton = CreateWindow(_T("BUTTON"),
	   _T("тест отправить"),
	   WS_CHILD | WS_VISIBLE | WS_BORDER,
	   120,
	   200,
	   DEFAULT_BUTTON_WIDTH,
	   DEFAULT_BUTTON_HEIGHT,
	   hWnd,
	   (HMENU)SEND_ACTION,
	   NULL,
	   NULL);
   readButton = CreateWindow(_T("BUTTON"),
	   _T("тест считка"),
	   WS_CHILD | WS_VISIBLE | WS_BORDER,
	   120,
	   250,
	   DEFAULT_BUTTON_WIDTH,
	   DEFAULT_BUTTON_HEIGHT,
	   hWnd,
	   (HMENU)READ_ACTION,
	   NULL,
	   NULL);
   openFileButton = CreateWindow(_T("BUTTON"),
	   _T("Открытие файла"),
	   WS_CHILD | WS_VISIBLE | WS_BORDER,
	   120,
	   300,
	   DEFAULT_BUTTON_WIDTH,
	   DEFAULT_BUTTON_HEIGHT,
	   hWnd,
	   (HMENU)4,
	   NULL,
	   NULL);
   
   */
   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   return TRUE;
}



Logger logger;

ViewHandler handle;

char* receivingComport = new char[6];
char* sendingComport = new char[6];

inline void fillChooseOneListWithActiveComports(ViewHandler handle) {
	TCHAR lpTargetPath[5000];
	char comportBase[4] = "COM";
	wchar_t* device = new wchar_t[6];
	for (int i = 0; i < 100; i++) { // проверка компортов с 0 по 100
		char temp[6];
		memset(temp, '\0', 6);
		snprintf(temp, 6, "%s%i", comportBase, i);

		MultiByteToWideChar(CP_ACP, 0, temp, 6, device, 6);
		int a = 0;
		if (QueryDosDevice(device, lpTargetPath, 5000) != 0) {
			handle.addValueToList(temp);
		}
		else {
			int a = 0;
		}
		int jopa = 0;

	}
	delete device;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		logger=Logger(hWnd, 300, 300);
		handle = ViewHandler(hWnd,NULL,NULL,NULL);
		
		handle.createChooseOneList(100);
		fillChooseOneListWithActiveComports(handle);

		handle.changeCurrentView(CHOOSECOMPORT_RECIEVING_VIEW,NULL,NULL);
		
	}
	break;
	case WM_MOUSEMOVE: 
	{
		TRACKMOUSEEVENT mousePos;
		mousePos.cbSize = sizeof(TRACKMOUSEEVENT);
		mousePos.dwFlags = TME_HOVER;
		mousePos.dwHoverTime = HOVER_DEFAULT;
		mousePos.hwndTrack = hWnd;
		TrackMouseEvent(&mousePos);
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		handle.sendMousePos(xPos, yPos);
	}
	break;
	case WM_LBUTTONDOWN: 
	{
		switch (handle.getCurrentView()) {
		case CHOOSECOMPORT_RECIEVING_VIEW:
		{
			memcpy(receivingComport, handle.getActiveListItemText(), 7);
			handle.changeCurrentView(CHOOSECOMPORT_SENDING_VIEW,NULL, NULL);
			handle.DeleteItemFromList(handle.getActiveListItemID());
		}
		break;
		case CHOOSECOMPORT_SENDING_VIEW:
		{
			memcpy(sendingComport, handle.getActiveListItemText(), 7);
			handle.changeCurrentView(CONNECT_USING_SELECTEDCOMPORTS_VIEW, sendingComport, receivingComport);
			handle.deleteChooseOneList();
			handle.createChooseOneList(550);
			handle.addValueToList("Connect as main");
			handle.addValueToList("Подключиться");

		}
		break;
		case CONNECT_USING_SELECTEDCOMPORTS_VIEW:
		{
			handle.changeCurrentView(CONNECTING_VIEW, NULL, NULL);
			
		}
		break;
		case CONNECTING_VIEW:
		{
			handle.changeCurrentView(CONNECT_SUCCESS_VIEW, NULL, NULL);
		}
		break;
		case CONNECT_SUCCESS_VIEW: 
		{
		}
		break;
		}
	}
	break;
    case WM_COMMAND:
		{
			switch (HIWORD(wParam))
			{
			}
			switch (LOWORD(wParam))
			{
				case ESTABLISH_CONNECTION_ACTION:
				{
					TCHAR text1[30];
					TCHAR text2[30];
					int com1;
					int com2;

					GetWindowText(textbox1, text1, GetWindowTextLength(textbox1) + 1);
					GetWindowText(textbox2, text2, GetWindowTextLength(textbox1) + 1);
					logger.addMessage(text1);
					logger.addMessage(text2);
					com1 = _wtoi(text1);
					com2 = _wtoi(text2);
					WorkWithCom(hWnd, com1, com2,logger);
					break;
				}
				case SEND_ACTION:
				{
					logger.addMessage(L"hello\0");
					trytowrrite(logger);
					break;
				}
				case READ_ACTION:
				{
					trytoread(logger);
					break;
				}
			}
        }
        break;
   
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
			case VK_UP:
			{
				handle.changeStatusToError();
			}
			break;
			case VK_DOWN:
			{
				handle.changeStatusToSuccess();
			}
			break;
			case VK_LEFT:
			{
				handle.changeActiveListItemTo(3);
				handle.changeStatusToWaiting();
			}
			break;
			case VK_RIGHT:
			{
				handle.changeActiveListItemTo(1);
				handle.changeStatusToRainbow();
			}
			break;
		}
		break;
	}

	default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


