#pragma once

#include "resource.h"
#include <fstream>
#include <iostream>
#include <ostream>

void WorkWithCom(HWND hWnd, char *com1, char *com2, bool major);
void close();
void reg();
void transmition();
void sendclose();
DWORD WINAPI read(LPVOID t);
void testflag();
void setFileName(wchar_t* filePath);
void setPath(wchar_t* filePath);

class registration
{
private:
	int				id;
	bool			write = false;
	std::ofstream	fd;
	char* filename = NULL;

public:
	bool status;
	registration(bool major, char *comtowrite, char *comtoread);
	void close();
	void setid(int a);
	int getid();
	bool getstatusw();
	void openfd();
	void getname(char* source, int len);
	void writetofile(char* mem, int size);
	void closefd();

};
