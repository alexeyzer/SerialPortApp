﻿#include <windows.h>
#include <iostream>
#include <ctime>
#include <locale>
#include <codecvt>
#include <string>
#include "View.h"
#include "SerialPortDataTransmit.h"
#include <shellapi.h>

using namespace std;

registration* a1;

HWND hWndg;

int catcher = 0;

int writeperm = 1;

int catched = 0;

int sender = 0;

bool auntification = false;

char* buffer = (char*)malloc(sizeof(char) * 4000);

char* temps = (char*)malloc(sizeof(char) * 4000);

int bytestosend;

int computerscount = 0;

int result = 0;

int livingflag = 0;

bool maincomputer = false;

int error = 0;

int recivestatus;

char* bufferread = (char*)calloc(4000, sizeof(char));

bool registration::getstatusw() { return (write); }
void registration::openfd()
{
	if (pathtofile != NULL)
	{
		free(pathtofile);
		pathtofile = NULL;
	}
	int pathsize = strlen(a1->path);
	int newsize = pathsize + 1 + a1->filesize;
	pathtofile  = (char*)calloc(newsize, sizeof(char));
	memcpy(pathtofile, a1->path, pathsize);
	pathtofile[pathsize] = '\\';
	memcpy(pathtofile + pathsize + 1, filename, a1->filesize);
	fd.open(pathtofile, ofstream::out | ofstream::trunc);
	write = true;
}
void registration::getname(char* source, int len)
{
	filename = (char*)calloc(len, sizeof(char));
	memcpy(filename, source, len);
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
	wchar_t* scomport;
	HANDLE hSerial;
	DCB dcbSerialParams = { 0 };
public:
	Comport(char *comport)
	{
		HANDLE hSerial;
		scomport = convertCharArrayToLPCWSTR(comport);
	}
	wchar_t  *returner() { return scomport; }
	void open()
	{
		hSerial = CreateFile(scomport,
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
	while (now < bytestosend / 2)
	{
		buff = codder(buffer[now]);
		memcpy(temps + nowtowrite, buff, 2);
		nowtowrite += 2;
		now++;
		free(buff);
	}
	memcpy(buffer, temps, bytestosend);
	memset(temps, 0, 4000);
}

registration::registration(bool major, char *comtowrite, char *comtoread)
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
	a1->status = false;
	Write->close();
	Read->close();
}
void registration::setid(int a)
{
	char b;
	id = a;
}
int registration::getid() { return id; }
void registration::closefd()
{
	fd.close();
	write = false;
	free(filename);
	filename = NULL;
}

void registration::writetofile(char* mem, int size) { fd.write(mem, size); }

int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());
	ws = wsTmp;
	return 0;
}

HANDLE  readthread;

void setid(int id);


DWORD WINAPI read(LPVOID t);

void WorkWithCom(HWND hWnd, char *com1, char *com2, bool major)
{
	hWndg = hWnd;
	Comport* commer;
	a1 = new registration(major, com1, com2);
}

