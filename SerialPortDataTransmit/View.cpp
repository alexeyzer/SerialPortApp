

#include "View.h"
#include <string>
#include <vector>

#define M_PI 3.1415926535897931084
#define GRADIENT_CHANGE_STEPS 1000
#define GRADIENT_OBJ_SIZE 100


double red = 1;
double green = 0;
double blue = 0;

bool COLORCHANGEFLAG = false;
HANDLE colorThread;

double wantedRed = 0;
double wantedGreen = 0;
double wantedBlue = 0;

int StrokeWidth = 200;


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


void ViewHandler::changeStatusToError() {
	changeColorToSmooth(1,0,0);
}
void ViewHandler::changeStatusToWaiting() {
	changeColorToSmooth(1, 0.2, 1);
}
void ViewHandler::changeStatusToSuccess() {
	changeColorToSmooth(0, 1, 0);
}

void ViewHandler::changeStatusToRainbow() {
	changeColorToSmooth(1, 1, 1);
}




std::vector<std::string> listArray(256);
size_t listArrayPos;
size_t activeListObject=2;
int listEnd=0;
bool chooseOneList = false;
int xOffset = 100;

void ViewHandler::changeActiveListItemTo(size_t pos)
{
	listEnd = 0;
	activeListObject = pos;
}

//
//size_t getItemByMousePos() {
//
//}

bool transition=false;


DWORD WINAPI transitionProc(LPVOID t) {
	transition = true;
	Sleep(1000);
	transition = false;
	return 0;
}


void StartTransition() {
	if (!transition) {
		CreateThread(NULL, NULL, transitionProc, 0, 0, 0);
	}
}

void ViewHandler::sendMousePos(int posX, int posY)
{
	if (chooseOneList == true) {
		if (posX > 300 && posX < 300+StrokeWidth) {
			size_t pos = (posY - xOffset) / 60;
			if (pos >= listArrayPos) {
				pos = listArrayPos-1;
			}
			
			changeActiveListItemTo(pos+1);
		}
	}
}

void ViewHandler::createChooseOneList(int posX)
{
	xOffset = posX;
	chooseOneList = true;
	listArrayPos = 0;
}

