// SerialPortDataTransmit.cpp : Определяет точку входа для приложения.
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





DWORD WINAPI Thread1(LPVOID t) {  //функция ,выполняемая первым потоком 

	HDC hdc = GetDC((HWND)t);
	RECT black;
	black.top = 400;
	black.bottom = 1000;			//координаты прямоугольника,который закрашивает предыдущее положение фигуры
	black.left = 0;
	black.right = 1000;
	int speed = 1;
	while (true) {
		for (double x = M_PI / 2; x < (5 * M_PI) / 2; x += (0.002 * speed)) {
			HPEN pen = CreatePen(PS_DASH, 2, RGB(120 * (sin(2 * x) + 1), 120 * (cos(4 * x) + 1), 120 * (cos(3 * x) + 1)));						//цвет обводки фигруы меняется в зависимости от x
			HBRUSH brush = CreateSolidBrush(RGB(120 * (sin(2 * x) + 1), 120 * (cos(4 * x) + 1), 120 * (cos(3 * x) + 1)));
			SelectObject(hdc, pen);
			SelectObject(hdc, brush);
			Ellipse(hdc, 200, 200, 400, 400);
			Sleep(11);		//задержка 
			DeleteObject(pen);						//удаление кисти и ручки ,чтобы избежать ошибок (иначе программа вылетает через какое-то время)
			DeleteObject(brush);
		}
	}
	return 0;
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
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERIALPORTDATATRANSMIT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 800, nullptr, nullptr, hInstance, nullptr);
   textbox1 = CreateWindow(_T("EDIT"),
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		logger=Logger(hWnd, 300, 300);
		handle = ViewHandler(hWnd,NULL,NULL,NULL);
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
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
				handle.changeStatusToWaiting();
			}
			break;
			case VK_RIGHT:
			{
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