int converttoint(char* mem)
{
	int	devider = 32768;
	int value = 0;
	int i = 7;
	int part = 1;
	int flag = 1;
	while (i >= 0)
	{
		if ((mem[part] >> i) & 1)
			value += devider;
		i--;
		devider /= 2;
		if (i < 0 && flag == 1)
		{
			part = 0;
			i = 7;
			flag = 0;
		}
	}
	return value;
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
			writeperm = 0;
			return (-1);
		}
	}
	result = 1;
	// главный компьютер установил соед с ведомыми 
	SendMessage(hWndg, WM_USER, NULL, NULL);
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
		Sleep(1000);
	codebuff();
	while (writeperm == 1)
	{
		result = WriteFile(Write->reth(), buffer, bytestosend, &temp, &overlapped);
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
	char* temp;

	i = 0;
	j = 0;
	if (syze > 4000)
		syze = 4000;
	while (i < syze)
	{
		if (iserror(memoery[i]) == 1)
			error = 1;
		i++;
	}
	i = 0;
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
							SendMessage(hWndg, WM_USER+1, NULL, NULL);
							// установлено соед с главным

							buffer[0] = 1;
							buffer[1] = (int)bufferread[1] + 1;
							bytestosend = 2;
							writetoconnect(NULL);
						}
					}
					else if (bufferread[0] == 2)
					{
						//завершение программы
						//уведомление о закрытии одного из приложения
						//закрываем порты и уведомляем о разрыве соединения
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
						buffer[0] = 0x03;
						bytestosend = 1;
						if (catcher == 1)
							catched = 1;
						else
							writetoconnect(NULL);
						if (livingflag == 1)
						{
							MessageBox(hWndg, L"пакет тут", L"Caption", MB_OK);
							livingflag = 0;
						}
					}
					else if (bufferread[0] == 4)
					{
						if (error == 1)
						{
							buffer[0] = 5;
							buffer[1] = bufferread[1];
							buffer[2] = 1;
							bytestosend = 3;
							writetoconnect(NULL);
							error = 0;
						}
						else
						{
							if (catcher == 0)
							{
								if (bufferread[2] == a1->getid())
								{
									//происходит прием на этот компьютер
									//запускаем функцию записи в файл
									//MessageBox(hWndg, L"получение файла", L"Caption", MB_OK);
									if (bufferread[3] == 1)
									{
										if (a1->getstatusw() == false)
										{
											a1->lastreciveid = bufferread[1];
											a1->filesize = converttoint(&bufferread[4]);
											a1->getname(bufferread + 5, a1->filesize);
											a1->openfd();
										}
									}
									else if (bufferread[3] == 2)
										a1->writetofile(&(bufferread[6]), converttoint(&bufferread[4]));
									else if (bufferread[3] == 3)
									{
										a1->closefd();
									}
									buffer[0] = 5;
									buffer[1] = bufferread[1];
									buffer[2] = 2;
									bytestosend = 3;
									writetoconnect(NULL);
								}
								else
								{
									//происходит ретрансляция через этот компьютер
									memcpy(buffer, bufferread, dwReaded / 2);
									bytestosend = dwReaded / 2;
									writetoconnect(NULL);
								}
							}
						}
					}
					else if (bufferread[0] == 5)
					{
						if (bufferread[1] == a1->getid())
						{
							//MessageBox(hWndg, L"пришел ответ по файлу", L"Caption", MB_OK);
							if (bufferread[2] == 1)
								recivestatus = 1;//ошибка
							else
								recivestatus = 2; //дошло все нормально
						}
						else
						{
							memcpy(buffer, bufferread, dwReaded / 2);
							bytestosend = dwReaded / 2;
							writetoconnect(NULL);
						}

					}
					else if (bufferread[0] == 6)
					{
						MessageBox(hWndg, L"файл открыт", L"Caption", MB_OK);
					}
				}
		}
		memset(bufferread, 0, 400);
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
		buffer[0] = 1;
		buffer[1] = 1;
		bytestosend = 2;
		writetoconnect(NULL);
		Sleep(2000);
	}
	if (result == 1)
	{
		//регистрация прошла успешно
		//создаем пакет жизни
		buffer[0] = 3;
		bytestosend = 1;
		PurgeComm(Write->reth(), PURGE_TXCLEAR | PURGE_TXABORT);
		writetoconnect(NULL);
	}
	else
	{
		//регистрация не удалась
		//отправить сообщение для семны изображения
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
		free(str);
	}
}

void close()
{
	a1->close();
}

DWORD WINAPI fileopend(LPVOID t)
{
	int go;

	go = 1;
	if (a1->status)
	{
		if (a1->lastreciveid != 0)
		{
			while (go == 1)
			{
				buffer[0] = 6;
				buffer[1] = a1->lastreciveid;//надо поставить id от которого был принят файл
				buffer[2] = a1->getid();//id компьюетра который говорит что он открыл файл
				bytestosend = 3;
				recivestatus = 0;
				writetoconnect(NULL);
				while (recivestatus == 0)
					continue;
				if (recivestatus == 2)
					go = 0;
				recivestatus = 0;
			}
		}
		else
			MessageBox(hWndg, L"Информация о последнем компьютере отсутствуют", L"Caption", MB_OK);
	}
	else
		MessageBox(hWndg, L"Уведомление об откытии файла не будет отправленно так как нет соединения", L"Caption", MB_OK);
	return (1);
}

void sendclose()
{
	MessageBox(hWndg, L"выход", L"Caption", MB_OK);
	int b = 0;
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
			a1->status = false;
			PostQuitMessage(0);
		}
	}
}

char* extrudename(char* path)
{
	int i;
	int len;
	char* name;
	i = 0;
	len = 0;
	while (path[i] != '\0')
		i++;
	while (path[i] != '\\' && i > 0)
	{
		i--;
		len++;
	}
	name = (char*)calloc(len, sizeof(char));
	memcpy(name, path + i + 1, len);
	return (name);
}

