#include <windows.h>
#include <iostream>
#include <ctime>
#include <locale>
#include <codecvt>
#include <string>
using namespace std;
#include <bitset>
#include <fstream>
#include <iostream>
#include <ostream>

HWND hWndg;
bool globaltimer = false;

bitset<7> a;

char* buffer = NULL;
int bytestosend;

int computerscount = 0;

bool maincomputer = false;

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
			(DWORD)(GENERIC_READ | (DWORD)GENERIC_WRITE),
			(DWORD)0,//(FILE_SHARE_READ | FILE_SHARE_WRITE),
			NULL,
			(DWORD)OPEN_EXISTING,
			(DWORD)FILE_FLAG_OVERLAPPED,//0
			NULL);
		if (hSerial == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
			{
				MessageBox(hWndg, L"serial port does not exist", L"Caption", MB_OK);
			}
			MessageBox(hWndg, L"some other error occurred. Inform user.", L"Caption", MB_OK);
		}

		GetSerialParams();
		
		dcbSerialParams.BaudRate = CBR_57600;
		//dcbSerialParams.fBinary = TRUE;                         // включаем двоичный режим обмена
		dcbSerialParams.fOutxCtsFlow = FALSE;                   // выключаем режим слежения за сигналом CTS
		///dcbSerialParams.fOutxDsrFlow = FALSE;                   // выключаем режим слежения за сигналом DSR
		//dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;      // отключаем использование линии DTR
		dcbSerialParams.fDsrSensitivity = FALSE;                // отключаем восприимчивость драйвера к состоянию линии DSR
		//dcbSerialParams.fNull = FALSE;//TRUE                    // разрешить приём нулевых байтов
		//dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;      // отключаем использование линии RTS
		//dcbSerialParams.fAbortOnError = FALSE;                  // отключаем остановку всех операций чтения/записи при ошибке
		dcbSerialParams.ByteSize = 8;                           // задаём 8 бит в байте
		dcbSerialParams.Parity = 1;                             // включаем проверку чётности
		dcbSerialParams.StopBits = 0;                           // задаём один стоп-бит
		//dcbSerialParams.ErrorChar=255;


		SetSerialParams();
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
		if (!SetCommState(hSerial, &dcbSerialParams)) {
			//error setting serial port state
			MessageBox(hWndg, L"error setting serial port state", L"Caption", MB_OK);
		}
		
	}
	void close()
	{
		CloseHandle(hSerial);
		hSerial = NULL;
	}

	HANDLE reth(){return hSerial;}
	
};
Comport* Read, * Write;
DWORD WINAPI read(LPVOID t);




class registration
{
private:
	int id;
public:
	registration(bool major, int comtowrite, int comtoread)
	{
		Write = new Comport(comtowrite);
		Read = new Comport(comtoread);
		Write->open();
		Read->open();
		CreateThread(NULL, 0, read, NULL, 0, NULL);
	}
	void close()
	{
		Write->close();
		Read->close();
	}
	void setid(int a)
	{
		char b;
		id = a;
	}
	int getid()
	{
		return id;
	}
};

registration *a1;


void WorkWithCom(HWND hWnd, int com1, int com2, bool major)
{
	hWndg = hWnd;
	Comport *commer;

	a1 = new registration(major, com1, com2);
}


DWORD WINAPI timer(LPVOID t)
{
	int counter = 0;
	while (!globaltimer)
	{
		Sleep(1000);
		counter++;
		if (counter > 5)
		{
			MessageBox(hWndg, L"Провалено", L"Caption", MB_OK);
			return (-1);
		}
	}
	MessageBox(hWndg,L"Регистрация успешна", L"Caption", MB_OK);
	return (1);
}

DWORD WINAPI writetoconnect(LPVOID t)
{
	DWORD temp, signal;
	DWORD dwSize = sizeof(t);   // ðàçìåð ýòîé ñòðîêè
	DWORD dwBytesWritten;
	//MessageBox(hWndg, convertCharArrayToLPCWSTR((const char*)t), L"Caption", MB_OK);      //создать событие
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
		signal = WaitForSingleObject(overlapped.hEvent, INFINITY);    //приостановить поток, пока не завершится перекрываемая операция WriteFile
		if ((signal == WAIT_OBJECT_0) && (GetOverlappedResult(Write->reth(), &overlapped, &temp2, true))) //если операция завершилась успешно
		{
			if (temp2 > 0)
			{
				free(buffer);
				buffer = NULL;
				//MessageBox(hWndg, L"Отпрвалено", L"Caption", MB_OK);
				return (1); //вывести сообщение об этом в строке состояни
			}
		}
	}
	//CloseHandle(hSerial); //close the handle
	return -1;
}

DWORD WINAPI read(LPVOID t)
{
	//ShowMessage(s);
	OVERLAPPED OL = { 0 };
	DWORD CommEventMask = EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING |
		EV_RLSD | EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY;

	BOOL retcode = SetCommMask(Read->reth(), CommEventMask);
	if (!retcode)
		OL.Offset = 0;
	OL.OffsetHigh = 0;
	DWORD EventMask = 0;
	int cadr = 0;


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
			char* lpInBuffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, quelen + 1);
			if (!lpInBuffer)
			{

			}
			//   memset(&lpInBuffer, '\0', (int)quelen);
			DWORD dwReaded = 0;
			retcode = ReadFile(Read->reth(), lpInBuffer, quelen, &dwReaded, &OL);
			if (dwReaded == 0 && GetLastError() == ERROR_IO_PENDING)
				retcode = GetOverlappedResult(Read->reth(), &OL, &dwReaded, FALSE);
			else
				if (dwReaded > 0)
				{
					if (lpInBuffer[0] &  0x01)
					{
						if (maincomputer == true)
						{
							if (lpInBuffer[1] & 0x02)
							{
								computerscount = 2;
								globaltimer = true;
							}
							if (lpInBuffer[1] & 0x02)
							{
								computerscount = 3;
								globaltimer = true;
							}
								
						}
						else
						{
							//MessageBox(hWndg, L"Got the file", L"Caption", MB_OK);
							a1->setid((int)lpInBuffer[1] + 1);
							globaltimer = true;
							if (buffer == NULL)
							{
								buffer = (char*)malloc(sizeof(char) * 2);
								buffer[0] = 0x01;
								buffer[1] = (int)lpInBuffer[1] + 1;
								bytestosend = 2;
							}
							CreateThread(NULL, 0, writetoconnect, buffer, bytestosend, NULL);
						}
					}
				}

		}
		EventMask = 0;
		ResetEvent(OL.hEvent);
	} while (1);
}



void registration()
{
	if (!globaltimer)
	{
		maincomputer = true;
		a1->setid(1);
		buffer = (char*)malloc(sizeof(char) * 2);
		buffer[0] = 1;
		buffer[1] = 1;
		bytestosend = 2;
		CreateThread(NULL, 0, writetoconnect, NULL, 0, NULL);
		CreateThread(NULL, 0, timer, NULL, 0, NULL);
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