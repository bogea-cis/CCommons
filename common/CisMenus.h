#ifndef _CISMENUS_H_
#define _CISMENUS_H_

#define MAX_MENU_DESC     80
#define MAX_TITLE_SIZE    80

typedef struct st_menuItem {
  char itemKey;
  char itemDesc[MAX_MENU_DESC+1];
  void (* itemFn) (void);
} st_menuItem;

// typedef struct st_menuInfo{
//   int numItems;
//   int colSize;
//   int numCols;
//   st_menuItem * mItems;
// } st_menuInfo;

class CisMenus
{
public:

  CisMenus( int numColsSet, int colSizeSet, const char * menuTitleSet);
  CisMenus( bool enterMode, int numColsSet, int colSizeSet, const char * menuTitleSet);
  virtual ~CisMenus();

  void CisMenusSet( bool enterMode, int numColsSet, int colSizeSet, const char * menuTitleSet);
  int addMenuItem( char iKey, const char * iDesc, void (*fnc)(void)) ;
  void runMenu();

private:
  int showMenu();

protected:

  int numItems;
  int colSize;
  int numCols;
  bool m_enterMode;
  char menuTitle[MAX_TITLE_SIZE+1];
  st_menuItem * menuItems;

};

#endif