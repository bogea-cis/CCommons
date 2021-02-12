#include <iostream>

#include "CConsoleMenu.h"


#define EXIT_OPTION 99

void FirstTest( void) {
    std::cout << "First choice" << std::endl;
}

void SecondTest( void) {
	std::cout << "Second choice" << std::endl;
}

void ThirdTest( void) {
    std::cout << "Third choice" << std::endl;
}

void ForthTest( void) {
	std::cout << "Forth choice" << std::endl;
}

void FifthTest( void) {
    std::cout << "Fifth choice" << std::endl;
}

void SixthTest( void) {
	std::cout << "Sixth choice" << std::endl;
}

void SeventhTest( void) {
    std::cout << "Seventh choice" << std::endl;
}

void EighthTest( void) {
	std::cout << "Eighth choice" << std::endl;
}

void NinthTest( void) {
    std::cout << "Ninth choice" << std::endl;
}

void TenthTest( void) {
	std::cout << "Tenth choice" << std::endl;
}

void FirstFirstTest( void) {
    std::cout << "FirstFirst choice" << std::endl;
}

void FirstSecondTest( void) {
	std::cout << "FirstSecond choice" << std::endl;
}

void SecondFirstTest( void) {
    std::cout << "SecondFirst choice" << std::endl;
}

void SecondSecondTest( void) {
	std::cout << "SecondSecond choice" << std::endl;
}

CMenuStr_t firstMenu[] = {
	{	1, "First choice", FirstTest,
	},
	{	2, "Second choice", SecondTest,
	},
	{	3, "Third choice", ThirdTest,
	},
	{	4, "Forth choice", ForthTest,
	},
	{	5, "Fifth choice", FifthTest,
	},
	{	6, "Sixth choice", SixthTest,
	},
	{	7, "Seventh choice", SeventhTest,
	},
	{	8, "Eighth choice", EighthTest,
	},
	{	9, "Ninth choice", NinthTest,
	},
	{	10, "Tenth choice", TenthTest,
	},
	{	EXIT_OPTION, "Exit", NULL,
	},
	{	0, NULL, NULL,
	},
};

CMenuStr firstFirstMenu[] = {
    { 1, "FirstFirst choice", FirstFirstTest,
    },
    { 2, "FirstSecond choice", FirstSecondTest,
    },
    { EXIT_OPTION, "Exit", NULL,
    },
    { 0, NULL, NULL,
    },
};

CMenuStr secondFirstMenu[] = {
    { 1, "SecondFirst choice", SecondFirstTest,
    },
    { 2, "SecondSecond choice", SecondSecondTest,
    },
    { EXIT_OPTION, "Exit", NULL,
    },
    { 0, NULL, NULL,
    },
};


int main(int argc, char** argv) {
	
	int option = 0;
	CConsoleMenu menu = CConsoleMenu( firstMenu);
	CConsoleMenu menu1 = CConsoleMenu( firstFirstMenu);
	CConsoleMenu menu2 = CConsoleMenu( secondFirstMenu);
	
	menu.showMenu();
	
	while( ( option = menu.getMenuOption()) != EXIT_OPTION) {
	    switch( option) {
	    case 1:
	        menu1.showMenu();
	        while( menu1.getMenuOption() != EXIT_OPTION);
	        break;
	    case 2:
	        menu2.showMenu();
	        while( menu2.getMenuOption() != EXIT_OPTION);
	        break;
	    default:
	        break;
        }
        menu.showMenu();
    }
	return 0;
}

