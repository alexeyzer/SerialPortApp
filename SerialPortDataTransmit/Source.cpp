#include <windows.h>
#include <iostream>
#include <ctime>
using namespace std;

HWND hWndg;

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

		GetSerialParams();
		
		dcbSerialParams.BaudRate = CBR_19200;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		
		SetSerialParams()

		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		
	}
	void GetSerialParams()
	{
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams)) {
			//error getting state
			MessageBox(hWndg, L"error getting state", L"Caption", MB_OK);
		}
	}
	void SetSerialParams()
	{
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
	void read(wstring &szBuf)
	{
		bool a = true;
		DWORD dwBytesWritten;
		
		dwBytesWritten = 0;
		time_t t = std::time(0);   // get time now
   		tm* now = std::localtime(&t);
    		memset(szBuf,0,sizeof(szBuf));
    		while (a == true || dwBytesWritten > 0)
		{
			ReadFile( hPort,szBuf,sizeof(szBuf),&dwBytesRead, NULL);
			if (dwBytesWritten == 0)
			{
				time_t t1 = std::time(0);   // get time now
   				tm* after = std::localtime(&t);
				if (difftime(now,after) > 120)
					a = false;
			}
			else
				a = false;
		}
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
	bool success;
public:
	registration(bool major, int comtowrite, int comtoread)
	{
		wstring szBuf[100];
		if (major == true)
		{
			id = 1;
			Write = new Comport(comtowrite);
			Read = new Comport(comtoread);
			if (Write->writetoconnect(L"c1") = false)
				MessageBox(hWndg, L"Проверьте запущены ли приложения на остальных компьютерах", L"Caption", MB_OK);
			else
			{
				Read->read(&szBuff);
				if ( szBuf[0] = 'C' && szBuff[1] == "2")
					success = true;
				else
					success = false;
			}
		}
		else
		{
			Write = new Comport(comtowrite);
			Read = new Comport(comtoread);
			Read->read(&szBuff);
			if (szBuf[0] = 'C' && (szbBuf[1] == '1' || (szbBuf[1] == '2')
			{
				int num = (int)szbBuf[1] - (int)'0'
				num += 1;
				id = num;
				szBuf[1] = '0' + id;
				if (Write->writetoconnect(szBuf) = false)
					MessageBox(hWndg, L"Проверьте запущены ли приложения на остальных компьютерах", L"Caption", MB_OK);
				else
					success = true;
			}
				
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
