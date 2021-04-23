

#include "View.h"


#define M_PI 3.1415926535897931084
#define GRADIENT_CHANGE_STEPS 1000

double red = 1;
double green = 0;
double blue = 0;

bool COLORCHANGEFLAG = false;
HANDLE colorThread;

double wantedRed = 0;
double wantedGreen = 0;
double wantedBlue = 0;

const wchar_t * message = TEXT("fuck me\0");

DWORD WINAPI changeColorProc(LPVOID t) {
	COLORCHANGEFLAG = true;
	std::tuple<double,double,double>* wantedColor = static_cast<std::tuple<double, double, double>*>(t);
	RGBTRIPLE rgb;
	double diffRed = wantedRed- red;
	double diffGreen = wantedGreen - green;
	double diffBlue = wantedBlue - blue;
	double redBuff = red;
	double greenBuff = green;
	double blueBuff = blue;
	constexpr double step = M_PI / GRADIENT_CHANGE_STEPS*2;
	for (double x = 0; x < M_PI/2; x += step) {
		red = (sin(x) * diffRed)+redBuff;
		green = (sin(x) * diffGreen) + greenBuff;
		blue = (sin(x) * diffBlue) + blueBuff;
		Sleep(10);
	}
	COLORCHANGEFLAG = false;
	return 0;
}

DWORD WINAPI gradientProc(LPVOID t) {
	HWND parentHWND = static_cast<HWND>(t);
	HDC hdc = GetDC(parentHWND);
	RECT screenRect;
	GetClientRect(parentHWND, &screenRect);
	FillRect(hdc, &screenRect, CreateSolidBrush(RGB(120,120,0)));
	long topValue = screenRect.bottom * ANIMATIONPERCENT;
	long diff = (screenRect.bottom - topValue) / 2;
	double step = (2*M_PI) / (screenRect.right + WAVEBALLSIZE + 100);
	double phase = 0;
	HFONT font = CreateFont(200, 0, 0, 0,0, 0, 0, 0, 0, OUT_OUTLINE_PRECIS, CLIP_STROKE_PRECIS, 3, FF_ROMAN, L"Proxima Nova");;
	SetTextColor(hdc, RGB(255, 255, 255));
	SelectObject(hdc, font);
	while (true) {
		float piValue = 0;
		for (int x = screenRect.right+WAVEBALLSIZE+100; x > 0; x--) {
			TextOut(hdc, 100, 100, message, 10);
			long y = 1000;
			TextOut(hdc, 100, 100, message, 10);
			HBRUSH brush = CreateSolidBrush(RGB((127.5 + 127.5 * sin(piValue)) * red, (127.5 + 127.5 * cos(piValue))*green,(127.5 + 127.5 * sin(-piValue))*blue));
			TextOut(hdc, 100, 100, message, 10);
			SelectObject(hdc, brush);
			TextOut(hdc, 100, 100, message, 10);
			HPEN pen = CreatePen(PS_SOLID, 3, RGB((127.5 + 127.5 * sin(piValue)) * red, (127.5 + 127.5 * cos(piValue)) * green, (127.5 + 127.5 * sin(-piValue)) * blue));
			TextOut(hdc, 100, 100, message, 10);
			SelectObject(hdc, pen);
			TextOut(hdc, 100, 100, message, 10);
			Rectangle(hdc, x - 200, 0, x, 800);
			TextOut(hdc, 100, 100, message, 10);
			
			SetBkMode(hdc, TRANSPARENT);
			
			TextOut(hdc, 100, 100, message, 10);
			piValue += step;
			TextOut(hdc, 100, 100, message, 10);
			
			TextOut(hdc, 100, 100, message, 10);
			DeleteObject(brush);
			DeleteObject(pen);
			TextOut(hdc, 100, 100, message, 10);
		}
		for (int x = 0; x < screenRect.right + WAVEBALLSIZE + 100; x++) {
			TextOut(hdc, 100, 100, message, 10);
			long y = 1000;
			TextOut(hdc, 100, 100, message, 10);
			HBRUSH brush = CreateSolidBrush(RGB((127.5 + 127.5 * sin(piValue)) * red, (127.5 + 127.5 * cos(piValue)) * green, (127.5 + 127.5 * sin(-piValue)) * blue));
			TextOut(hdc, 100, 100, message, 10);
			SelectObject(hdc, brush);
			TextOut(hdc, 100, 100, message, 10);
			HPEN pen = CreatePen(PS_SOLID, 3, RGB((127.5 + 127.5 * sin(piValue)) * red, (127.5 + 127.5 * cos(piValue)) * green, (127.5 + 127.5 * sin(-piValue)) * blue));
			TextOut(hdc, 100, 100, message, 10);
			SelectObject(hdc, pen);
			TextOut(hdc, 100, 100, message, 10);
			Rectangle(hdc, x - 200, 0, x, 800);
			TextOut(hdc, 100, 100, message, 10);

			SetBkMode(hdc, TRANSPARENT);
			
			TextOut(hdc, 100, 100, message, 10);
			piValue -= step;
			TextOut(hdc, 100, 100, message, 10);

			TextOut(hdc, 100, 100, message, 10);
			DeleteObject(brush);
			DeleteObject(pen);
			TextOut(hdc, 100, 100, message, 10);
		}
		phase += M_PI;
	
	};
}

