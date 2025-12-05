#pragma once


#define TIMECONV_UTC_UTC 1
#define TIMECONV_UTC_LOCAL 0


// Set Console cursor position
void SetTerminalCursorPos(int x, int y);
// Get Console cursor position
int GetTerminalCursorPos(int* x, int* y);
// Set console output colors
void SetColor(int textColor, int bgColor);

// Read string from console, options: 0x01 number, 0x02 letter, 0x04 ' ',  0x08 '.', 0x10 printable
int InputStr(char* str, int sizeStr, int options = 0x10);
// Read double from console,
int InputDouble(double* inputD, bool exitOnEmpty = 0);
// Read integer from console,
int InputInt(int* inputI, bool exitOnEmpty = 0);

// find subsrtring in field separated string, Return 0 on success, or error code
int		GetStringField(char* pChOriginal, int nField, char* pBuffer, int sizeBuffer, char separator = 9);
// find subsrtring 'field' in string, and convert to integer. Return 0 on success, or error code
int		GetStringFieldInt(char* pChOriginal, int nField, char separator, int* pInt);
// find subsrtring 'field' in string, and convert to int64. Return 0 on success, or error code
int		GetStringFieldInt64(char* pChOriginal, int nField, char separator, __int64* pInt64);
// find subsrtring 'field' in string, and convert to double. Return 0 on success, or error code
int		GetStringFieldDouble(char* pChOriginal, int nField, char separator, double* pdbl);

// return 1 if the file exists, folder is full path specification (not relative!), setfile size and last modification date if pointers are not NULL
int		FileExists(char* pFile, __int64* pSize = nullptr /*NULL*/, __int64* pTimeMod = nullptr /*NULL*/);



class CDateTime
{
public:
	CDateTime();
	~CDateTime();
	int InitTime(long long timeVal, int ms=0);
	void InitActualTime();

	int LeapYear(int year);

	int DaysPerMonth(int month, int year);

	int ChkIsoTimeFormat(char* pIsoTime);
	int PcTime2IsoStr(char* pTimeStr, int sizeChar, int format=1, int methode = TIMECONV_UTC_LOCAL);
	int IsoStr2TimeUnits(char* pChar, int* pYear, int* pMonth, int* pDay, int* pHour, int* pMin, int* pSec, int* pMs);
	int IsoStr2PcTime(char* pChar, int mode);



	long long	m_unixTime;		// seconds since Jan 1 1970, 00:00:00
	int			m_millisecs;

};

