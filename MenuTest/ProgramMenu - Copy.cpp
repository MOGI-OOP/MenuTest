

#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "Shlwapi.h"
#include "ProgramMenu.h"
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "_InputUtils.h"

CProgramMenu::CProgramMenu()
{
	m_nMenuitems = 0;
	m_pMainTitle[0] = 0;
	m_pMenuItems = NULL;
	SetConsoleOutputCP(1250);
	memset(m_pMainTitle, 0, 32);
	// setlocale(LC_CTYPE, "hu-HU");

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


// Converts PC __time64_t to ISO string, returms 0 on OK, or (-) error. Parameters: t: PcTime (-1: current PC time, 0: returns UNKNOWN), format:0 only date 1 : date and time, 2 : date time ms, 11: time only, 101-102:filename compat.   methode 0: localTime, 1: UTC, 2: simple convert
int PcTime2IsoStr(__int64 t, char* pChar, int sizeChar, int format /* = 1 */, int ms /* = 0 */, int methode /* = TIMECONV_UTC_LOCAL */)
{
	struct tm   newTime;
	__int64 t0 = 0;
	char tstr[32] = "Unknown";
	int err = 0, ms1 = 0;

	ms1 = ms;
	if (sizeChar < 1) {
		sprintf_s(tstr, "Error");
		return -11;
	}
	pChar[0] = 0;
	//if (t == 0) err = -1;
	if (t < -1) {
		switch (t) {
		case -2:	sprintf_s(tstr, "Invalid");
			break;
		case -3:	sprintf_s(tstr, "Unknown");
			break;
		default:sprintf_s(tstr, "Error");
		}
		err = -1;
	}
	else {
		if (t < -1) {			// invalid PCTime
			err = -2;
			sprintf_s(tstr, "Error");
			strncpy_s(pChar, sizeChar, "Invalid", sizeChar);
		}
		else {
			if (t == -1) {			// invalid PCTime, but return current time
				_time64(&t0);
				if ((format == 2) || (format == 12) || (format == 102)) ms = clock() % 1000;
			}
			else t0 = t;
			if (methode) err = _gmtime64_s(&newTime, &t0);		// TIMECONV_UTC_UTC  = 1
			else err = _localtime64_s(&newTime, &t0);
			if (err) {
				err = -3;
				sprintf_s(tstr, "Error");
			}
		}
	}
	if (err == 0) {
		switch (format) {
		case 0:	sprintf_s(tstr, "%04d-%02d-%02d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday);
			break;
		case 1:	sprintf_s(tstr, "%04d-%02d-%02dT%02d:%02d:%02d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
			break;
		case 2:		sprintf_s(tstr, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec, ms1);
			break;
			// for filename: yymmddThhmmss
		case 3:		sprintf_s(tstr, "%02d%02d%02dT%02d%02d%02d", (newTime.tm_year + 1900) - 2000, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
			break;
			// for filename: yymmddThhmmssfff
		case 33:	sprintf_s(tstr, "%02d%02d%02dT%02d%02d%02d%03d", (newTime.tm_year + 1900) - 2000, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec, ms1);
			break;
		case 11:	sprintf_s(tstr, "%02d:%02d:%02d", newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
			break;
		case 12:	sprintf_s(tstr, "%02d:%02d:%02d.%03d", newTime.tm_hour, newTime.tm_min, newTime.tm_sec, ms1);
			break;
		case 101:	sprintf_s(tstr, "%04d-%02d-%02dT%02d-%02d-%02d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
			break;
		case 102:	sprintf_s(tstr, "%04d-%02d-%02dT%02d-%02d-%02d.%03d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec, ms1);
			break;
		default:	sprintf_s(tstr, "%04d-%02d-%02dT%02d:%02d:%02d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
		}
	}
	if (sizeChar <= (int)strnlen(tstr, 32)) return -11;
	strncpy_s(pChar, sizeChar, tstr, 32);
	return err;
}


// Converts to ISO time string to PC __time64_t. Returns 0 on OK, optionally in pMs>0 returns milliseconds also, mode 0: use localtime, 1: UTC (no timezone and  daylight savings conversion)
int IsoStr2PcTime(char* pChar, __int64* pPcTime, int* pMs, int mode /* = TIMECONV_UTC_LOCAL*/)
{
	struct tm   newTime;
	int fmt, ye, mo, da, hr, mi, sec, ms;
	__time64_t t0;

	fmt = IsoStr2TimeUnits(pChar, &ye, &mo, &da, &hr, &mi, &sec, &ms);
	if (fmt) return -2;
	newTime.tm_year = ye - 1900;
	newTime.tm_mon = mo - 1;
	newTime.tm_mday = da;
	newTime.tm_hour = hr;
	newTime.tm_min = mi;
	newTime.tm_sec = sec;
	newTime.tm_isdst = 1;

	if (mode) t0 = _mkgmtime64(&newTime);
	else t0 = _mktime64(&newTime);
	if (t0 < 0) return -101;

	*pPcTime = t0;
	if (pMs) *pMs = ms;
	return 0;
}


//		return 1 if year is a leap year
int LeapYear(int year)
{
	int ly = 0;

	if (year < 0) return ly;
	if (year % 4 == 0) {		// every 4. leap year
		ly = 1;
		if (year % 100 == 0) {  // every 100. not leapyear
			ly = 0;
			if (year % 400 == 0) ly = 1; // every 400. again leapyear
		}
	}
	return ly;
}

//		returns the number of days in the month, or 0 on error. (January = 1)
int DaysPerMonth(int month, int year)
{
	int ly = 0;
	int dpm[14] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 9999 };

	if ((year < 1) || (year > 5000)) return 0;
	if ((month < 1) || (month > 12)) return 0;
	if (LeapYear(year)) dpm[2] = 29;
	return dpm[month];
}


// Converts ISO string to integers: year, month, day, hour, minute, sec, and milliseonds
int IsoStr2TimeUnits(char* pChar, int* pYear, int* pMonth, int* pDay, int* pHour, int* pMin, int* pSec, int* pMs)
{
	int fmt, nc, len, cdx, repeat;
	int cye = 0, cmo = 0, cda = 0, cdy = 0, chr = 0, cmi = 0, cse = 0, cms = 0;
	char timeStr[32], * pTm;

	//fmt = ChkIsoTimeFormat(pChar);
	//if (fmt == 0) return -1;
	memset(timeStr, 0, 32);
	strncpy_s(timeStr, pChar, 31);
	_strupr_s(timeStr);
	len = (int)strnlen(timeStr, 63);
	if (timeStr[len - 1] == 'Z') timeStr[len - 1] = 0;		// remove zulu 
	// get year
	timeStr[4] = 0;
	nc = sscanf_s(timeStr, "%d", &cye);
	if ((nc != 1) || (cye < 0) || (cye > 5000)) return -11;
	if (LeapYear(cye)) cdx = 366;
	else cdx = 365;
	if (fmt > 0) {
		timeStr[7] = 0;
		timeStr[10] = 0;
		nc = sscanf_s(timeStr + 5, "%d", &cmo);
		if ((nc != 1) || (cmo < 1) || (cmo > 12)) return -12;
		nc = sscanf_s(timeStr + 8, "%d", &cda);
		if ((nc != 1) || (cda < 1) || (cda > DaysPerMonth(cmo, cye))) return -13;
		pTm = timeStr + 11;
	}
	else {
		timeStr[8] = 0;
		nc = sscanf_s(timeStr + 5, "%d", &cdy);
		if ((nc != 1) || (cdy < 1) || (cdy > cdx)) return -13;
		cmo = 1;
		if (cdy > DaysPerMonth(cmo, cye)) repeat = 1;
		else repeat = 0;
		while (repeat > 0) {
			cdy -= DaysPerMonth(cmo, cye);
			cmo++;
			if (cdy <= DaysPerMonth(cmo, cye)) repeat = 0;
			if (cmo > 12) repeat = -1;
		}
		if (repeat) return -12;
		if (cdy > DaysPerMonth(cmo, cye)) return -13;
		cda = cdy;
		pTm = timeStr + 9;
	}
	if (abs(fmt) > 1) {
		pTm[2] = 0;
		pTm[5] = 0;
		pTm[8] = 0;
		nc = sscanf_s(pTm, "%d", &chr);
		if ((nc != 1) || (chr < 0) || (chr > 23)) return -14;
		nc = sscanf_s(pTm + 3, "%d", &cmi);
		if ((nc != 1) || (cmi < 0) || (cmi > 59)) return -15;
		nc = sscanf_s(pTm + 6, "%d", &cse);
		if ((nc != 1) || (cse < 0) || (cse > 59)) return -16;
		if (abs(fmt) > 2) {
			*(pTm + 12) = 0;
			nc = sscanf_s(pTm + 9, "%d", &cms);
			if ((nc != 1) || (cms < 0) || (cms >= 1000)) return -17;
		}
	}
	*pYear = cye;
	*pMonth = cmo;
	*pDay = cda;
	if (pHour)	*pHour = chr;
	if (pMin)	*pMin = cmi;
	if (pSec)	*pSec = cse;
	if (pMs)	*pMs = cms;
	return 0;
}

