/**
  @file CisMenus.cpp

  @brief Classe de implementação de menu texto.

  @details Esta classe implementa a criação de menu para execução de funções de testes.
          Basicamente, cria-se o objeto do menu e adiciona-se os itens do menu:
            tecla de acionamento da opção
            texto a identificar a operação
            função a ser chamada.
          Depois de adicionar os itens, basta executar o menu pelo método runMenu.
          Este método exibirá o menu e aguardará a seleção da operação.
          Se a função for Nula, a tecla provoca o abandono do Menu

  @ingroup GrUtils

  @author CIS/Ie

  @version 1.000
  History\n

  @todo

  @warning

  @mainpage 

  @image html cis.png

     */
#ifdef WINDOWS
#include <Windows.h>
#include <conio.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

#include "CisMenus.h"
#include "CTimeUtils.h"

/**
 * @fn      int lkbhit()
 * @brief   implementação para linux da funcao kbhit()
 * @param   none
 * @return  none
 */
static int lkbhit( void);


/*========================================================================================
     Fnc: CisMenus::CisMenus( int numColsSet, int colSizeSet, const char * menuTitleSet)
  ===========================================*/ /**

  @brief Constructor

  @details Inicia os parâmetros do menu

  @param int numColsSet - Número de colunas do menu
  @param int colSizeSet - Largura da coluna da descrição
  @param const char * menuTitleSet - Tìtulo do menu

  @return none

  @author Ie

  @warning

  */
CisMenus::CisMenus( int numColsSet, int colSizeSet, const char * menuTitleSet) {
  this->CisMenusSet(false, numColsSet, colSizeSet, menuTitleSet);
}

CisMenus::CisMenus( bool enterMode, int numColsSet, int colSizeSet, const char * menuTitleSet) {
  this->CisMenusSet( enterMode, numColsSet, colSizeSet, menuTitleSet);
}

void CisMenus::CisMenusSet( bool enterMode, int numColsSet, int colSizeSet, const char * menuTitleSet)
{
  m_enterMode = enterMode;
  if (colSizeSet > MAX_MENU_DESC) {                                                         //  Se a largura da coluna é maior que o tamanho máximo [
    printf( "Tamanho de coluna ajustado para %d\n", MAX_MENU_DESC);                         
    colSizeSet = MAX_MENU_DESC;                                                             //    Limita o tamanho máximo
  }                                                                                         //  ]
  
  if (strlen(menuTitleSet) > MAX_TITLE_SIZE) {                                              //  Se o título excede o máximo [
    printf( "Tamanho de título ajustado para %d\n", MAX_TITLE_SIZE);
    memcpy(menuTitle, menuTitleSet, MAX_TITLE_SIZE);                                        //    Limita o título
    menuTitle[MAX_TITLE_SIZE] = 0;
  }                                                                                         //  ]
  else {                                                                                    //  Senao [
    strcpy(menuTitle, menuTitleSet);                                                        //    Copia o título
  }                                                                                         //  ]

  colSize = colSizeSet;                                                                     //  Inicia tamanho da coluna
  numCols = numColsSet;                                                                     //  Inicia numero de colunas
  numItems = 0;                                                                             //  Inicia numero de itens do menu
  menuItems = NULL;                                                                         //  Inicia ponteiro de itens do Menu
}

CisMenus::~CisMenus()
{
  delete menuItems;
}


/*========================================================================================
     Fnc: int CisMenus::addMenuItem( char itKey, const char * itDesc, void (*fnc)(void)) 
  ===========================================*/ /**

  @brief Adiciona itens no menu

  @details

  @param char itKey - tecla de acionamento da opção
  @param const char * itDesc - Descrição do item
  @param void (*fnc)(void) - função a executar - NULL significa que deixa o menu

  @return none

  @author Ie

  @warning

  */
