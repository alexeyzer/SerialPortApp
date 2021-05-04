#include <windows.h>
#include <iostream>
#include <ctime>
#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include "SerialPortDataTransmit.h"
using namespace std;

registration *a1;

HWND hWndg;

int catcher = 0;

int catched = 0;

int sender = 0;

int recivestatus = 0;

bool auntification = false;

char* buffer = NULL;

int bytestosend;

int computerscount = 0;

int result = 0;

int livingflag = 0;

bool maincomputer = false;

char* temps = NULL;

char *bufferread  = (char*)malloc(sizeof(char) * 60);

void realocbuff()
{
	if (buffer == NULL)
		buffer = (char*)malloc(sizeof(char) * 60);
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

char* decoder(char* bytes)
{
	char* result;
	int i = 0;
	result = (char*)calloc(1, sizeof(char));
	*result |= ((bytes[0] >> 2) & 1);
	*result |= (((bytes[0] >> 4) & 1) << 1);
	*result |= (((bytes[0] >> 5) & 1) << 2);
	*result |= (((bytes[0] >> 6) & 1) << 3);
	*result |= (((bytes[1] >> 2) & 1) << 4);
	*result |= (((bytes[1] >> 4) & 1) << 5);
	*result |= (((bytes[1] >> 5) & 1) << 6);
	*result |= (((bytes[1] >> 6) & 1) << 7);
	*result = (char)*result;
	return (result);
}

int iserror(char byte)
{
	if (((byte) & 1) ^ ((byte >> 2) & 1) ^ ((byte >> 4) & 1) ^ ((byte >> 6) & 1))
		return (1);
	if (((byte >> 1) & 1) ^ ((byte >> 2) & 1) ^ ((byte >> 5) & 1) ^ ((byte >> 6) & 1))
		return (1);
	if (((byte >> 3) & 1) ^ ((byte >> 4) & 1) ^ ((byte >> 5) & 1) ^ ((byte >> 6) & 1))
		return (1);
	return (0);
}

char* codder(char byte)
{
	int i;
	int j;
	int k;
	char* a;

	i = 0;
	j = 0;
	k = 2;
	a = (char*)calloc(2, sizeof(char));
	while (i < 8)
	{
		a[j] = a[j] | (((byte >> i) & 1) << k);
		if (k < 6 && k >= 4)
			k++;
		else if (k == 2)
			k += 2;
		else if (k == 6)
		{
			k = 2;
			j++;
		}
		i++;
	}
	i = 0;
	while (i < 2)
	{
		a[i] |= ((a[i] >> 2) & 1) ^ ((a[i] >> 4) & 1) ^ ((a[i] >> 6) & 1);
		a[i] |= (((a[i] >> 2) & 1) ^ ((a[i] >> 5) & 1) ^ ((a[i] >> 6) & 1)) << 1;
		a[i] |= (((a[i] >> 4) & 1) ^ ((a[i] >> 5) & 1) ^ ((a[i] >> 6) & 1)) << 3;
		i++;
	}
	return (a);
}

void decodeanndwritetofile(char* memoery, int syze)
{
	int i;
	char* temp;
	ofstream w("./Text1.txt", fstream::out);
	i = 0;
	while (i < syze * 2)
	{
		temp = decoder(&(memoery[i]));
		i += 2;
		w.write(temp, 1);
		free(temp);
	}
}

void codebuff()
{
	char* buff;
	int		nowtowrite;
	int		now;

	now = 0;
	nowtowrite = 0;
	bytestosend = bytestosend * 2;
	if (temps == NULL)
		temps = (char*)calloc(bytestosend, sizeof(char));
	while (now < bytestosend / 2)
	{
		buff = codder(buffer[now]);
		memcpy(temps + nowtowrite, buff, 2);
		nowtowrite += 2;
		now++;
		free(buff);
	}
	realocbuff();
	memcpy(buffer, temps, bytestosend);
}

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

DWORD WINAPI writetoconnect(LPVOID t)
{
	DWORD temp, signal;
	DWORD dwBytesWritten;
	DWORD temp2 = 0;
	OVERLAPPED overlapped = { 0 };
	bool result;

	overlapped.hEvent = CreateEvent(NULL, true, false, NULL);
	if (!overlapped.hEvent)
		MessageBox(hWndg, L"Ошибка", L"Caption", MB_OK);
	if (buffer[0] == 3)
		Sleep(3000);
	codebuff();
	while (1)
	{
		result = WriteFile(Write->reth(), buffer , bytestosend, &temp, &overlapped);
		signal = WaitForSingleObject(overlapped.hEvent, INFINITY);    
		if ((signal == WAIT_OBJECT_0) && (GetOverlappedResult(Write->reth(), &overlapped, &temp2, true)))
		{
			if (temp2 > 0)
				return (1);
		}
	}
	return -1;
}

void decodetobuff(char* memoery, int syze)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	j = 0;
	if (syze > 60)
		syze = 60;
	while (i < syze)
	{
		temp = decoder(&(memoery[i]));
		bufferread[j] = *temp;
		free(temp);
		i += 2;
		j++;
	}
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
					decodetobuff(lpInBuffer, dwReaded);
					if (bufferread[0] == 1)
					{
						if (maincomputer == true)
						{
							if (bufferread[1] == 2)
							{
								computerscount = 2;
								auntification = true;
							}
							if (bufferread[1] == 3)
							{
								computerscount = 3;
								auntification = true;
							}
						}
						else
						{
							a1->setid((int)bufferread[1] + 1);
							auntification = true;
							realocbuff();
							buffer[0] = 1;
							buffer[1] = (int)bufferread[1] + 1;
							bytestosend = 2;
							CreateThread(NULL, 0, writetoconnect, NULL, bytestosend, NULL);
						}
					}
					else if (bufferread[0] == 2)
					{
						//завершение программы
						//уведомление о закрытии одного из приложения
						//закрываем порты и уведомляем о разрыве соединения
						realocbuff();
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
					else if (bufferread[0] == 3)
					{
						realocbuff();
						buffer[0] = 0x03;
						bytestosend = 1;
						if (catcher == 1)
							catched = 1;
						else
							CreateThread(NULL, 0, writetoconnect, NULL, bytestosend, NULL);
						if (livingflag == 1)
						{
							MessageBox(hWndg, L"пакет тут", L"Caption", MB_OK);
							livingflag = 0;
						}
					}
					else if (bufferread[0] == 4)
					{
						if (catcher == 0)
						{
							if (bufferread[1] == a1->getid())
							{
								//происходит прием на этот компьютер
							}
							else
							{
								//происходит ретрансляция через этот компьютер
							}
						}
						realocbuff();
						//перевести 
						buffer[0] = 0x03;
						bytestosend = 1;

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
		realocbuff();
		buffer[0] = 1;
		buffer[1] = 1;
		bytestosend = 2;
		PurgeComm(Write->reth(), PURGE_TXCLEAR | PURGE_TXABORT);
		writetoconnect(NULL);
		Sleep(4000);
	}
	if (result == 1)
	{
		//регистрация прошла успешно
		//создаем пакет жизни
		realocbuff();
		buffer[0] = 3;
		bytestosend = 1;
		PurgeComm(Write->reth(), PURGE_TXCLEAR | PURGE_TXABORT);
		CreateThread(NULL, 0, writetoconnect, NULL, 0, NULL);
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


DWORD WINAPI transmitionrun(LPVOID t)
{
	ifstream f("./Text.txt", ios::binary | ios::in);
	char* buff = NULL;
	int i;
	char* temp = NULL;
	char* mem = NULL;
	int now = 0;
	int nowtowrite = 0;
	buff = (char*)calloc(500, sizeof(char));
	int strsize = 0;
	int readed = 0;

	while ((readed = f.read(buff, 500).gcount()) > 0)
	{
		if (mem == NULL)
		{
			strsize = readed;
			mem = (char*)calloc(readed, sizeof(char));
			memcpy(mem, buff, readed);
		}
		else
		{
			temp = mem;
			mem = (char*)calloc(strsize + readed, sizeof(char));
			memcpy(mem, temp, strsize);
			memcpy(mem + strsize, buff, readed);
			free(temp);
			strsize = strsize + readed;
		}
	}
	free(buff);
	temp = (char*)calloc((strsize * 2), sizeof(char));
	while (now < strsize)
	{
		buff = codder(mem[now]);
		memcpy(temp + nowtowrite, buff, 2);
		nowtowrite += 2;
		now++;
		free(buff);
	}
	free(mem);
	mem = temp;
	temp = NULL;
	now = 0;
	//на данном этапе в mem лежит закодированный файл
	catcher = 1;
	while (catched == 0)
		continue;
	sender = 1;
	i = 0;
	realocbuff();
	buffer[0] = 4;
	buffer[1] = 2;
	while (sender)
	{
		while (i < strsize * 2)
		{
			buffer[0] = mem[i];
			buffer[1] = mem[i + 1];
			i += 2;

		}

	}

	/*
	while (now < strsize * 2)
	{
		if (iserror(&(mem[now])) > 0)
			cout << "error" << endl;
		now += 2;
	}
	decodeanndwritetofile(mem, strsize);
	*/
	return (0);
}

void transmition()
{
	CreateThread(NULL, 0, transmitionrun, NULL, 0, NULL);
}



void testflag()
{
	livingflag = 1;
}