// SerialPortDataTransmit.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "SerialPortDataTransmit.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND textbox1;
HWND textbox2;
HWND button;
HWND button2;
HWND button3;

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

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   textbox1 = CreateWindow(_T("EDIT"),
	   _T(""),
	   WS_BORDER | WS_CHILD | WS_VISIBLE,
	   0,
	   25,
	   120,
	   25,
	   hWnd,
	   NULL,
	   NULL,
	   NULL
   );
   textbox2 = CreateWindow(_T("EDIT"),
	   _T(""),
	   WS_BORDER | WS_CHILD | WS_VISIBLE,
	   0,
	   50,
	   120,
	   25,
	   hWnd,
	   NULL,
	   NULL,
	   NULL
   );
   button = CreateWindow(_T("BUTTON"),
	   _T("Подключить главный"),
	   WS_CHILD | WS_VISIBLE | WS_BORDER,
	   120,
	   25,
	   120,
	   33,
	   hWnd,
	   (HMENU)1,
	   NULL,
	   NULL);
   button2 = CreateWindow(_T("BUTTON"),
	   _T("Подключить ведомый"),
	   WS_CHILD | WS_VISIBLE | WS_BORDER,
	   120,
	   50,
	   120,
	   33,
	   hWnd,
	   (HMENU)2,
	   NULL,
	   NULL);
   button3 = CreateWindow(_T("BUTTON"),
	   _T("тест считка"),
	   WS_CHILD | WS_VISIBLE | WS_BORDER,
	   120,
	   80,
	   120,
	   33,
	   hWnd,
	   (HMENU)3,
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

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
		{
			switch (HIWORD(wParam))
			{
			}
			switch (LOWORD(wParam))
			{
				case 1:
				{
					TCHAR text1[30];
					TCHAR text2[30];
					int com1;
					int com2;

					GetWindowText(textbox1, text1, GetWindowTextLength(textbox1) + 1);
					GetWindowText(textbox2, text2, GetWindowTextLength(textbox1) + 1);
					MessageBox(hWnd,text1, L"Caption", MB_OK);
					MessageBox(hWnd, text2, L"Caption", MB_OK);
					com1 = _wtoi(text1);
					com2 = _wtoi(text2);
					WorkWithCom(hWnd, com1, com2, true);
					break;
				}
				case 2:
				{
					TCHAR text1[30];
					TCHAR text2[30];
					int com1;
					int com2;

					GetWindowText(textbox1, text1, GetWindowTextLength(textbox1) + 1);
					GetWindowText(textbox2, text2, GetWindowTextLength(textbox1) + 1);
					MessageBox(hWnd, text1, L"Caption", MB_OK);
					MessageBox(hWnd, text2, L"Caption", MB_OK);
					com1 = _wtoi(text1);
					com2 = _wtoi(text2);
					WorkWithCom(hWnd, com1, com2, false);
					break;
				}
				case 3:
				{
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
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
