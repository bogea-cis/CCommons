#include "CConsoleMenu.h"

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>


CConsoleMenu::CConsoleMenu( CMenuStr_t *menu)
{
	validOptionNumber = 0;
	this->menu = menu;	
}

CConsoleMenu::~CConsoleMenu()
{
}

void CConsoleMenu::showMenu()
{
	validOptionNumber = 0;
	while( menu[validOptionNumber].iMenuOption > 0) {
    	std::cout << menu[validOptionNumber].iMenuOption << "-" << menu[validOptionNumber].pszMenuDescriptionOption << std::endl;
    	validOptionNumber++;
	}
}

int CConsoleMenu::getMenuOption()
{
	int option = 0;
	std::cin >> option;

    if( std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore();      // for values greater than 2147483647 (0x7FFFFFFF), std::cin may crash
    }
    
	if( option <= 0 || option > 1000) {
        showMenu();
        return 0;
    }
	
	for( int i = 0; i < validOptionNumber; i++) {
	    if( option == menu[i].iMenuOption) {
            if( menu[option-1].function != NULL) menu[option-1].function();
            else return option;
        }
    }	
	showMenu();
	return option;
}
