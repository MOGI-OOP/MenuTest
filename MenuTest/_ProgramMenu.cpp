

#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "Shlwapi.h"
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "_ProgramMenu.h"
#include "_InputUtils.h"

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

