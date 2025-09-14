// MenuTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ProgramMenu.h"
#include <windows.h>
#include <conio.h>
#include <stdio.h>


CProgramMenu mainMenu;


int Factorial()
{
	int base, i;
	unsigned long factorial;

	system("cls");
	printf("\n\nFaktoriális számítás\n\n");

	printf("Írjon be egy egész számot: ");
	InputInt(&base);
	printf("\n");
	if (base == 0 || base == 1) {
		printf("Bevitt adat hibás");
		_getch();
		return 0;
	}
	factorial = base;
	for (i = base - 1;i > 1;i--) {
		factorial = factorial * i;
		
	}
	printf("%lu", factorial);

	_getch();
	return 0;
}


int main()
{
	char str[32];
	double dbl;
	int menuSelected = -1;

	system("cls");

	mainMenu.AddTitle("Program menü");

	mainMenu.AddMenuItem("Faktoriális");
	mainMenu.AddMenuItem("Black on White");
	mainMenu.AddMenuItem("White on Black");
	mainMenu.AddMenuItem("Blue on Yellow");
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
		}

	} while (menuSelected!=0);

	system("cls");

}

