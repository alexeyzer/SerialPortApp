#include <windows.h>
#include <iostream>
#include <ctime>
#include <locale>
#include <codecvt>
#include <string>
using namespace std;

HWND hWndg;

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
		
		dcbSerialParams.BaudRate = CBR_9600;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.fParity = FALSE;
		dcbSerialParams.Parity = EVENPARITY;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.fErrorChar = FALSE;
		//dcbSerialParams.ErrorChar=255;
		dcbSerialParams.fNull = FALSE;                     // задаём 8 бит в байте                            // включаем проверку чётности      
		dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
		dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
		// задаём один стоп-бит

		SetSerialParams();

		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = MAXDWORD;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.ReadTotalTimeoutConstant =100;
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 0;
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


DWORD WINAPI writetoconnect(LPVOID t)
{
	DWORD temp, signal;
	DWORD dwSize = sizeof(t);   // ðàçìåð ýòîé ñòðîêè
	DWORD dwBytesWritten;
	MessageBox(hWndg, convertCharArrayToLPCWSTR((const char*)t), L"Caption", MB_OK);      //создать событие
	HANDLE hserial = Write->reth();
	int counter = 0;
	OVERLAPPED overlapped = { 0 };
	overlapped.hEvent = CreateEvent(NULL, true, true, NULL);
	PurgeComm(hserial, PURGE_TXCLEAR | PURGE_RXCLEAR);
	while (1)
	{
		counter++;
		WriteFile(hserial, "error", 4, &temp, &overlapped);
		char str1[100];
		snprintf(str1, 100, "%i", counter);
		LPCWSTR str = convertCharArrayToLPCWSTR(str1);
		TextOut(GetDC(hWndg), 300, 300, str,10);
		delete str;
		signal = WaitForSingleObject(overlapped.hEvent, 40);    //приостановить поток, пока не завершится перекрываемая операция WriteFile
		if ((signal == WAIT_OBJECT_0) && (GetOverlappedResult(Write->reth(), &overlapped, &temp, true))) //если операция завершилась успешно
		{
			MessageBox(hWndg, L"Отпрвалено", L"Caption", MB_OK);
			return (1); //вывести сообщение об этом в строке состояния
			
		}
		PurgeComm(hserial, PURGE_TXCLEAR | PURGE_RXCLEAR);
	}
	//CloseHandle(hSerial); //close the handle
	return -1;
}

DWORD WINAPI readforconnect(LPVOID t)
{
	char szBuf[100];
	OVERLAPPED overlapped = {0};
	COMSTAT comstat;        // структура текущего состояния порта, в данной программе используется для определения количества принятых в порт байтов
	DWORD btr, temp, mask, signal;  // переменная temp используется в качестве заглушки
	int flag;

	flag = 1;
	overlapped.hEvent = CreateEvent(NULL, true, true, NULL); // создать сигнальный объект-событие для асинхронных операций
	SetCommMask(Read->reth() , EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING |
		EV_RLSD | EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY);                     // установить маску на срабатывание по событию приёма байта в порт
	while (flag)                       // пока поток не будет прерван, выполняем цикл
	{
		WaitCommEvent(Read->reth(), &mask, &overlapped);   //ожидать события приёма байта (это и есть перекрываемая операция)
		signal = WaitForSingleObject(overlapped.hEvent, 40);    //приостановить поток до прихода байта
		if (signal == WAIT_OBJECT_0)                       //если событие прихода байта произошло
		{
			MessageBox(hWndg, convertCharArrayToLPCWSTR(szBuf), L"Caption", MB_OK);
			if (GetOverlappedResult(Read->reth(), &overlapped, &temp, true)) //проверяем, успешно ли завершилась перекрываемая операция WaitCommEvent
				if ((mask & EV_RXCHAR) != 0)                 //если произошло именно событие прихода байта
				{
					ClearCommError(Read->reth(), &temp, &comstat); // нужно заполнить структуру COMSTAT
					btr = comstat.cbInQue;                    // и получить из неё количество принятых байтов
					if (btr)                                   // если действительно есть байты для чтения
					{
						ReadFile(Read->reth(), szBuf, btr, &temp, &overlapped); // прочитать байты из порта в буфер программы
						MessageBox(hWndg, convertCharArrayToLPCWSTR(szBuf), L"Caption", MB_OK);
						if (szBuf[0] == 'c')
						{
							if (atoi(&szBuf[1]) > 0)
								return (atoi(&szBuf[1] + 1));
							else
								return (-1);
						}
						else
							return (-1);
					}
				}
		}
	}
	return (1);
}


class registration
{
private:
	int id;
	bool success;
public:
	registration(bool major, int comtowrite, int comtoread)
	{
		char szBuf[100];
		int	temp;
		char a[2];
		a[0] = 'c';
		a[1] = '1';
		
		Write = new Comport(comtowrite);
		Read = new Comport(comtoread);
		Write->open();
		Read->open();
		if (major == true)
		{
			id = 1;
			CreateThread(NULL, 0, writetoconnect, LPVOID(a), 0, NULL);
		}
		else
		{
			CreateThread(NULL, 0, readforconnect, NULL, 0, NULL);
			//CreateThread(NULL, 0, writetoconnect, LPVOID(a), 0, NULL);
			MessageBox(hWndg, L"выход Ведомго", L"Caption", MB_OK);

		}
	}


};

registration *a1;

void WorkWithCom(HWND hWnd, int com1, int com2, bool major)
{
	hWndg = hWnd;
	Comport *commer;

	a1 = new registration(major, com1, com2);
}
