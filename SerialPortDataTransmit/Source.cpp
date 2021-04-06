#include <windows.h>
#include <iostream>
using namespace std;

HWND hWndg;

class Comport
{
private:
	wstring scomport = L"COM0";
	HANDLE hSerial;

	
public:
	Comport(int numport)
	{
		HANDLE hSerial;
		scomport[3] += numport;
	}
	
	wstring  returner()
	{
		return scomport;
	}

	HANDLE open()
	{
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
				MessageBox(hWndg, L"serial port does not exist", L"Caption", MB_OK);
			}
			MessageBox(hWndg, L"some other error occurred. Inform user.", L"Caption", MB_OK);
		}

		DCB dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams)) {
			//error getting state
			MessageBox(hWndg, L"error getting state", L"Caption", MB_OK);
		}
		dcbSerialParams.BaudRate = CBR_19200;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		if (!SetCommState(hSerial, &dcbSerialParams)) {
			//error setting serial port state
			MessageBox(hWndg, L"error setting serial port state", L"Caption", MB_OK);
		}

		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		if (!SetCommTimeouts(hSerial, &timeouts)) {
			//error occureed. Inform user
			MessageBox(hWndg, L"error occureed. Inform user", L"Caption", MB_OK);
		}
	}
	void close()
	{
		CloseHandle(hSerial);
		hSerial = NULL;
	}
	wstring read()
	{

	}
	bool writetoconnect(wstring data)
	{
		DWORD dwSize = sizeof(data);   // ðàçìåð ýòîé ñòðîêè
		DWORD dwBytesWritten;
		bool retVal = WriteFile(hWndg, &data, 1, &dwBytesWritten, NULL);
		CloseHandle(hWndg); //close the handle
		return retVal;
	}
};

class registration
{
private:
	int id;
	Comport *Read, *Write;
public:
	registration(bool major, int comtowrite, int comtoread)
	{
		if (major == true)
		{
			id = 1;
			Write = new Comport(comtowrite);
			Read = new Comport(comtoread);
			Write->writetoconnect(L"c1");
			Read->read();
		}
		else
		{

		}
	}
};

void WorkWithCom(HWND hWnd)
{
	hWndg = hWnd;
	Comport *commer;

	commer = new Comport(3);
	wstring a = commer->returner();
	MessageBox(hWnd, a.c_str(), L"Caption", MB_OKCANCEL);
	
}