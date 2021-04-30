#include <windows.h>
#include <iostream>
#include <ctime>
#include <locale>
#include <codecvt>
#include <string>
#include <bitset>
#include <fstream>
#include <iostream>
#include <ostream>
#include "SerialPortDataTransmit.h"
using namespace std;

registration *a1;

HWND hWndg;

HANDLE life = NULL;

int counterlife = 0;

bool auntification = false;

bitset<7> a;

char* buffer = NULL;

int bytestosend;

int computerscount = 0;

int result = 0;

bool maincomputer = false;


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
	wstring  returner() { return scomport; }
	void open()
	{
		hSerial = CreateFile(scomport.c_str(),
			(DWORD)(GENERIC_READ | (DWORD)GENERIC_WRITE),
			(DWORD)0,
			NULL,
			(DWORD)OPEN_EXISTING,
			(DWORD)FILE_FLAG_OVERLAPPED,
			NULL);
		PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
		if (hSerial == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
			{
				MessageBox(hWndg, L"Проблема с последовательным портом.", L"Caption", MB_OK);
				a1->close();
				PostQuitMessage(0);
			}
			MessageBox(hWndg, L"Проблема с последовательным портом.", L"Caption", MB_OK);
			a1->close();
			PostQuitMessage(0);
		}

		GetSerialParams();

		dcbSerialParams.BaudRate = CBR_57600;
		dcbSerialParams.fOutxCtsFlow = FALSE;                   // выключаем режим слежения за сигналом CTS
		dcbSerialParams.fDsrSensitivity = FALSE;                // отключаем восприимчивость драйвера к состоянию линии DSR
		dcbSerialParams.ByteSize = 8;                           // задаём 8 бит в байте
		dcbSerialParams.Parity = 1;                             // включаем проверку чётности
		dcbSerialParams.StopBits = 0;                           // задаём один стоп-бит


		SetSerialParams();
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		if (!SetCommTimeouts(hSerial, &timeouts)) {
			MessageBox(hWndg, L"Проблема с таймаутом последовательного порта.", L"Caption", MB_OK);
			a1->close();
			PostQuitMessage(0);
		}
	}
	void GetSerialParams()
	{
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams)) {
			MessageBox(hWndg, L"Проблема с получением  состояния последовательного порта.", L"Caption", MB_OK);
			a1->close();
			PostQuitMessage(0);
		}
	}
	void SetSerialParams()
	{
		if (!SetCommState(hSerial, &dcbSerialParams)) {
			MessageBox(hWndg, L"Проблема с заданием  состояния последовательного порта.", L"Caption", MB_OK);
			a1->close();
			PostQuitMessage(0);
		}

	}
	void close()
	{
		CloseHandle(hSerial);
		hSerial = NULL;
	}

	HANDLE reth() { return hSerial; }

};

Comport* Read, * Write;

registration::registration(bool major, int comtowrite, int comtoread)
{
	Write = new Comport(comtowrite);
	Read = new Comport(comtoread);
	Write->open();
	Read->open();
	CreateThread(NULL, 0, read, NULL, 0, NULL);
	status = true;
}
void registration::close()
{
	Write->close();
	Read->close();
}
void registration::setid(int a)
{
	char b;
	id = a;
}
int registration::getid()
{
	return id;
}

int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());
	ws = wsTmp;
	return 0;
}

HANDLE  readthread;

void setid(int id);

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

DWORD WINAPI read(LPVOID t);

void WorkWithCom(HWND hWnd, int com1, int com2, bool major)
{
	hWndg = hWnd;
	Comport *commer;
	a1 = new registration(major, com1, com2);
}

DWORD WINAPI timerforreg(LPVOID t)
{
	int counter = 0;
	while (!auntification)
	{
		Sleep(1000);
		counter++;
		if (counter > 30)
		{
			MessageBox(hWndg, L"Провалено", L"Caption", MB_OK);
			result = 2;
			return (-1);
		}
	}
	result = 1;
	MessageBox(hWndg, L"Регистрация успешна", L"Caption", MB_OK);
	return (1);
}

DWORD WINAPI timerforlife(LPVOID t)
{
	while (1)
	{
		Sleep(1000);
		counterlife++;
		if (counterlife > 30)
		{
			sendclose();
			return(1);
		}
	}
	return (1);
}

