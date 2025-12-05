

#include <conio.h>
#include <stdio.h>
#include < cstdio >
#include <windows.h>
#include <iostream>
#include <time.h>
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






CDateTime::CDateTime()
{
}


CDateTime::~CDateTime()
{
}

int CDateTime::InitTime(long long timeVal, int ms /*= 0 */)
{
	if (timeVal < -1) return -1;
	if ((ms < 0) || (ms>999)) return -1;
	if (timeVal == -1) InitActualTime();
}


void CDateTime::InitActualTime()
{
	_time64(&m_unixTime);
	m_millisecs = 0;
}

//		return 1 if year is a leap year
int CDateTime::LeapYear(int year)
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
int CDateTime::DaysPerMonth(int month, int year)
{
	int ly = 0;
	int dpm[14] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 9999 };

	if ((year < 1) || (year > 5000)) return 0;
	if ((month < 1) || (month > 12)) return 0;
	if (LeapYear(year)) dpm[2] = 29;
	return dpm[month];
}



// Checks ISO date time format return 0 on error, -1: on YYYY-DDD, 1: on YYYY-MM-DD, -2: on YYYY-DDDThh:mm:ss 2: on YYYY-MM-DDThh:mm:ss, -3: on YYYY-DDDThh:mm:ss.fff 3: on YYYY-MM-DDThh:mm:ss.fff
int CDateTime::ChkIsoTimeFormat(char* pIsoTime)
{
	int i, len, fmt, nH;
	//char *pTime;

	len = (int)strnlen(pIsoTime, 32);
	if ((len > 31) || (len < 8)) return 0;				// length too long or too short
	for (i = 0; i < 4; i++) {
		if (isdigit(pIsoTime[i]) == 0) return 0;		// year error
	}
	if (pIsoTime[4] != '-') return 0;					// year separator maust be a '-'
	if ((isdigit(pIsoTime[5]) == 0) || (isdigit(pIsoTime[6]) == 0)) return 0;		// month or day error
	if (len == 8) {							// YYYY-DDD
		if (isdigit(pIsoTime[7]) == 0) return 0;
		fmt = -1;
		return fmt;
	}
	else {
		if (len == 9) return 0;
		if (len == 10) {					// YYYY-MM-DD
			if ((pIsoTime[7] == '-') && isdigit(pIsoTime[8]) && isdigit(pIsoTime[9])) return 1;
			else return 0;
		}
		if (pIsoTime[8] == 'T') {			// YYYY-DDDT...
			fmt = -1;
			if (isdigit(pIsoTime[7]) == 0) return 0;
			nH = 9;
		}
		else {
			if ((pIsoTime[7] != '-') || (isdigit(pIsoTime[8]) == 0) || (isdigit(pIsoTime[9]) == 0)) return 0;
			fmt = 1;						// YYYY-MM-DDT...
			nH = 11;
		}
	}
	// YYYY-MM-DDThh:mm:ss
	if ((pIsoTime[nH + 2] != ':') && (pIsoTime[nH + 2] != '.') && (pIsoTime[nH + 2] != '-')) return 0;		// check minute separator
	if ((pIsoTime[nH + 5] != ':') && (pIsoTime[nH + 5] != '.') && (pIsoTime[nH + 5] != '-')) return 0;		// sec  separator
	if (isdigit(pIsoTime[nH + 0]) == 0) return 0;
	if (isdigit(pIsoTime[nH + 1]) == 0) return 0;
	if (isdigit(pIsoTime[nH + 3]) == 0) return 0;
	if (isdigit(pIsoTime[nH + 4]) == 0) return 0;
	if (isdigit(pIsoTime[nH + 6]) == 0) return 0;
	if (isdigit(pIsoTime[nH + 7]) == 0) return 0;
	//  no fractonal seconds
	if (pIsoTime[nH + 8] == 0) return 2 * fmt;
	if (pIsoTime[nH + 9] == 0) {
		if ((pIsoTime[nH + 8] == 'Z') || (pIsoTime[nH + 8] == 'z')) return 2 * fmt;
		else return 0;
	}
	if (pIsoTime[nH + 8] != '.') return 0;
	if (isdigit(pIsoTime[nH + 9]) == 0) return 0;
	if (isdigit(pIsoTime[nH + 10]) == 0) return 0;
	if (isdigit(pIsoTime[nH + 11]) == 0) return 0;
	if (pIsoTime[nH + 12] == 0) return 3 * fmt;
	if (pIsoTime[nH + 13] == 0) {
		if ((pIsoTime[nH + 12] == 'Z') || (pIsoTime[nH + 12] == 'z')) return 3 * fmt;
		else return 0;
	}
	if (isdigit(pIsoTime[nH + 12]) == 0) return 0;
	if (pIsoTime[nH + 13] == 0) return 3 * fmt;
	if (isdigit(pIsoTime[nH + 13]) == 0) return 0;
	if (pIsoTime[nH + 14] == 0) return 3 * fmt;
	if (isdigit(pIsoTime[nH + 14]) == 0) return 0;
	if (pIsoTime[nH + 15] == 0) return 3 * fmt;
	return 0;
}