void ViewHandler::deleteChooseOneList()
{
	chooseOneList = false;
	listArrayPos = 0;
	listArray = std::vector<std::string>(10);
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

void ViewHandler::DeleteItemFromList(size_t itemID) {
	for (int i = itemID-1; i < listArrayPos; i++) {
		std::swap(listArray[i], listArray[i + 1]);
	}
	listArrayPos--;
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
	Rectangle(hdc, 300, xOffset+ 60 * (activeListObject-1), 300+StrokeWidth, xOffset + 60 * (activeListObject));
	
	for (int i = 0; i < listArrayPos; i++) {
		LPCWSTR temp = s2ws(listArray[i]);
		
		TextOut(hdc, 360, xOffset + 60 * i, temp , listArray[i].length());
		delete temp;
	}
}

int ViewHandler::getCurrentView()
{
	return currentView;
}

HBITMAP SerialPortImage;
bool DrawSerialPortImages = false;
bool DrawPCPortImages = false;


void LoadSerialPortImage() {
	SerialPortImage = (HBITMAP)LoadImageW(NULL, L"SerialPortIcon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);


	if (SerialPortImage == NULL) {
		DWORD error = GetLastError();
		int a = 0;
	}
}


void ShowImage(HDC hdc,int posX, int posY, HBITMAP Image) {
	
	// LoadSerialPortImage();
	HGDIOBJ oldBitmap;
	HDC buffer = CreateCompatibleDC(hdc);
	BITMAP bitmap;
	
	oldBitmap = SelectObject(buffer, Image);

	GetObject(Image, sizeof(bitmap), &bitmap);
	BitBlt(hdc, posX, posY, 200, 200, buffer, 0, 0, SRCCOPY);

	SelectObject(buffer, oldBitmap);
	DeleteObject(buffer);
}

HBITMAP PcImage;

void LoadPcImage() {
	PcImage = (HBITMAP)LoadImageW(NULL, L"PCIcon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);


	if (SerialPortImage == NULL) {
		DWORD error = GetLastError();
		int a = 0;
	}
}




wchar_t *comportSending;
wchar_t *comportReceiving;

void ViewHandler::changeCurrentView(int newView,const char* param1,const char* param2)
{
	switch (newView) {
	case CHOOSECOMPORT_RECIEVING_VIEW:
	{
		changeColorToSmooth(1, 1, 1);
		updateMessage(TEXT("Choose a receiving comport"), 28);
		StartTransition();
	}
	break;
	case CHOOSECOMPORT_SENDING_VIEW:
	{
		updateMessage(TEXT("Choose a sending comport"), 26);
		StartTransition();
	}
	break;
	case CONNECT_USING_SELECTEDCOMPORTS_VIEW:
	{
		comportSending = convertCharArrayToLPCWSTR(param1);
		comportReceiving = convertCharArrayToLPCWSTR(param2);
		updateMessage(TEXT("Selected comports"), 19);
		deleteChooseOneList();
		StartTransition();
		DrawSerialPortImages = true;
		StrokeWidth = 400;
	}
	break;
	case CONNECTING_VIEW:
	{
		DrawSerialPortImages = false;
		deleteChooseOneList();
		updateMessage(TEXT("Connecting"), 12);
		StartTransition();
	}
	break;
	case CONNECT_SUCCESS_VIEW:
	{
		changeColorToSmooth(0, 0, 1);
		updateMessage(TEXT("Connected"), 10);
		DrawPCPortImages = true;
	}
	break;
	case CONNECT_FAILURE_VIEW:
	{

	}
	break;
	}
	currentView = newView;
}

ViewHandler::ViewHandler() {
	
}

void ViewHandler::changeColorToSmooth(float RValue, float GValue, float BValue) {
	
	wantedRed = RValue;
	wantedGreen = GValue;
	wantedBlue = BValue;

	if (!COLORCHANGEFLAG) {
		colorThread = CreateThread(NULL, 0, changeColorProc, NULL, NULL, 0);
	}
	else {
		TerminateThread(colorThread, 0);
		colorThread = CreateThread(NULL, 0, changeColorProc, NULL, NULL, 0);
	}
}

ViewHandler::ViewHandler(HWND hWnd, COLORREF backgroundColor,
	int animtationType, LPWSTR startMessage) {
	parentHWND = hWnd;
	startView();
}

void ViewHandler::updateMessage(const wchar_t* text, size_t length) {
	message = text;
	messageLength = length;
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
	LoadSerialPortImage();
	LoadPcImage();
	while (true) {
		float piValue = 0;
		for (int x = screenRect.right + GRADIENT_OBJ_SIZE; x > 0; x--) {
			
			HDC bufferDCreDraw = CreateCompatibleDC(hdc);
			HBITMAP bufferFramereDraw = CreateCompatibleBitmap(hdc,
				screenRect.right - screenRect.left,
				screenRect.bottom - screenRect.top);
			SelectObject(bufferDCreDraw, bufferFramereDraw);
			SetBkMode(bufferDCreDraw, TRANSPARENT);
			SetTextColor(bufferDCreDraw, RGB(255, 255, 255));
			SelectObject(bufferDCreDraw, font);

			HBRUSH brush = CreateSolidBrush(RGB(
				(127.5 + 127.5 * sin(piValue)) * green,
				(127.5 + 127.5 * cos(piValue)) * blue,
				(127.5 + 127.5 * sin(-piValue)) * red));

			SelectObject(bufferDC, brush);

			if (DrawSerialPortImages) {
				ShowImage(bufferDCreDraw, 100, 200,SerialPortImage);
				ShowImage(bufferDCreDraw, 500, 200,SerialPortImage);
				TextOut(bufferDCreDraw, 150, 400, comportSending, 5);
				TextOut(bufferDCreDraw, 550, 400, comportReceiving, 5);
			}

			if (DrawPCPortImages) {
				ShowImage(bufferDCreDraw, 66, 200, PcImage);
				ShowImage(bufferDCreDraw, 300, 200, PcImage);
				ShowImage(bufferDCreDraw, 533, 200, PcImage);
			}
			
			HPEN pen = CreatePen(PS_SOLID, 3, RGB(
				(127.5 + 127.5 * sin(piValue)) * green,
				(127.5 + 127.5 * cos(piValue)) * blue,
				(127.5 + 127.5 * sin(-piValue)) * red));

			SelectObject(bufferDC, pen);

			Ellipse(bufferDC, x - GRADIENT_OBJ_SIZE, 0, x,800);
			piValue += step;
			

			DeleteObject(brush);
			DeleteObject(pen);

			TextOut(bufferDCreDraw, 50, 50, message, messageLength);

			

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

			if (!transition) {
				BitBlt(hdcMerge, 0, 0, screenRect.right - screenRect.left,
					screenRect.bottom - screenRect.top,
					bufferDCreDraw, 0, 0, SRCPAINT);
			}

			BitBlt(hdc, 0, 0, screenRect.right - screenRect.left,
				screenRect.bottom - screenRect.top,
				hdcMerge, 0, 0, SRCCOPY);

			DeleteObject(hdcMerge);
			DeleteObject(mergeFrame);

			DeleteObject(bufferDCreDraw);
			DeleteObject(bufferFramereDraw);

			if (!transition) Sleep(2);
			if (transition) {
				Sleep(1);
				x -= step * 2;
			}
		}


	};
}



void ViewHandler::startView() {
	viewThread = CreateThread(NULL, 0, continueDrawLoop, parentHWND, NULL, 0);
	
}