int CisMenus::addMenuItem( char itKey, const char * itDesc, void (*fnc)(void)) 
{
  int ii;
  char tmpBuff[MAX_MENU_DESC+1];
  for (ii=0; ii < numItems; ii++) {                                                         //  Para todos os itens do Menu [
    if (menuItems[ii].itemKey == itKey ) {                                                  //    Se a tecla a incluir coincide [
      printf("Item com tecla repetida!\n");                                                 //      Exibe mensagem de erro
      return -1;                                                                            //      retorna erro de tecla repetida
    }                                                                                       //    ]
  }                                                                                         //  ]

  memset(tmpBuff, ' ', colSize);
  tmpBuff[colSize] = 0;                                                                     //  Cria linha base com espaços
  if ((int) strlen(itDesc) > colSize) {                                                     //  Se a deescrição é maior que o buffer
    printf("Descricao excede buffer - %s", itDesc);
    memcpy(tmpBuff, itDesc, colSize);                                                       //    Copia apenas o que cabe
  }                                                                                         //  ]
  else {                                                                                    //  Senão [
    memcpy(tmpBuff, itDesc, strlen(itDesc));                                                //    Copia apenas o texto (para deixar espaços até o fim)
  }                                                                                         //  ]


  numItems++;                                                                               //  Incrementa numero de ítens
  st_menuItem * tmpItems = new st_menuItem[numItems];                                       //  Cria um novo array de itens com o novo tamanho
  for (ii=0; ii < numItems-1; ii++) {                                                       //  Para todos os itens antigos [
    tmpItems[ii] = menuItems[ii];                                                           //    Copia o elemento para o novo array
  }                                                                                         //  ]
  tmpItems[ii].itemKey = itKey;                                                             //  Atualiza o último elemento com a tecla nova
  memcpy(tmpItems[ii].itemDesc, tmpBuff, MAX_MENU_DESC+1);                                  //  Copia a descrição
  tmpItems[ii].itemFn = fnc;                                                                //  Copia a função
  if (menuItems != NULL) {                                                                  //  Se tinha menu anteriormente [
    delete menuItems;                                                                       //    apaga o objeto
  }                                                                                         //  ]
  menuItems = tmpItems;                                                                     //  Atualiza o menu com o novo array
  return 0;                                                                                 //  Retorna sucesso
}

/*========================================================================================
     Fnc: void CisMenus::runMenu()
  ===========================================*/ /**

  @brief Executa o menu

  @details  Fica em loop experando a seleção da função a executar.
            Antes de executar, verifica se tem uma opção de saída.

  @param none

  @return none

  @author Ie

  @warning

  */
void CisMenus::runMenu()
{
  if (showMenu() != 0) {                                                                    //  Exibe o menu
    printf("Menu nao tem opcao de saida!!\n");
    return;
  }
  while (true) {                                                                            //  Loop eterno [
    char opt;
    if (m_enterMode == false) {                                                             //    Se esta em modo kbhit [
      if (lkbhit()) {                                                                       //      Se alguma tecla foi pressionada [
        int ii;
#ifdef WINDOWS
        opt = _getch();
#else
        opt = getchar();                                                                    //        Pega a tecla
#endif
        for (ii = 0; ii < numItems; ii++) {                                                 //        Para todas os itens do Menu [
          if (menuItems[ii].itemKey == opt) {                                               //          Se a tecla coincide [
            if (menuItems[ii].itemFn) {                                                     //            Se tem função definida []
              printf("-%s\n", menuItems[ii].itemDesc);
              menuItems[ii].itemFn();                                                       //              Executa a função
              break;                                                                        //              Encerra o loop
            }                                                                               //            ]
            else {                                                                          //            Senao [
              printf ("-Leaving menu \"%s\"\n", menuTitle);                                 //            Mostra msg de saida
              return;                                                                       //              finaliza o menu
            }                                                                               //            ]
          }                                                                                 //          ]
        }                                                                                   //        ]
        if (ii == numItems) {                                                               //        Se chegou ao final da lista [
          printf("\nInvalid Option\n");                                                     //          Indica opção invalida
        }                                                                                   //        ]
        showMenu();                                                                         //        Exibe o menu
      }                                                                                     //      ]
      #ifdef WINDOWS
        Sleep( 50);
      #else
        usleep( 50 * 1000);                                                                 //      Sleep
      #endif
    }                                                                                       //    ]
    else {                                                                                  //    Senao [ // modo ENTER
      char tmpBuf[10];
      char * tmpPnt;
      scanf("%s", tmpBuf);                                                                  //      Pega o comando a executar
      opt = tmpBuf[0];
      if (opt == '\n') {                                                                    //      Se é um LF [
        continue;                                                                           //        despreaza a entrada (Isto era usado com o fgets)
      }                                                                                     //      ]
      int ii;
      for (ii = 0; ii < numItems; ii++) {                                                   //      Para todas os itens do Menu [
        if (menuItems[ii].itemKey == opt) {                                                 //        Se a tecla coincide [
          if (menuItems[ii].itemFn) {                                                       //          Se tem função definida [
            printf("-%s\n", menuItems[ii].itemDesc);
            menuItems[ii].itemFn();                                                         //            Executa a função
            break;                                                                          //            Encerra o loop
          }                                                                                 //          ]
          else {                                                                            //          Senao [
            printf ("Leaving menu %s\n", menuTitle);                                        //            Mostra msg de saida
            return;                                                                         //            finaliza o menu
          }                                                                                 //          ]
        }                                                                                   //        ]
      }                                                                                     //      ]
      if (ii == numItems) {                                                                 //      Se chegou ao final da lista [
        printf("\nInvalid Option\n");                                                       //        Indica opção invalida
      }                                                                                     //      ]
      showMenu();                                                                           //      Exibe o menu
    }                                                                                       //    ]
  }                                                                                         //  ]
}