DWORD WINAPI writetoconnect(LPVOID t)
{
	DWORD temp, signal;
	DWORD dwBytesWritten;
	DWORD temp2 = 0;
	OVERLAPPED overlapped = { 0 };
	bool result;

	FlushFileBuffers(Write->reth());
	overlapped.hEvent = CreateEvent(NULL, true, false, NULL);
	if (!overlapped.hEvent)
	{
		MessageBox(hWndg, L"Ошибка", L"Caption", MB_OK);
	}
	while (1)
	{
		result = WriteFile(Write->reth(), buffer , bytestosend, &temp, &overlapped);
		signal = WaitForSingleObject(overlapped.hEvent, INFINITY);    
		if ((signal == WAIT_OBJECT_0) && (GetOverlappedResult(Write->reth(), &overlapped, &temp2, true)))
		{
			if (temp2 > 0)
			{
				free(buffer);
				buffer = NULL;
				return (1);
			}
		}
	}
	return -1;
}

DWORD WINAPI read(LPVOID t)
{
	OVERLAPPED OL = { 0 };
	DWORD CommEventMask = EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING |
		EV_RLSD | EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY;
	BOOL retcode = SetCommMask(Read->reth(), CommEventMask);
	if (!retcode)
		OL.Offset = 0;
	OL.OffsetHigh = 0;
	DWORD EventMask = 0;
	int cadr = 0;
	int b = 0;
	char* lpInBuffer = NULL;

	do
	{
		retcode = WaitCommEvent(Read->reth(), &EventMask, &OL);
		if ((!retcode) && (GetLastError() == ERROR_IO_PENDING))
			WaitForSingleObject(OL.hEvent, INFINITE);
		if (EventMask & EV_RXCHAR)
		{
			DWORD ErrorMask = 0;
			COMSTAT CStat;
			ClearCommError(Read->reth(), &ErrorMask, &CStat);
			DWORD quelen = CStat.cbInQue;
			lpInBuffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, quelen + 1);
			DWORD dwReaded = 0;
			retcode = ReadFile(Read->reth(), lpInBuffer, quelen, &dwReaded, &OL);
			if (dwReaded == 0 && GetLastError() == ERROR_IO_PENDING)
				retcode = GetOverlappedResult(Read->reth(), &OL, &dwReaded, FALSE);
			else
				if (dwReaded > 0)
				{
					if (lpInBuffer[0] == 1)
					{
						if (maincomputer == true)
						{
							if (lpInBuffer[1] == 2)
							{
								computerscount = 2;
								auntification = true;
							}
							if (lpInBuffer[1] == 3)
							{
								computerscount = 3;
								auntification = true;
							}
						}
						else
						{
							MessageBox(hWndg, L"Соединение", L"Caption", MB_OK);
							a1->setid((int)lpInBuffer[1] + 1);
							auntification = true;
							if (buffer == NULL)
								buffer = (char*)malloc(sizeof(char) * 2);
							else
							{
								free(buffer);
								buffer = (char*)malloc(sizeof(char) * 2);
							}
							buffer[0] = 1;
							buffer[1] = (int)lpInBuffer[1] + 1;
							bytestosend = 2;
							PurgeComm(Read->reth(), PURGE_RXCLEAR);
							CreateThread(NULL, 0, writetoconnect, buffer, bytestosend, NULL);
						}
					}
					else if (lpInBuffer[0] == 2)
					{
						//уведомление о закрытии одного из приложения
						//закрываем порты и уведомляем о разрыве соединения
						if (buffer == NULL)
							buffer = (char*)malloc(sizeof(char) * 1);
						else
						{
							free(buffer);
							buffer = (char*)malloc(sizeof(char) * 1);
						}
						buffer[0] = 0x02;
						bytestosend = 1;
						CreateThread(NULL, 0, writetoconnect, buffer, bytestosend, NULL);
						MessageBox(hWndg, L"Соединение разорвано", L"Caption", MB_OK);
						while (b < 2)
						{
							Sleep(1000);
							b++;
						}
						if (a1)
							a1->close();
					}
					else if (lpInBuffer[0] == 3)
					{
						if (buffer == NULL)
							buffer = (char*)malloc(sizeof(char) * 1);
						else
						{
							free(buffer);
							buffer = (char*)malloc(sizeof(char) * 1);
						}
						buffer[0] = 3;
						bytestosend = 1;
						TerminateThread(life, 0);
						counterlife = 0;
						life = CreateThread(NULL, 0, timerforlife, NULL, 0, NULL);
						CreateThread(NULL, 0, writetoconnect, buffer, bytestosend, NULL);
					}
				}
		}
		EventMask = 0;
		ResetEvent(OL.hEvent);
	} while (1);
}


