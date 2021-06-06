

#include "View.h"
#include <string>
#include <vector>
#include "SerialPortDataTransmit.h"

#define M_PI 3.1415926535897931084
#define GRADIENT_CHANGE_STEPS 1000
#define GRADIENT_OBJ_SIZE 300


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


wchar_t* nickname= new wchar_t[100];
size_t nicknameLength = 0;
bool showNickname = false;


void ViewHandler::updateNickname(char* name,size_t len)
{
	nicknameLength = len;
	// convertCharArrayToLPCWSTR(name);
	
	MultiByteToWideChar(CP_ACP, 0, name, -1, nickname,len);
	nickname[len] = '\0';
	int a = 0;
}

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

bool pcSelector = false;
size_t pcAmount = 3;
size_t selectedPC = 1;


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
	if (pcSelector == true) {
		if (posX > 66 && posY < 400) {
			selectedPC = 1;
		}
		if (posX > 300 && posY < 400) {
			selectedPC = 2;
		}
		if (posX > 533 && posY < 400) {
			selectedPC = 3;
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

void ShowImage(HDC hdc,int posX, int posY, HBITMAP Image, int sizeX, int sizeY) {
	
	// LoadSerialPortImage();
	HGDIOBJ oldBitmap;
	HDC buffer = CreateCompatibleDC(hdc);
	BITMAP bitmap;
	
	oldBitmap = SelectObject(buffer, Image);

	GetObject(Image, sizeof(bitmap), &bitmap);
	BitBlt(hdc, posX, posY, sizeX, sizeY, buffer, 0, 0, SRCCOPY);

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

HBITMAP BackgroundImage;

void LoadBackgroundImage() {
	BackgroundImage = (HBITMAP)LoadImageW(NULL, L"Background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);


	if (SerialPortImage == NULL) {
		DWORD error = GetLastError();
		int a = 0;
	}
}

size_t ViewHandler::getSelectedPC() {
	return selectedPC;
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
		// StartTransition();
	}
	break;
	case CHOOSECOMPORT_SENDING_VIEW:
	{
		updateMessage(TEXT("Choose a sending comport"), 26);
		// StartTransition();
	}
	break;
	case CONNECT_USING_SELECTEDCOMPORTS_VIEW:
	{
		showNickname = false;
		comportSending = convertCharArrayToLPCWSTR(param1);
		comportReceiving = convertCharArrayToLPCWSTR(param2);
		updateMessage(TEXT("Selected comports"), 19);
		deleteChooseOneList();
		// StartTransition();
		DrawSerialPortImages = true;
		StrokeWidth = 400;
	}
	break;
	case CONNECTING_VIEW:
	{
		DrawSerialPortImages = false;
		deleteChooseOneList();
		updateMessage(TEXT("Connecting"), 12);
		// StartTransition();
	}
	break;
	case CONNECT_SUCCESS_VIEW:
	{
		registration* a = returner();
		for (int i = 0; i < a->countofcomputers; i++) {
			nickname = convertCharArrayToLPCWSTR(a->user[i].name);
			addPC(nickname, a->user[i].namelen, a->user[i].id);
		}
		changeColorToSmooth(0, 0, 1);
		updateMessage(TEXT("Connected"), 10);
		pcSelector = true;
		DrawPCPortImages = true;
	}
	break;
	case CONNECT_SUCCESS_VIEW_MAIN:
	{
		nickname = convertCharArrayToLPCWSTR(param1);
		addPC(nickname, 10, 1);
		changeColorToSmooth(0, 0, 1);
		updateMessage(TEXT("Connected as main"), 18);
		pcSelector = true;
		DrawPCPortImages = true;
	}
	break;
	case INPUTNICKNAME_VIEW:
	{
		showNickname = true;
		changeColorToSmooth(0, 1, 1);
		updateMessage(TEXT("Введите имя компьютера"), 23);
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




std::vector<std::tuple<LPCWSTR, uint16_t, uint16_t>> PCTable(10);
size_t PCAmount = 0;

void ViewHandler::addPC(LPCWSTR nickname,uint16_t length, uint16_t id)
{
	std::tuple<LPCWSTR, uint16_t, uint16_t> newPC {nickname,length, id};
	PCTable[PCAmount] = newPC;
	PCAmount++;
}

void showPCS(HDC bufferDCreDraw) {
	switch (selectedPC) {
	case 1:
	{
		if(PCAmount>0)	Rectangle(bufferDCreDraw, 56, 190, 276, 410);
	}
	break;
	case 2:
	{
		if (PCAmount > 1) Rectangle(bufferDCreDraw, 290, 190, 510, 410);
	}
	break;
	case 3:
	{
		if (PCAmount > 2) Rectangle(bufferDCreDraw, 523, 190, 743, 410);
	}
	break;

	}
	for (int i = 0; i < PCAmount; i++) {
		ShowImage(bufferDCreDraw, 66+(i*234), 200, PcImage, 200, 200);
		RECT rect;
		rect.top = 420;
		rect.left = 66 + (i * 234);
		rect.right = rect.left + 200;
		rect.bottom = rect.top + 100;
		DrawTextW(bufferDCreDraw, std::get<0>(PCTable[i]), 6, &rect , DT_CENTER);
		// TextOut(bufferDCreDraw, 66 + (i * 234), 420, std::get<0>(PCTable[i]),6);
	}
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
	LoadBackgroundImage();
	while (true) {
		float piValue = 0;
		for (int x = screenRect.right + GRADIENT_OBJ_SIZE; x > 0; x-=2) {
			
			HDC bufferDCreDraw = CreateCompatibleDC(hdc);
			HBITMAP bufferFramereDraw = CreateCompatibleBitmap(hdc,
				screenRect.right - screenRect.left,
				screenRect.bottom - screenRect.top);
			SelectObject(bufferDCreDraw, bufferFramereDraw);
			SetBkMode(bufferDCreDraw, TRANSPARENT);
			SetTextColor(bufferDCreDraw, RGB(255, 255, 255));
			SelectObject(bufferDCreDraw, font);
			ShowImage(bufferDCreDraw, 0, 0, BackgroundImage, 800, 800);
			SetTextColor(bufferDCreDraw, RGB(0, 0, 0));
			HBRUSH brush = CreateSolidBrush(RGB(
				(127.5 + 127.5 * sin(piValue)) * green,
				(127.5 + 127.5 * cos(piValue)) * blue,
				(127.5 + 127.5 * sin(-piValue)) * red));

			SelectObject(bufferDC, brush);

			if (DrawSerialPortImages) {
				ShowImage(bufferDCreDraw, 100, 200,SerialPortImage , 200, 200);
				ShowImage(bufferDCreDraw, 500, 200,SerialPortImage, 200 ,200);
				TextOut(bufferDCreDraw, 150, 400, comportSending, 5);
				TextOut(bufferDCreDraw, 550, 400, comportReceiving, 5);
			}
			if (showNickname) {
				TextOut(bufferDCreDraw, 400-(nicknameLength*13), 300, nickname, nicknameLength);
			}
			if (DrawPCPortImages) {
				showPCS(bufferDCreDraw);
			}
			
			HPEN pen = CreatePen(PS_SOLID, 3, RGB(
				(127.5 + 127.5 * sin(piValue)) * green,
				(127.5 + 127.5 * cos(piValue)) * blue,
				(127.5 + 127.5 * sin(-piValue)) * red));

			SelectObject(bufferDC, pen);

			Ellipse(bufferDC, x - GRADIENT_OBJ_SIZE, 0, x,800);
			piValue += step*2;
			

			DeleteObject(brush);
			DeleteObject(pen);

			TextOut(bufferDCreDraw, 50, 50, message, messageLength);

			

			if (chooseOneList) {
				HBRUSH listBrush = CreateSolidBrush(RGB(
					255,255,255));
				SelectObject(bufferDCreDraw, listBrush);

				HPEN listPen = CreatePen(PS_SOLID, 3, RGB(
					0,0,0));

				SetTextColor(bufferDCreDraw, RGB(0, 0, 0));
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
				hdcMerge, 0, 0, NOTSRCCOPY);

			DeleteObject(hdcMerge);
			DeleteObject(mergeFrame);

			DeleteObject(bufferDCreDraw);
			DeleteObject(bufferFramereDraw);

			if (!transition) Sleep(1);
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