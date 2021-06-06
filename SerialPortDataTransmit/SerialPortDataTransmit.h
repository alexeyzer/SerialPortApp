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
void openfile();

struct users
{
	int id;
	char* name = NULL;
	int namelen = 0;
};


class registration
{
private:
	int				id;
	bool			write = false;
	std::ofstream	fd;
	char* filename = NULL;
	char* memory = NULL;
	int size = 0;
public:
	int  filesize;
	char path[255] = ".";
	char *pathtofile = NULL;
	bool status;
	int  lastreciveid = 0;
	char* username = NULL;
	int countofcomputers = 0;
	users user[3];
	registration(bool major, char *comtowrite, char *comtoread);
	void close();
	void setid(int a);
	int getid();
	bool getstatusw();
	void openfd();
	void getname(char* source, int len);
	void writetofile();
	void closefd();
	void tomassive(char* source, int len);
	void setname(char* name);
};

registration* returner(void);