DWORD WINAPI regthread(LPVOID t)
{
	maincomputer = true;
	a1->setid(1);
	CreateThread(NULL, 0, timerforreg, NULL, 0, NULL);
	while (result == 0)
	{
		buffer = (char*)malloc(sizeof(char) * 2);
		buffer[0] = 1;
		buffer[1] = 1;
		bytestosend = 2;
		PurgeComm(Write->reth(), PURGE_TXCLEAR | PURGE_TXABORT);
		CreateThread(NULL, 0, writetoconnect, NULL, 0, NULL);
		Sleep(7000);
	}
	if (result == 1)
	{
		//регистрация прошла успешно
		//создаем пакет жизни
		if (buffer == NULL)
			buffer = (char*)malloc(sizeof(char) * 1);
		else
		{
			free(buffer);
			buffer = (char*)malloc(sizeof(char) * 1);
		}
		buffer[0] = 0x03;
		bytestosend = 1;
		PurgeComm(Write->reth(), PURGE_TXCLEAR | PURGE_TXABORT);
		CreateThread(NULL, 0, writetoconnect, NULL, 0, NULL);
		life = CreateThread(NULL, 0, timerforlife, NULL, 0, NULL);
	}
	else
	{
		//регистрация не удалась
	}
	return (1);
}


void reg()
{
	if (!auntification)
	{
		CreateThread(NULL, 0, regthread, NULL, 0, NULL);
	}
	else
	{
		//char text[31] = "Компьютер уже зарегестрирован";
		char a;
		//text[30]
		a = a1->getid() + '0';
		wchar_t* str;
		str = convertCharArrayToLPCWSTR(&a);
		MessageBox(hWndg, str, L"Caption", MB_OK);
		free (str);
	}
}

void transmition()
{
	ifstream f("./Text.txt", ios::binary | ios::in);
	ofstream w("./Text1.txt", ios::binary | std::fstream::out);
	char *buff = NULL;
	char *temp = NULL;
	char* mem = NULL;
	buff = (char*)malloc(sizeof(char)*500);
	int strsize = 0;
	int readed = 0;
	while ((readed = f.read(buff, 500).gcount()) > 0)
	{
		if (mem == NULL)
		{
			strsize = readed;
			mem = (char*)malloc(sizeof(char) * readed);
			memcpy(mem, buff, readed);
		}
		else
		{
			temp = mem;
			mem = (char*)malloc(sizeof(char) * (strsize + readed));
			memcpy(mem, temp, strsize);
			memcpy(mem + strsize, buff, readed);
			free(temp);
			strsize = strsize + readed;
		}
		//for (int i = 7; i >= 0; i--) // or (int i = 0; i < 8; i++)  if you want reverse bit order in bytes
		//{
		//	int a = ((c >> i) & 1);
		//	char b = '0' + a;
		//	w.write(&b, 1);
		//}
		//w.close();
	}

	//w.write(mem, strsize);
	//w.close();
}

void close()
{
	a1->close();
}


void sendclose()
{
	MessageBox(hWndg, L"выход", L"Caption", MB_OK);
	int b = 0;
	if (buffer == NULL)
		buffer = (char*)malloc(sizeof(char) * 1);
	else
	{
		free(buffer);
		buffer = (char*)malloc(sizeof(char) * 1);
	}
	buffer[0] = 0x02;
	bytestosend = 1;
	if (a1 != NULL)
	{
		if (a1->status)
		{
			CreateThread(NULL, 0, writetoconnect, NULL, 0, NULL);
			while (b < 2)
			{
				Sleep(1000);
				b++;
			}
			a1->close();
			PostQuitMessage(0);
		}
	}
}