int CDateTime::PcTime2IsoStr(char *pTimeStr, int sizeChar, int format /* = 1 */, int methode /* = TIMECONV_UTC_LOCAL */)
{
	int err;
	struct tm   newTime;
	char pTime[32];

	if (m_unixTime < 0) return -1;
	if (methode) err = _gmtime64_s(&newTime, &m_unixTime);		// TIMECONV_UTC_UTC  = 1
	else err = _localtime64_s(&newTime, &m_unixTime);
	if (err) return -3;
	switch (format) {
	case 0:	sprintf_s(pTime, 32, "%04d-%02d-%02d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday);
		break;
	case 1:	sprintf_s(pTime, 32, "%04d-%02d-%02dT%02d:%02d:%02d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
		break;
	case 2:		sprintf_s(pTime, 32, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec, m_millisecs);
		break;
		// for filename: yymmddThhmmss
	case 3:		sprintf_s(pTime, 32, "%02d%02d%02dT%02d%02d%02d", (newTime.tm_year + 1900) - 2000, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
		break;
		// for filename: yymmddThhmmssfff
	case 33:	sprintf_s(pTime, 32, "%02d%02d%02dT%02d%02d%02d%03d", (newTime.tm_year + 1900) - 2000, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec, m_millisecs);
		break;
	case 11:	sprintf_s(pTime, 32, "%02d:%02d:%02d", newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
		break;
	case 12:	sprintf_s(pTime, 32, "%02d:%02d:%02d.%03d", newTime.tm_hour, newTime.tm_min, newTime.tm_sec, m_millisecs);
		break;
	case 101:	sprintf_s(pTime, 32, "%04d-%02d-%02dT%02d-%02d-%02d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
		break;
	case 102:	sprintf_s(pTime, 32, "%04d-%02d-%02dT%02d-%02d-%02d.%03d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec, m_millisecs);
		break;
	default:	sprintf_s(pTime, 32, "%04d-%02d-%02dT%02d:%02d:%02d", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
	}
	return 0;
}


// Converts to ISO time string to PC __time64_t. Returns 0 on OK, optionally in pMs>0 returns milliseconds also, mode 0: use localtime, 1: UTC (no timezone and  daylight savings conversion)
int CDateTime::IsoStr2PcTime(char* pChar, int mode /* = TIMECONV_UTC_LOCAL*/)
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

	m_unixTime = t0;
	m_millisecs = ms;
	return 0;
}


// Converts ISO string to integers: year, month, day, hour, minute, sec, and milliseonds
int CDateTime::IsoStr2TimeUnits(char* pChar, int* pYear, int* pMonth, int* pDay, int* pHour, int* pMin, int* pSec, int* pMs)
{
	int fmt, nc, len, cdx, repeat;
	int cye = 0, cmo = 0, cda = 0, cdy = 0, chr = 0, cmi = 0, cse = 0, cms = 0;
	char timeStr[32], * pTm;

	fmt = ChkIsoTimeFormat(pChar);
	if (fmt == 0) return -1;
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


