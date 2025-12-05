// MenuTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include "_ProgramMenu.h"
#include "_InputUtils.h"

CProgramMenu mainMenu;


void Factorial()
{
	int base;
	__int64 fact=1;

	system("cls");
	printf("\n\nFaktoriális számítás\n\n");
	printf("Írjon be egy egész számot: ");
	InputInt(&base);
	for (int i = 2; i <= base; i++) fact *= i;
	printf("\n\nFaktoriális: %lli ", fact);
	int ch =_getch();
}


int main()
{
	char str[32], ch;
	int result, menuSelected = -1;

	system("cls");

	mainMenu.AddTitle("Program menü");

	mainMenu.AddMenuItem("Faktoriális");
	mainMenu.AddMenuItem("Black on White");
	mainMenu.AddMenuItem("White on Black");
	mainMenu.AddMenuItem("Blue on Yellow");
	mainMenu.AddMenuItem("String bevitel");
	mainMenu.AddMenuItem("Kilépés", 0);

	do {
		menuSelected = mainMenu.ShowMenu();
		switch (menuSelected) {
		case 1: Factorial();
			break;
		case 2 : SetColor(0, 15);
			break;
		case 3: SetColor(15, 0);
			break;
		case 4: SetColor(1, 14);
			break;
		case 5: system("cls");
				printf("\n\nDátum string bevitel\n\n");
				printf("Írjon be egy stringet: ");
				result = InputStr(str, 32);
				if (result > 0) printf("\nA beírt string: %s\n", str);
				else printf("\nNem sikerült a beolvasás!\n");
				ch = _getch();
				break;
		}

	} while (menuSelected!=0);

	system("cls");

}

