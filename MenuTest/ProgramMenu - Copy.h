#pragma once


#define TIMECONV_UTC_UTC 1
#define TIMECONV_UTC_LOCAL 0


typedef struct menuItem
{
	int menuId;
	char menuItemName[32];
} MENU_ITEM;



// Class CTxtFlexLine

class CProgramMenu
{
public:
	// Constructon 
	CProgramMenu();
	~CProgramMenu(void);

	void AddTitle(const char *pTitle);
	int AddMenuItem(const char* pTitle, int retValue = -1);
	int ShowMenu();

private:
	int	m_nMenuitems;
	char m_pMainTitle[32];

	MENU_ITEM* m_pMenuItems;
};




void SetTerminalCursorPos(int x, int y);
int GetTerminalCursorPos(int *x, int *y);
void SetColor(int textColor, int bgColor);

// Read string from console, options: 0x01 number, 0x02 letter, 0x04 ' ',  0x08 '.', 0x10 printable
int InputStr(char* str, int sizeStr, int options=0x10);
// Read double from console,
int InputDouble(double* inputD, bool exitOnEmpty=0);
// Read integer from console,
int InputInt(int* inputI);


// return 1 if the file exists, folder is full path specification (not relative!), setfile size and last modification date if pointers are not NULL
int		FileExists(char* pFile, __int64* pSize /*NULL*/, __int64* pTimeMod /*NULL*/);

//		return 1 if year is a leap year
int LeapYear(int year);

//		returns the number of days in the month, or 0 on error. (January = 1)
int DaysPerMonth(int month, int year);

// Converts ISO string to integers: year, month, day, hour, minute, sec, and milliseonds
int		IsoStr2TimeUnits(char* pChar, int* pTimearray7);
// Converts PC __time64_t to ISO string, returms 0 on OK, or (-) error. Parameters: t: PcTime (-1: current PC time, 0: returns UNKNOWN), format:0 only date 1 : date and time, 2 : date time ms, 11: time only, 101-102:filename compat.   methode 0: localTime, 1: UTC, 2: simple convert
int		PcTime2IsoStr(__int64 t, char* pChar, int sizeChar, int format = 1, int ms  = 0, int methode = TIMECONV_UTC_LOCAL);
// Converts to ISO time string to PC __time64_t. Returns 0 on OK, optionally in pMs>0 returns milliseconds also, mode 0: use localtime, 1: UTC (no timezone and  daylight savings conversion TIMECONV_UTC_UTC)
int		IsoStr2PcTime(char* pChar, __int64* pPcTime, int* pMs, int mode = TIMECONV_UTC_LOCAL);
