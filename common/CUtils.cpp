// CUtils.cpp: implementation of the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#include <CUtils.h>
#ifdef WINDOWS
#include <windows.h>
#include <conio.h>
#else
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#endif

#include <ctype.h>


#include <time.h>

void CUtils_msleep(unsigned long milisec)  
{  
#ifdef WINDOWS
  Sleep(milisec);
#else
  struct timespec req={0};  
  time_t sec=(int)(milisec/1000);  
  milisec=milisec-(sec*1000);  
  req.tv_sec=sec;  
  req.tv_nsec=milisec*1000000L;  

  while(nanosleep(&req,&req)==-1)  
    continue;  
#endif
} 

void CUtils_start (CUtils_timer *thisTimer,int timeout)
{
  int startTime;
#ifdef WINDOWS
  startTime = GetTickCount();
#else
  struct timeval current;

  gettimeofday (&current, NULL);
  startTime = current.tv_sec * 1000 + current.tv_usec / 1000;
#endif
  thisTimer->start = startTime;
  thisTimer->timeout = timeout;

}

bool CUtils_checkTimeout (CUtils_timer * thisTimer)
{
  int now;

#ifdef WINDOWS
  now = GetTickCount();
#else
  struct timeval current;
  gettimeofday (&current, NULL);
  now = current.tv_sec * 1000 + current.tv_usec / 1000;
#endif
    
  if ((now - thisTimer->start) > thisTimer->timeout)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int CUtils_getElapsedTime (CUtils_timer * thisTimer)
{
  int now;

#ifdef WINDOWS
  now = GetTickCount();
#else
  struct timeval current;

  gettimeofday (&current, NULL);
  now = current.tv_sec * 1000 + current.tv_usec / 1000;
#endif
  
  return now - thisTimer->start;
}

/*========================================================================================
     Fnc: int CUtils::filesize(FILE* fp)
  ===========================================*/ /**

  @brief Retorna tamanho do arquivo indicado por fp

  @param FILE * fp - Filhe handle para o arquivo

  @return tamanho do arquivo.

  @author Ie

  @warning - Apos sua execucao do ponteiro do arquivo esta no final do arquivo.

  */
int CUtils_filesize(FILE* fp)
{
  unsigned int retLen;

  fseek(fp, 0, SEEK_END);
  retLen = ftell(fp);
  return retLen;
}


int CUtils_kbhit( void)
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

int CUtils_hexPack(unsigned char * dataBuf, int datalen) 
{
  int ii;
  datalen = datalen / 2;
  for ( ii = 0; ii < datalen; ii++) {
    unsigned char tmpL = toupper(dataBuf[(ii*2)+1]);
    unsigned char tmpH = toupper(dataBuf[ii*2]);

    if (tmpL > '9') {
      tmpL = tmpL - 'A' + 10;
    }
    else {
      tmpL = tmpL - '0';
    }
    if (tmpH > '9') {
      tmpH = tmpH - 'A' + 10;
    }
    else {
      tmpH = tmpH - '0';
    }
    dataBuf[ii] = (tmpH << 4) + tmpL;
  }
  dataBuf[ii]=0;
  return 0;
}

int CUtils_hexUnpack(unsigned char * dataBuf, int datalen) 
{
  for (int ii = datalen-1; ii >= 0; ii--) {
    unsigned char tmpL = dataBuf[ii] & 0x0F;
    unsigned char tmpH = (dataBuf[ii] & 0xF0) >> 4;
    if (tmpL > 9) {
      tmpL = tmpL - 10 + 'A';
    }
    else {
      tmpL = tmpL + '0';
    }
    if (tmpH > 9) {
      tmpH = tmpH - 10 + 'A';
    }
    else {
      tmpH = tmpH + '0';
    }
    dataBuf[(2*ii)+1] = tmpL;
    dataBuf[(2*ii)] = tmpH;
  }
  return 0;
}

/**____________________________________________________
 * @brief	Convert ASCII stringz into HEX byte array
 * @param	pszStr
 * @param	lengthStr
 * @param	pbHex
 */
void ConvAsciiToHex( char *pszStr, int lengthStr, unsigned char *pbHex)
{
	int i;
	char c;

	for ( i = 0; i < lengthStr; i++) {
		c = pszStr[i];

		if( c >= 97 && c <= 122) { 				// [a(97) - z(122)]
			c = ( c - ( 'a' - 'A'));
		}
		if( ( c >= '0' && c <= '9') || ( c >= 'A' && c <= 'F')) {
			if( c <= '9') {
				c -= '0';
			} else {
				c -= ( 'A' - 10);
			}
			if( ( i & 1) == 0) {
				pbHex[i / 2] = ( c << 4);
			} else {
				pbHex[i / 2] += c;
			}
		} else {
			break;
		}
	}
}


/**___________________________________________________
 * @brief	Convert HEX byte array into ASCII stringz
 * @param	pbHex
 * @param	lengthHex
 * @param	pszStr
 */
void ConvHexToAscii( unsigned char *pbHex, int lengthHex, char *pszStr)
{
    int  i;
    char c;

    for( i = 0; i < lengthHex*2; i++) {
        if( ( i & 1) == 0) {
            c = ( ( pbHex[i / 2]) >> 4);
        } else {
            c = ( ( pbHex[i / 2]) & 15);
        }
        if( c < 10) {
        	pszStr[i] = '0' + c;
        } else {
        	pszStr[i] = 'A' - 10 + c;
        }
    }
    pszStr[lengthHex*2] = '\0';
}