char* path = new char[40];
// char path1[] = ".//Text.txt";
char* file = new char[40];


void setFileName(wchar_t* filePath) {
	WideCharToMultiByte(CP_UTF8, NULL, filePath, 255, file, 255, NULL, NULL);
}

void setPath(wchar_t* filePath) 
{

	WideCharToMultiByte(CP_UTF8, NULL, filePath, 255, a1->path, 255, NULL, NULL);
	int a = 0;
}

char* converttobyte(int size)
{
	char* syze;

	int	devider = 32768;
	char a;
	int part;
	part = 1;
	a = 0x01;
	int flag = 1;
	int i = 7;

	syze = (char*)malloc(sizeof(char) * 2);
	memset(syze, 0, 2);
	while (size > 0)
	{
		if (size / devider >= 1)
		{
			size -= devider;
			syze[part] = syze[part] | (a << i);
		}
		devider /= 2;
		i--;
		if (i < 0 && flag == 1)
		{
			part = 0;
			i = 7;
			flag = 0;
		}
	}
	return (syze);
}

DWORD WINAPI transmitionrun(LPVOID t)
{
	char* name;
	ifstream f;
	char buff[500];
	int i;
	int j;
	int go;
	char* temp = NULL;
	char* mem = NULL;
	int now = 0;
	int namelen;
	int nowtowrite = 0;
	int strsize = 0;
	int readed = 0;
	int idofrecivingcomp = 2;
	char* tempsize;


	f.open(file, ios::binary | ios::in);
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
	recivestatus = 0;
	now = 0;
	//на данном этапе в mem лежит закодированный файл
	catcher = 1;
	while (catched == 0)
		continue;
	name = extrudename(file);
	i = 0;
	namelen = strlen(name);
	while (recivestatus != 2)
	{
		buffer[0] = 4;
		buffer[1] = a1->getid();
		buffer[2] = idofrecivingcomp;
		buffer[3] = 1;
		tempsize = converttobyte(namelen);
		buffer[4] = tempsize[0];
		buffer[5] = tempsize[1];
		free(tempsize);
		memcpy(buffer + 5, name, namelen);
		bytestosend = 5 + namelen;
		writetoconnect(NULL);
		while (recivestatus == 0)
			continue;
	}
	recivestatus = 0;
	go = 1;
	while (i < strsize)
	{
		//MessageBox(hWndg, L"отправка куска", L"Caption", MB_OK);
		buffer[0] = 4;
		buffer[1] = a1->getid();
		buffer[2] = idofrecivingcomp;
		buffer[3] = 2;

		j = 0;
		while (j < 1000 && i < strsize && go == 1)
		{
			if (mem[i] == '\r')
				i++;
			buffer[6 + j] = mem[i];
			j++;
			i++;
		}
		tempsize = converttobyte(j);
		buffer[4] = tempsize[0];//количество информационных битов
		buffer[5] = tempsize[1];//
		free(tempsize);
		bytestosend = 6 + j;
		writetoconnect(NULL);
		Sleep(250);
		while (recivestatus == 0)
			continue;
		if (recivestatus == 2)
			go = 1;
		else
			go = 0;
		recivestatus = 0;
	}
	buffer[0] = 4;
	buffer[1] = a1->getid();
	buffer[2] = idofrecivingcomp;
	buffer[3] = 3;
	bytestosend = 4;
	writetoconnect(NULL);
	MessageBox(hWndg, L"файл доставлен", L"Caption", MB_OK);
	free(name);
	free(mem);
	mem = NULL;
	name = NULL;
	f.close();
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

DWORD WINAPI threadopenfile(LPVOID t)
{
	char* newstr;

	if (a1->pathtofile != NULL)
	{
		newstr = (char*)calloc(a1->filesize + 8, sizeof(char));
		memcpy(newstr, "notepad ", 8);
		memcpy(newstr + 8, a1->pathtofile, a1->filesize);
		if (a1->status == true)
			CreateThread(NULL, 0, fileopend, NULL, 0, NULL);
		system(newstr);
		free(newstr);
	}
	else
	{
		MessageBox(hWndg, L"Файл не был принят", L"Caption", MB_OK);
	}
	return (0);
}

void openfile()
{
	CreateThread(NULL, 0, threadopenfile, NULL, 0, NULL);
}