/*========================================================================================
     Fnc: void CisMenus::showMenu()
  ===========================================*/ /**

  @brief Mostra o menu de opções

  @details

  @param none

  @return 0 - Tem item de saída, -1 não tem item de saída

  @author Ie

  @warning

  */
int CisMenus::showMenu()
{
  char emptyCol[MAX_MENU_DESC+1];
  int iRet = -1;
  int iNumLinhas = (int) ceil((double)numItems/(double)numCols);                            //  Determina o numero de linhas do menu

  printf("\n%s\n", menuTitle);                                                              //  Imprime titulo

  memset(emptyCol, '-', sizeof(emptyCol));                                                  //  Monta linha de separação
  emptyCol[colSize] = 0;
  for (int jj=0; jj < numCols; jj++) {
    printf( "+-----%s-", emptyCol);
  }
  printf( "+\n");

  memset(emptyCol, ' ', sizeof(emptyCol));                                                  //  Monta string para opção vazia
  emptyCol[colSize] = 0;
  for (int ii=0; ii<iNumLinhas; ii++) {                                                     //  Para todas as linhas [
    for (int jj=0; jj < numCols; jj++) {                                                    //    Para todas as colunas [
      int itemNum = ii*numCols+jj;                                                          //     Calcula o item relativo
      if (itemNum < numItems) {                                                             //      Se o item é válido [
        if (menuItems[itemNum].itemFn == NULL) {
          iRet = 0;
        }
        printf( "| %c - %s ", menuItems[itemNum].itemKey, menuItems[itemNum].itemDesc);     //        Exibe item
      }                                                                                     //      ]
      else {                                                                                //      Senao [
        printf( "|     %s ", emptyCol);                                                     //        Exibe item vazio
      }                                                                                     //      ]
    }                                                                                       //    ]
    printf("|\n");                                                                          //    Pula linha
  }                                                                                         //  ]
  memset(emptyCol, '-', sizeof(emptyCol));                                                  //  Monta linha de separação
  emptyCol[colSize] = 0;
  for (int jj=0; jj < numCols; jj++) {
    printf( "+-----%s-", emptyCol);
  }
  printf( "+\n");
  printf("Option: ");                                                    //  Exibe msg de opção
  fflush(stdout);
  return iRet ;
}

/**
 * @fn      int lkbhit()
 * @brief   implementacao para linux da funcao kbhit()
 * @param   none
 * @return  none
 */
int lkbhit( void)
{
#ifdef WINDOWS
  return _kbhit();
#else
  struct termios term, term2;
  tcgetattr( 0, &term);
  
  term2 = term;
  term2.c_lflag &= ~ICANON;
  tcsetattr( 0, TCSANOW, &term2);
  
  int byteswaiting;
  ioctl( 0, FIONREAD, &byteswaiting);
  
  tcsetattr( 0, TCSANOW, &term);
  
  return byteswaiting;
#endif
}

