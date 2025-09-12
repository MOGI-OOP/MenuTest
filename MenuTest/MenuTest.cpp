// MenuTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ProgramMenu.h"
#include <windows.h>
#include <conio.h>
#include <stdio.h>


CProgramMenu mainMenu;


void Factorial()
{
	int base;

	system("cls");
	printf("\n\nFaktori�lis sz�m�t�s\n\n");

	printf("�rjon be egy eg�sz sz�mot: ");
	InputInt(&base);


	printf("Kov�cs G�bor\n");

	// K�sz�tse el a sz�m�t�st �s az eredm�ny ki�r�s�t


	_getch();
}


int main()
{
	char str[32];
	double dbl;
	int menuSelected = -1;

	system("cls");

	mainMenu.AddTitle("Program men�");

	mainMenu.AddMenuItem("Faktori�lis");
	mainMenu.AddMenuItem("Black on White");
	mainMenu.AddMenuItem("White on Black");
	mainMenu.AddMenuItem("Blue on Yellow");
	mainMenu.AddMenuItem("Kil�p�s", 0);

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

