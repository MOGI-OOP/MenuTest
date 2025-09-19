#pragma once



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
int InputDouble(double* inputD);
// Read integer from console,
int InputInt(int* inputI);


// return 1 if the file exists, folder is full path specification (not relative!), setfile size and last modification date if pointers are not NULL
int		FileExists(char* pFile, __int64* pSize /*NULL*/, __time64_t* pTimeMod /*NULL*/);

