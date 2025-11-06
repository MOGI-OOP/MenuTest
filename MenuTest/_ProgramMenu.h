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


