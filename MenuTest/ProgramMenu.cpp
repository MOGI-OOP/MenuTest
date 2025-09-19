

#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "Shlwapi.h"
#include "ProgramMenu.h"
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>

CProgramMenu::CProgramMenu()
{
	m_nMenuitems = 0;
	m_pMainTitle[0] = 0;
	m_pMenuItems = NULL;
	SetConsoleOutputCP(1250);
	memset(m_pMainTitle, 0, 32);
	setlocale(LC_CTYPE, "hu-HU");

}

CProgramMenu::~CProgramMenu(void)
{
	if (m_pMenuItems) delete[]m_pMenuItems;
}

void CProgramMenu::AddTitle(const char *pTitle)
{
	strncpy_s(m_pMainTitle, pTitle, 31);				// max 31 char másolása
}

int CProgramMenu::AddMenuItem(const char* pTitle, int retValue /* = -1 */)
{
	MENU_ITEM newItem, *pTempItems;

	memset(&newItem, 0, sizeof(MENU_ITEM));
	strncpy_s(newItem.menuItemName, pTitle, 31);				// max 31 char másolása
	if (retValue<0) newItem.menuId = m_nMenuitems+1;
	else {
		newItem.menuId = retValue;
		for (int i = 0; i < m_nMenuitems; i++) {
			if (m_pMenuItems[i].menuId == newItem.menuId) return -1;
		}
	}


	if (m_nMenuitems == 0) {
		m_pMenuItems = new MENU_ITEM[1];
		m_pMenuItems[0] = newItem;
		m_nMenuitems++;
		return 0;
	}
	pTempItems = new MENU_ITEM[m_nMenuitems];
	for (int i=0; i< m_nMenuitems; i++ ) pTempItems[i] =  m_pMenuItems[i];
	delete[]m_pMenuItems;
	m_pMenuItems = new MENU_ITEM[m_nMenuitems+1];
	for (int i = 0; i < m_nMenuitems; i++) m_pMenuItems[i] = pTempItems[i];
	m_pMenuItems[m_nMenuitems] = newItem;
	m_nMenuitems++;
	delete[]pTempItems;
	return 0;
}


int CProgramMenu::ShowMenu()
{
	int i, ch, selected = -1;
	int x=-1, y= -1;

	do {
		selected = -1;
		system("cls");
		GetTerminalCursorPos(&x, &y);
		SetCursorPos(10, 5);
		GetTerminalCursorPos(&x, &y);
		printf(m_pMainTitle);
		GetTerminalCursorPos(&x, &y);
		printf("\n\n\n");
		for (i = 0; i < m_nMenuitems; i++) {
			printf("%d\t%s", m_pMenuItems[i].menuId, m_pMenuItems[i].menuItemName);
			printf(" \n\n");
		}
		ch = _getch();
		if ((ch>='0') || (ch<='9')) selected = ch - '0';
		else {
			if ((ch >= 'A') || (ch <= 'Z')) selected = ch - 'A'+10;
			else {
				if ((ch >= 'a') || (ch <= 'z')) selected = ch - 'a' + 10;
			}
		}
	} while (selected<0);
	return selected;
}


void SetTerminalCursorPos(int x, int y)
{
	COORD coord;

	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	printf("");
}

int GetTerminalCursorPos(int *x, int *y)
{
	BOOL result; 
	CONSOLE_SCREEN_BUFFER_INFO  scBufferInfo;
	INPUT_RECORD inp;
	DWORD ev;

	PeekConsoleInput(GetStdHandle(STD_OUTPUT_HANDLE), &inp, 1, &ev),
	result = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &scBufferInfo);
	if (result) {
		*x = scBufferInfo.dwCursorPosition.X;
		*y = scBufferInfo.dwCursorPosition.Y;
		return 0;
	}
	return -1;
}


void SetColor(int textColor, int bgColor)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}


// Read string from console, options: 0x01 number, 0x02 letter, 0x04 ' ',  0x08 '.', 0x10 printable
int InputStr(char* str, int sizeStr, int options /*=0x10*/)
{
	int ch, ujra = 1, nch = 0, chOK = 0;
	char pBuffer[256];

	memset(pBuffer, 0, 256);
	if ((sizeStr > 256) || (sizeStr < 1)) return -1;
	do {
		ch = _getch();
		chOK = 0;
		if ((ch == 0) || (ch == 0xe0)) {			// function or arrow key
			ch = _getch();
			ch = 0;
		}
		if ((options & 0x10) && (isprint(ch))) chOK = 1;
		else {
			if ((options & 0x08) && (ch == '.')) chOK = 1;
			else {
				if ((options & 0x04) && (ch == ' ')) chOK = 1;
				else {
					if ((options & 0x02) && (isalpha(ch))) chOK = 1;
					else {
						if ((options & 0x01) && (isdigit(ch))) chOK = 1;
					}
				}
			}		
		}
		if (chOK) {
			if (nch < sizeStr - 1) {
				putchar(ch);
				pBuffer[nch] = ch;
				nch++;
				pBuffer[nch] = 0;
			}
		}
		else {
			if ((ch == 8) && (nch > 0)) {
				putchar(ch);
				putchar(' ');
				putchar(ch);
				pBuffer[nch] = 0;
				nch--;
			}
			if (ch == 13) ujra = 0;
		}
	} while (ujra == 1);
	if (nch > 0) strncpy_s(str, sizeStr, pBuffer, sizeStr);
	return nch;
}


int InputInt(int* inputI)
{
	int result = 0, valI = 0, nc = 0, len;
	char pBuffer[32];

	do {

		result = InputStr(pBuffer, 32);
		if (result > 0) nc = sscanf_s(pBuffer, "%d", &valI);
		if (nc == 0) {
			len = strnlen(pBuffer, 32);
			for (int i = 0; i < len; i++) putchar(0x08);
		}
	} while (nc == 0);


	if (result > 0) nc = sscanf_s(pBuffer, "%d", &valI);

	*inputI = valI;
	return nc;
}


int InputDouble(double* inputD)
{
	int result = 0, nc = 0, len;
	char pBuffer[32];
	double valD = 0.0;

	do {
		result = InputStr(pBuffer, 32);
		if (result > 0) nc = sscanf_s(pBuffer, "%lf", &valD);
		if (nc == 0) {
			len = strnlen(pBuffer, 32);
			for (int i=0; i<len; i++) putchar(0x08);
		}
	}while (nc == 0);
	*inputD = valD;
	return nc;
}





// return 1 if the file exists, folder is full path specification (not relative!), setfile size and last modification date if pointers are not NULL
int		FileExists(char* pFile, __int64* pSize /*NULL*/, __int64* pTimeMod /*NULL*/)
{
	int result;
	struct _stat64 buffer;

	if (strnlen(pFile, 4) < 3) return 0;
	if (strnlen(pFile, _MAX_PATH) > _MAX_PATH - 1) return 0;
	// if (PathFileExists(CA2CT(pFile)) != 1) return 0;
	result = _stat64(pFile, &buffer);
	if (result) {
		result = errno;
		return 0;
	}
	if (buffer.st_mode & _S_IFDIR) return 0;
	else {
		if (pSize) *pSize = buffer.st_size;
		if (pTimeMod) *pTimeMod = buffer.st_mtime;
		return 1;
	}
}
