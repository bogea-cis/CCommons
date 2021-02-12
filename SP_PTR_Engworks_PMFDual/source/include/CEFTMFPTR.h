#ifndef _CEF_TMF_PTR_H_
#define _CEF_TMF_PTR_H_

#include "CEFTMF.h"

#ifdef WINDOWS
#include <windows.h>
#else
#include <string.h>
#endif

// Acesso a Impressora/Autenticadora
// API do Banco do Brasil
//

/*-----------------------------------------------------------------------*/
/* PTR Device                                                            */
/*-----------------------------------------------------------------------*/

/* PTR Status Mask */
#define CEF_TMF_PTR_OUTOFPAPER			0x0001 // End Of Paper
#define CEF_TMF_PTR_POWEROFF			0x0002 // Printer Power Off
#define CEF_TMF_PTR_IOERROR				0x0004 // I/O Hardware Error
#define CEF_TMF_PTR_NOTREADY			0x0008 // Printer Not Ready
#define CEF_TMF_PTR_OPENCOVER			0x0010 // Printer Cover Open
#define CEF_TMF_PTR_DOCINSERTED			0x0020 // Document Inserted
#define CEF_TMF_PTR_OK					0x00F0 // Printer is Ready

/* IoControl Commands */
#define CEF_TMF_PTR_IO_ENABLEETX        1
#define CEF_TMF_PTR_IO_DISABLEETX       2

/*-----------------------------------------------------------------------*/
/* PTR Functions Prototypes */
/*-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
    {
#endif
    int CEFTMFAPI CEF_TMF_PTR_Initialize();

    int CEFTMFAPI CEF_TMF_PTR_Terminate();

    int CEFTMFAPI CEF_TMF_PTR_Reset();

    int CEFTMFAPI CEF_TMF_PTR_PrintLine(unsigned char * Data, 
                                        unsigned int DataLength);

    int CEFTMFAPI CEF_TMF_PTR_PrintDoc(unsigned char * Data, 
                                       unsigned int DataLength);

    int CEFTMFAPI CEF_TMF_PTR_GetStatus();
	
	int CEFTMFAPI CEF_TMF_PTR_PrintRawData(unsigned char * Data, 
                                       unsigned int DataLength);

    int CEFTMFAPI CEF_TMF_PTR_DocumentInserted();

    int CEFTMFAPI CEF_TMF_PTR_IoControl(int Command, void * pArg);

    int CEFTMFAPI CEF_TMF_PTR_Who(void * pArg);

    int CEFTMFAPI CEF_TMF_PTR_ReceiptCut(int uiMode);

	void CEF_TMF_PTR_UpdateLastStatus(unsigned char* status);

#ifdef __cplusplus
    }
#endif


#endif  // _CEF_TMF_PTR_H_
