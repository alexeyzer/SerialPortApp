#include <windows.h>
#include <iostream>
using namespace std;

void WorkWithCom(HWND hWnd)
{
	HANDLE hSerial;

	MessageBox(hWnd, L"lol this is kyrsach", L"Caption", MB_OKCANCEL);
	hSerial = CreateFile(L"COM1",
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND) 
		{
			MessageBox(hWnd, L"Hello", L"Caption", MB_OKCANCEL);
		}
		MessageBox(hWnd, L"some other error occurred. Inform user.", L"Caption", MB_OKCANCEL);
	}
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		//error getting state
	}
	dcbSerialParams.BaudRate = CBR_19200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams)) {
		//error setting serial port state
	}
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (!SetCommTimeouts(hSerial, &timeouts)) {
		//
	}
	char data[] = "Hello from C++";  // строка для передачи
	DWORD dwSize = sizeof(data);   // размер этой строки
	DWORD dwBytesWritten;
	BOOL iRet = WriteFile(hSerial, data, dwSize, &dwBytesWritten, NULL);
}