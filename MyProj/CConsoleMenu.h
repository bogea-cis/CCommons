#ifndef	_CCONSOLE_MENU_H
#define	_CCONSOLE_MENU_H

typedef struct CMenuStr {
	int iMenuOption;	
	const char *pszMenuDescriptionOption;
	void ( *function)();
} CMenuStr_t;

class CConsoleMenu
{
public:
	CConsoleMenu( CMenuStr_t *menu);
	~CConsoleMenu();
	int getMenuOption();
	void showMenu();
private:
	CMenuStr_t *menu;
	int validOptionNumber;
};

#endif	//	_CCONSOLE_MENU_H

 

	