void ViewHandler::changeStatusToError() {
	wantedRed = 1;
	wantedGreen = 0;
	wantedBlue = 0;
	message = TEXT("error\0");
	std::tuple<double, double, double> COLOR{1,0,0};
	changeColorToSmooth(COLOR);
}
void ViewHandler::changeStatusToWaiting() {
	wantedRed = 1;
	wantedGreen = 0.2;
	wantedBlue = 1;
	message = TEXT("waiting\0");
	std::tuple<double, double, double> COLOR{ 1,1,0 };
	changeColorToSmooth(COLOR);
}
void ViewHandler::changeStatusToSuccess() {
	wantedRed = 0;
	wantedGreen = 1;
	wantedBlue = 0;
	message = TEXT("success\0");
	std::tuple<double, double, double> COLOR{ 0,1,0 };
	changeColorToSmooth(COLOR);
}

void ViewHandler::changeStatusToRainbow() {
	wantedRed = 1;
	wantedGreen = 1;
	wantedBlue = 1;
	message = TEXT("rainbow\0");
	std::tuple<double, double, double> COLOR{ 0,1,0 };
	changeColorToSmooth(COLOR);
}

ViewHandler::ViewHandler() {
	
}

void ViewHandler::changeColorToSmooth(std::tuple<double,double,double> wantedColor) {
	if (!COLORCHANGEFLAG) {
		colorThread = CreateThread(NULL, 0, changeColorProc, (LPVOID)&wantedColor, NULL, 0);
	}
	else {
		TerminateThread(colorThread, 0);
		colorThread = CreateThread(NULL, 0, changeColorProc, (LPVOID)&wantedColor, NULL, 0);
	}
}

ViewHandler::ViewHandler(HWND hWnd, COLORREF backgroundColor,
	int animtationType, LPWSTR startMessage) {
	parentHWND = hWnd;
	startView();
}

void ViewHandler::updateMessage(LPWSTR) {
	
}

void ViewHandler::startView() {
	viewThread = CreateThread(NULL, 0, gradientProc, parentHWND, NULL,0);
}

DWORD WINAPI ViewHandler::renderView(LPVOID t) {
	HDC hdc = GetDC(parentHWND);
	RECT screenRect;
	GetClientRect(parentHWND, &screenRect);
	FillRect(hdc, &screenRect, CreateSolidBrush(backgroundColor));
	long topValue = screenRect.bottom * ANIMATIONPERCENT;
	long diff = (screenRect.bottom-topValue)/2;
	double step = M_PI / screenRect.right;
	while (true) {
		float piValue = 0;
		for (int x = screenRect.right; x > 0; x--) {
			long y = (cos(piValue) * diff) + (topValue+diff);
			Ellipse(hdc, x - WAVEBALLSIZE, y, x, y - WAVEBALLSIZE);
			piValue += step;
		}
	};
}