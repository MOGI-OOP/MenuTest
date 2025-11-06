

#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "Shlwapi.h"
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "_InputUtils.h"

#define TXT_MAX_LINE_SIZE	8192    // max. linesize including term 0


void SetTerminalCursorPos(int x, int y)
{
	COORD coord;

	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	printf("");
}

int GetTerminalCursorPos(int* x, int* y)
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
	str[0] = 0;
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


int InputInt(int* inputI, bool exitOnEmpty /*=0*/)
{
	int result = 0, valI = 0, nc = 0, len;
	char pBuffer[32];

	do {
		result = InputStr(pBuffer, 32);
		if ((result == 0) && (exitOnEmpty)) return 0;
		if (result > 0) nc = sscanf_s(pBuffer, "%d", &valI);
		if (nc == 0) {
			len = strnlen(pBuffer, 32);
			for (int i = 0; i < len; i++) putchar(0x08);
		}
	} while (nc == 0);
	//if (result > 0) nc = sscanf_s(pBuffer, "%d", &valI);
	*inputI = valI;
	return nc;
}


int InputDouble(double* inputD, bool exitOnEmpty /*=0*/)
{
	int result = 0, nc = 0, len;
	char pBuffer[32];
	double valD = 0.0;

	do {
		result = InputStr(pBuffer, 32);
		if ((result == 0) && (exitOnEmpty)) return 0;
		if (result > 0) nc = sscanf_s(pBuffer, "%lf", &valD);
		if (nc == 0) {
			len = strnlen(pBuffer, 32);
			for (int i = 0; i < len; i++) putchar(0x08);
		}
	} while (nc == 0);
	*inputD = valD;
	return nc;
}


// find subsrtring 'field' in string, and copy to pBuffer. Return 0 on success, or error code
int		GetStringField(char* pChOriginal, int nField, char* pBuffer, int sizeBuffer, char separator)
{
	int i, len, ps = -1, nf = 0, pf = 0, ns;
	char* pCh0, * pCh1;
	__int32 lenF = 0;

	if ((nField < 0) || (nField > 1024) || (sizeBuffer < 1) || (separator < 1)) return -101;
	if ((pChOriginal == NULL) || (pBuffer == NULL)) return -102;
	len = strnlen(pChOriginal, TXT_MAX_LINE_SIZE);
	if (len >= TXT_MAX_LINE_SIZE)  return -103;
	// count separators, find the requested one
	ns = 0;
	for (i = 0; i < len; i++) {
		if (pChOriginal[i] == separator) {
			ns++;
			if (ns == nField) pf = i;
		}
	}
	pBuffer[0] = 0;
	if (ns < nField) return -104;
	if (nField == 0) {
		pCh0 = pChOriginal;
		pCh1 = strchr(pChOriginal, separator);
		if (pCh1) lenF = (int)(pCh1 - pCh0);
		else lenF = len;
	}
	else
	{
		pCh0 = pChOriginal + pf + 1;
		pCh1 = strchr(pCh0, separator);
		if (pCh1) {
			lenF = (int)(pCh1 - pCh0);
		}
		else
		{
			lenF = strlen(pCh0);
		}
	}
	if (sizeBuffer < lenF + 1) return -112;
	for (i = 0; i < lenF; i++) {
		pBuffer[i] = pCh0[i];
	}
	pBuffer[lenF] = 0;
	return 0;
}


// find subsrtring 'field' in string, and convert to integer. Return 0 on success, or error code
int		GetStringFieldInt(char* pChOriginal, int nField, char separator, int* pInt)
{
	int	result, nc, valI;
	char pBuffer[64];

	result = GetStringField(pChOriginal, nField, pBuffer, 64, separator);
	if (result) return result;
	nc = sscanf_s(pBuffer, "%d", &valI);
	if (nc == 0) return -1;
	*pInt = valI;
	return 0;
}

// find subsrtring 'field' in string, and convert to int64. Return 0 on success, or error code
int		GetStringFieldInt64(char* pChOriginal, int nField, char separator, __int64* pInt64)
{
	int	result, nc;
	char pBuffer[64];
	__int64 valI64;

	result = GetStringField(pChOriginal, nField, pBuffer, 64, separator);
	if (result) return result;
	nc = sscanf_s(pBuffer, "%I64d", &valI64);
	if (nc == 0) return -1;
	*pInt64 = valI64;
	return 0;
}

// find subsrtring 'field' in string, and convert to double. Return 0 on success, or error code
int GetStringFieldDouble(char* pChOriginal, int nField, char separator, double* pdbl)
{
	int	result, nc;
	char pBuffer[64];
	double valD;

	result = GetStringField(pChOriginal, nField, pBuffer, 64, separator);
	if (result) return result;
	nc = sscanf_s(pBuffer, "%lf", &valD);
	if (nc == 0) return -1;
	*pdbl = valD;
	return 0;
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
