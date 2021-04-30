#pragma once

#include "resource.h"
void WorkWithCom(HWND hWnd, int com1, int com2, bool major);
void close();
void reg();
void transmition();
void sendclose();
DWORD WINAPI read(LPVOID t);

class registration
{
private:
	int id;
public:
	bool status;
	registration(bool major, int comtowrite, int comtoread);
	void close();
	void setid(int a);
	int getid();
};
