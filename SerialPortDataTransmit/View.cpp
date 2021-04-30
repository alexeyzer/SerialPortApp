

#include "View.h"
#include <string>
#include <vector>

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
size_t messageLength = 0;

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

#define GRADIENT_OBJ_SIZE 200.0

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
	message = TEXT("Select a comport\0");
	messageLength = 18;
	std::tuple<double, double, double> COLOR{ 0,1,0 };
	changeColorToSmooth(COLOR);
}


std::vector<std::string> listArray(256);
size_t listArrayPos;
size_t activeListObject=2;
int listEnd=0;
bool chooseOneList = false;

void ViewHandler::changeActiveListItemTo(size_t pos)
{
	listEnd = 0;
	activeListObject = pos;
}

//
//size_t getItemByMousePos() {
//
//}

void ViewHandler::sendMousePos(int posX, int posY)
{
	if (chooseOneList == true) {
		if (posX > 300 && posX < 500) {
			size_t pos = (posY - 100) / 60;
			if (pos >= listArrayPos) {
				pos = listArrayPos-1;
			}
			
			changeActiveListItemTo(pos+1);
		}
	}
}

void ViewHandler::createChooseOneList()
{
	chooseOneList = true;
	listArrayPos = 0;
}

void ViewHandler::addValueToList(const char* value)
{
	listArray[listArrayPos] = std::string(value);
	listArrayPos++;
}

size_t ViewHandler::getActiveListItemID()
{
	return activeListObject;
}

const char* ViewHandler::getActiveListItemText()
{
	return listArray[activeListObject-1].c_str();
}



LPCWSTR s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	LPCWSTR r = buf;
	
	return r;
}

HFONT ListFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, 0, 
	OUT_OUTLINE_PRECIS, CLIP_STROKE_PRECIS, 
	3, FF_ROMAN, L"Proxima Nova");;

void DrawList(HDC hdc) {
	SelectObject(hdc, ListFont);
	SetTextColor(hdc, 0x00FFFFFF);
	SetBkMode(hdc, TRANSPARENT);
	Rectangle(hdc, 300, 100+ 60 * (activeListObject-1), 500, 100 + 60 * (activeListObject));
	
	for (int i = 0; i < listArray.size(); i++) {
		LPCWSTR temp = s2ws(listArray[i]);
		
		TextOut(hdc, 360,100 + 60 * i, temp , listArray[i].length());
		delete temp;
	}
}

int ViewHandler::getCurrentView()
{
	return currentView;
}

void ViewHandler::changeCurrentView(int newView)
{
	currentView = newView;
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




DWORD WINAPI continueDrawLoop(LPVOID t) {
	HWND parentHWND = static_cast<HWND>(t);
	HDC hdc = GetDC(parentHWND);


	RECT screenRect;
	GetClientRect(parentHWND, &screenRect);

	HDC bufferDC = CreateCompatibleDC(hdc);
	HBITMAP bufferFrame = CreateCompatibleBitmap(hdc,
		screenRect.right - screenRect.left,
		screenRect.bottom - screenRect.top);
	SelectObject(bufferDC, bufferFrame);


	// FillRect(hdc, &screenRect, CreateSolidBrush(RGB(120,120,0)));
	// long topValue = screenRect.bottom * ANIMATIONPERCENT;
	// long diff = (screenRect.bottom - topValue) / 2;
	double step = (2 * M_PI) / (screenRect.right + GRADIENT_OBJ_SIZE);
	HFONT font = CreateFont(50, 0, 0, 0,0, 0, 0, 0, 0, OUT_OUTLINE_PRECIS, CLIP_STROKE_PRECIS, 3, FF_ROMAN, L"Proxima Nova");;
	SetBkMode(bufferDC, TRANSPARENT);
	SetTextColor(bufferDC, RGB(255, 255, 255));
	SelectObject(bufferDC, font);
	while (true) {
		float piValue = 0;
		for (int x = screenRect.right + GRADIENT_OBJ_SIZE; x > 0; x--) {

			HDC bufferDCreDraw = CreateCompatibleDC(hdc);
			HBITMAP bufferFramereDraw = CreateCompatibleBitmap(hdc,
				screenRect.right - screenRect.left,
				screenRect.bottom - screenRect.top);
			SelectObject(bufferDCreDraw, bufferFramereDraw);

			HBRUSH brush = CreateSolidBrush(RGB(
				(127.5 + 127.5 * sin(piValue)) * green,
				(127.5 + 127.5 * cos(piValue)) * blue,
				(127.5 + 127.5 * sin(-piValue)) * red));

			SelectObject(bufferDC, brush);

			HPEN pen = CreatePen(PS_SOLID, 3, RGB(
				(127.5 + 127.5 * sin(piValue)) * green,
				(127.5 + 127.5 * cos(piValue)) * blue,
				(127.5 + 127.5 * sin(-piValue)) * red));

			SelectObject(bufferDC, pen);

			Rectangle(bufferDC, x - 200, 0, x, 800);

			piValue += step;

			DeleteObject(brush);
			DeleteObject(pen);

			TextOut(bufferDC, 50, 50, message, messageLength);

			

			if (chooseOneList) {
				HBRUSH listBrush = CreateSolidBrush(RGB(
					(127.5 + 127.5 * sin(2 * piValue)) * red,
					(127.5 + 127.5 * cos(-piValue)) * green,
					(127.5 + 127.5 * sin(piValue)) * blue));
				SelectObject(bufferDCreDraw, listBrush);

				HPEN listPen = CreatePen(PS_SOLID, 3, RGB(
					(127.5 + 127.5 * sin(2 * piValue)) * red,
					(127.5 + 127.5 * cos(-piValue)) * green,
					(127.5 + 127.5 * sin(piValue)) * blue));

				SelectObject(bufferDCreDraw, listPen);

				DrawList(bufferDCreDraw);

				DeleteObject(listBrush);
				DeleteObject(listPen);
			}
			HDC hdcMerge = CreateCompatibleDC(hdc);

			HBITMAP mergeFrame = CreateCompatibleBitmap(hdc,
				screenRect.right - screenRect.left,
				screenRect.bottom - screenRect.top);
			
			SelectObject(hdcMerge, mergeFrame);
			
			BitBlt(hdcMerge, 0, 0, screenRect.right - screenRect.left,
				screenRect.bottom - screenRect.top,
				bufferDC, 0, 0, SRCCOPY);

			BitBlt(hdcMerge, 0, 0, screenRect.right - screenRect.left,
				screenRect.bottom - screenRect.top,
				bufferDCreDraw, 0, 0, SRCPAINT);

			BitBlt(hdc, 0, 0, screenRect.right - screenRect.left,
				screenRect.bottom - screenRect.top,
				hdcMerge, 0, 0, SRCCOPY);

			DeleteObject(hdcMerge);
			DeleteObject(mergeFrame);

			DeleteObject(bufferDCreDraw);
			DeleteObject(bufferFramereDraw);


			Sleep(5);
		}


	};
}



void ViewHandler::startView() {
	viewThread = CreateThread(NULL, 0, continueDrawLoop, parentHWND, NULL, 0);
	
}