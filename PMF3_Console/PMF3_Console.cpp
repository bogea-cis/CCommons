//============================================================================
// Name        : PMF3_Console.cpp
// Author      : L. Bogea
// Version     : 1.01
// Copyright   : Cis Eletronica
// Description : Programa de teste e validacao para impressora PR1000-PMF3-Banese
//============================================================================

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <time.h>

#include <iostream>

#define PAPER_END       0x0080
#define VAL_DOC_ON      0x0040
#define THERMAL_TAMPER  0x0020
#define VAL_TAMPER      0x0010
#define BUSY            0x0008
#define STANDBY         0x0004
#define VAL_ERR         0x0002
#define CUTTER_ERR      0x0001

using namespace std;

typedef HMODULE lib_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

lib_t LoadLib( const char* szLib)
{
	return ::LoadLibraryA( szLib);
}

BOOL UnloadLib( lib_t hLib)
{
	return ::FreeLibrary( hLib);
}

FARPROC LoadProc( lib_t hLib, const char* szProc)
{
	return ::GetProcAddress( hLib, szProc);
}


typedef int( __stdcall *LigaPortaUSB_t)( void);
typedef void( __stdcall *DesligaPortaUSB_t)( void);
typedef unsigned long( __stdcall *Envia_t)( char *buffer, int length);
typedef unsigned int( __stdcall *getStatus_t)( void);

typedef unsigned long( __stdcall *CPImprime_t)( char *buffer, int length);


lib_t hLib = NULL;
LigaPortaUSB_t LigaPortaUSB = NULL;
DesligaPortaUSB_t DesligaPortaUSB = NULL;
Envia_t Envia = NULL;
getStatus_t getStatus = NULL;

CPImprime_t CPImprime = NULL;


/**
 *	Menu de apresentacao
 */
void showMenu( void)
{
	std::cout << "Impressora PR1000" << std::endl;
	std::cout << "1 - Imprime texto longo" << std::endl;
	std::cout << "2 - Le status" << std::endl;
	std::cout << "3 - Autenticar" << std::endl;
	std::cout << "4 - Grafico" << std::endl;
	std::cout << "5 - ABICOMP" << std::endl;
	std::cout << "6 - CODEPAGE 850" << std::endl;
	std::cout << "7 - CODEPAGE 437" << std::endl;
	std::cout << "8 - Recibo autenticacao" << std::endl;
	std::cout << "Q - Sair" << std::endl;
}


uint8_t grfBuf[] = {		//	24 x 3    imprime um quadrado
		0x1b, 0x2a, 0x21, 0x18, 0x00,	// ESC * 33 nL nH 
		0xff, 0xff, 0xff,
		0xff, 0xff, 0xff,
		0xff, 0xff, 0xff,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xc0, 0x00, 0x03,
		0xff, 0xff, 0xff,
		0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0,
};

uint8_t charBuf[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 0,
};

uint8_t logoCis[] = {			// logo da cis em uma linha para autenticacao
    0x1b, 0x4b, 0x14, 0x00,
    (byte)0xff, (byte)0xff, (byte)0xe7, (byte)0xc3,
    (byte)0x81, (byte)0x99, (byte)0x3c, (byte)0x3c, 
    (byte)0xff, (byte)0x20, (byte)0x20, (byte)0xff, 
    (byte)0x3c, (byte)0x3c, (byte)0x9c, (byte)0xcc, 
    (byte)0xc1, (byte)0xe1, (byte)0xf3, (byte)0xff, 
    0x00
};

string recibo1[] = {
	"012345678901234567890123456789012345678901234567\n\0",
	"------------------------------------------------\n\0",
	"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n\0",
	"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n\0",
	"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n\0",
	"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n\0",
	};

string recibo2[] = {
	" **** ATENTICACAO - AUTENTICACAO **** \n\0",
	"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n\0",
	"------------------------------------------------\n\n\0",
	};



uint32_t volatile globalStatus;		// variavel com status da impressora
uint32_t volatile globalStatusRun;	// se 1 mantém capturando status, se 0 encerra a thread.

/**
 *		global status monitor
 *		thread que captura status da impressora a cada 10ms.
 */
void StatusMonitorThread( void *pParams)
{
	while( globalStatusRun) {
		globalStatus = getStatus();
		Sleep( 10);
	}
}
 
 
 
/**
 *		main
 */
