#include <windows.h>
#include <iostream>
#include <ctime>
#include <locale>
#include <codecvt>
#include <string>
#include "Source.h"
#include "Logger.h"

using namespace std;

HWND hWndg;


Logger loggerGlobal;

int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

class Comport
{
private:
	wstring scomport = L"COM0";
	HANDLE hSerial;
	DCB dcbSerialParams = { 0 };


	
public:
	Comport(int numport)
	{
		HANDLE hSerial;
		scomport[3] += numport;
	}
	
	wstring  returner() { return scomport;}

	void open()
	{
		hSerial = CreateFile(scomport.c_str(),
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
				// MessageBox(hWndg, L"serial port does not exist", L"Caption", MB_OK);
				loggerGlobal.addMessage(L"serial port does not exist");
			}
			loggerGlobal.addMessage(L"some other error occurred. Inform user.");
			// MessageBox(hWndg, L"some other error occurred. Inform user.", L"Caption", MB_OK);
		}

		GetSerialParams();
		
		dcbSerialParams.BaudRate = CBR_19200;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;

		SetSerialParams();

		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		if (!SetCommTimeouts(hSerial, &timeouts)) {
			//error occureed. Inform user
			// MessageBox(hWndg, L"error occureed. Inform user", L"Caption", MB_OK);
			loggerGlobal.addMessage(L"error occureed. Inform user");
		}
	}
	void GetSerialParams()
	{
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams)) {
			//error getting state
			// MessageBox(hWndg, L"error getting state", L"Caption", MB_OK);
			loggerGlobal.addMessage(L"error getting state");
		}
	}
	void SetSerialParams()
	{
		if (!SetCommState(hSerial, &dcbSerialParams)) {
			//error setting serial port state
			// MessageBox(hWndg, L"error setting serial port state", L"Caption", MB_OK);
			loggerGlobal.addMessage(L"error setting serial port state");
		}
		
	}
	void close()
	{
		CloseHandle(hSerial);
		hSerial = NULL;
	}
	void read()
	{
		DWORD dwBytesWritten;
		char szBuf[100];
		
		dwBytesWritten = 0;
		ReadFile(hSerial,&szBuf,sizeof(szBuf),&dwBytesWritten, NULL);
		// MessageBox(hWndg, convertCharArrayToLPCWSTR(szBuf), L"Caption", MB_OK);
		loggerGlobal.addMessage(convertCharArrayToLPCWSTR(szBuf));
	}
	bool writetoconnect()
	{

		char a[10];
		a[0] = 'Z';
		a[1] = 'n';
		a[2] = 'd';
		a[3] = 'r';
		a[4] = 'e';
		a[5] = 'y';
		a[6] = ' ';
		a[7] = 's';
		a[8] = 'o';
		a[9] = 's';
		DWORD dwSize = sizeof(a);   // ðàçìåð ýòîé ñòðîêè
		DWORD dwBytesWritten;
		bool retVal = WriteFile(hSerial, a, 10, &dwBytesWritten, NULL);
		//CloseHandle(hSerial); //close the handle
		return retVal;
	}
};

class registration
{
private:
	int id;
	Comport *Read, *Write;
	bool success;
public:
	registration(bool major, int comtowrite, int comtoread)
	{
		char szBuf[100];
		
		Write = new Comport(comtowrite);
		Read = new Comport(comtoread);
		Write->open();
		Read->open();
	}

	void test()
	{
		if (Write->writetoconnect() == false)
		{
			MessageBox(hWndg, L"Проверьте запущены ли приложения на остальных компьютерах", L"Caption", MB_OK);
			loggerGlobal.addMessage(L"Проверьте запущены ли приложения на остальных компьютерах");
		}
	}
	void read()
	{
		char ne[100];
		Read->read();
	}
};

registration *a1;

void WorkWithCom(HWND hWnd, int com1, int com2, Logger givenloggerGlobal)
{
	loggerGlobal = givenloggerGlobal;
	hWndg = hWnd;
	Comport *commer;

	a1 = new registration(true, com1, com2);
}

void trytowrrite(Logger givenloggerGlobal)
{
	loggerGlobal = givenloggerGlobal;
	a1->test();
}

void trytoread(Logger givenloggerGlobal)
{
	loggerGlobal = givenloggerGlobal;
	a1->read();
}
