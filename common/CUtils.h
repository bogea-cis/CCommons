#ifndef _CTILS_H_
#define _CTILS_H_

#include <stdio.h>
typedef union w2b{
      unsigned char bytes[2];
      unsigned short word;
      } w2b;
      
typedef struct CUtils_timer {
  int start;
  int timeout;
} CUtils_timer;

      

void CUtils_msleep(unsigned long milisec);
void CUtils_start (CUtils_timer *thisTimer, int timeout);
bool CUtils_checkTimeout (CUtils_timer *thisTimer);
int  CUtils_getElapsedTime (CUtils_timer *thisTimer);
int  CUtils_filesize(FILE * fp);
int  CUtils_kbhit( void);
int  CUtils_hexPack(unsigned char * dataBuf, int datalen);
int  CUtils_hexUnpack(unsigned char * dataBuf, int datalen); 
void ConvAsciiToHex( char *pszStr, int lengthStr, unsigned char *pbHex);
void ConvHexToAscii( unsigned char *pbHex, int lengthHex, char *pszStr);

#endif // _CTILS_H_