int main( void)
{
    uint32_t ui32_Status;

	globalStatus = 0;
	globalStatusRun = 0;
	
	
	//	carregando a dll
	if ((hLib = ::LoadLib("EngW32.dll")))
//	if ((hLib = ::LoadLib("EngW64.dll")))
	{
		if( !( LigaPortaUSB = ( LigaPortaUSB_t)::LoadProc( hLib, "LigaPortaUSB")))
		{
			std::cout << "Function load error: LigaPortaUSB" << std::endl;
			UnloadLib(hLib);
            std::cout << "Pressione uma tecla para sair." << std::endl << std::endl;
            _getch();
			return -2;
		}
		if( !( DesligaPortaUSB = ( DesligaPortaUSB_t)::LoadProc( hLib, "DesligaPortaUSB")))
		{
			std::cout << "Function load error: DesligaPortaUSB" << std::endl;
			UnloadLib(hLib);
            std::cout << "Pressione uma tecla para sair." << std::endl << std::endl;
            _getch();
			return -3;
		}
		if( !( Envia = ( Envia_t)::LoadProc( hLib, "Envia")))
		{
			std::cout << "Function load error: Envia" << std::endl;
			UnloadLib(hLib);
            std::cout << "Pressione uma tecla para sair." << std::endl << std::endl;
            _getch();
			return -4;
		}
		if( !( getStatus = ( getStatus_t)::LoadProc( hLib, "getStatus")))
		{
			std::cout << "Function load error: getStatus" << std::endl;
			UnloadLib(hLib);
            std::cout << "Pressione uma tecla para sair." << std::endl << std::endl;
            _getch();
			return -5;
		}
		if( !( CPImprime = ( CPImprime_t)::LoadProc( hLib, "CPImprime")))
		{
			std::cout << "Function load error: CPImprime" << std::endl;
			UnloadLib( hLib);
			std::cout << "Pressione uma tecla para sair." << std::endl << std::endl;
			_getch();
			return -6;
		}
	}
	else
	{
		std::cout << "USB_Port.dll not found!" << std::endl;
        std::cout << "Pressione uma tecla para sair." << std::endl << std::endl;
        _getch();
		return -1;
	}

	// Inicio da aplicacao
	if( LigaPortaUSB() == 0)
	{
		std::cout << "Impressora desligada ou desconectada.\n" << std::endl;
	}
	else
	{
		showMenu();

		int iFlag = 0;
		
    	globalStatusRun = 1;		//
    	_beginthread( StatusMonitorThread, 0, NULL);	// inicia a thread

		while( 1)
		{        	
		    iFlag = !iFlag;
			if( _kbhit())
			{
				switch( _getch())
				{
				case '1':
					for( int i = 0; i < 10; i++)
					{
						Sleep( 35);
						if( ( globalStatus & 0xff) == 0xAF) 
//						if( ( ui32_Status = ( getStatus() & 0xFF)) == 0xAF)
						{
							Envia( ( char *)"123456789012345678901234567890123456789012345678\n\0", 50);
						}
					}
					Sleep( 35);
					Envia( ( char *)"\n", 1);
					Envia( ( char *)"\x1bm\0", 3);	//corte parcial
					break;
				case '2':
//				    ui32_Status = getStatus();
//				    printf( "\r/    Status = %X ", ui32_Status);
					printf( "\r/	Status = %X ", globalStatus);					
					break;
				case '3':
					Envia( ( char *)"\x1dp1\0", 4);	//autenticacao
//					Envia((char *)"\x1ds1\0", 4);	//autenticacao sublinhado
					Envia( ( char *)logoCis, sizeof( logoCis));
					Envia( ( char *)"123456789012345678901234567890123456789012345678\n\0", 50);
					Sleep( 35);
					break;
		        case '4':
		            Envia( ( char *)grfBuf, sizeof( grfBuf)+1);
		            Envia( ( char *)"\n\n\0", 3);
		            break;
		        case '5':
		            Envia( ( char *)"\x1b\x74\x31\x00", 4);		// ESC w 1 ABICOMP
		            Envia( ( char *)charBuf, sizeof( charBuf));
		            Envia( ( char *)"\n\n\0", 3);
		            break;
		        case '6':
		            Envia( ( char *)"\x1b\x74\x32\x00", 4);		// ESC w 2 CODEPAGE 850
		            Envia( ( char *)charBuf, sizeof( charBuf));
		            Envia( ( char *)"\n\n\0", 3);
		            break;
		        case '7':
		            Envia( ( char *)"\x1b\x74\x33\x00", 4);		// ESC w 3 CODEPAGE 437
		            Envia( ( char *)charBuf, sizeof( charBuf));
		            Envia( ( char *)"\n\n\0", 3);
		            break;
		        case '8':
		        	// imprime primeira parte do recibo...
		        	for( int i = 0; i < 6; i++)
		        	{
		        		Sleep( 35);
						if( ( globalStatus & 0xff) == 0xAF) 
						{
				        	Envia( ( char*)recibo1[i].c_str(), recibo1[i].length() + 1);
//				        	CPImprime( ( char*)recibo1[i].c_str(), recibo1[i].length() + 1);
						}
					}
					// ...imprime logo da cis...
					Sleep( 35);
					if( ( globalStatus & 0xff) == 0xAF)
					{
						Envia( ( char *)logoCis, sizeof( logoCis));
//						CPImprime( ( char *)logoCis, sizeof( logoCis));
					}
					// ...imprime segunda parte do recibo.
		        	for( int i = 0; i < 3; i++)
		        	{
		        		Sleep( 35);
						if( ( globalStatus & 0xff) == 0xAF) 
						{
				        	Envia( ( char*)recibo2[i].c_str(), recibo2[i].length() + 1);
//				        	CPImprime( ( char*)recibo2[i].c_str(), recibo2[i].length() + 1);
						}
					}
					
		        	break;
				case 'q':
				case 'Q':
		        	globalStatusRun = 0;		// Para a thread
					DesligaPortaUSB();
					return 0;
					break;
				default:
					break;
				}
			}
		}
	}

	std::cout << "Pressione uma tecla para sair." << std::endl << std::endl;
	_getch();

	DesligaPortaUSB();
	UnloadLib(hLib);

	return 0;
}
