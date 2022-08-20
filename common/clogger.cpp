///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

// CLogger.cpp: implementation of the CLogger class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <errno.h>
#include "clogger.h"
#include <time.h>

// #include "CIndexedStringTable.h"
// #include "CFileUtils.h"

#define THIS_FILE "clogger.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogger::CLogger(const char* fileName, const char* basePath, unsigned int numberOfBackups, unsigned int maxFileSize, LOG_PRIORITY priority)
{
  m_fp = NULL;

  m_defaultOperationCountLimit = 0;

  m_timer.start(TRACER_TIMEOUT_LIMIT);

  //inicializa o objeto
  initialize(fileName, basePath, numberOfBackups, maxFileSize, priority);
  //initialize("mxndc.log", basePath, numberOfBackups, maxFileSize, priority);
  //sprintf(m_fullLogName, "%s\\%s.%s", m_basePath, m_fileName, CLOGGER_EXTENSION);

  // m_lastFileName = new CBasicString();
  // m_lastFunctionName = new CBasicString();
  // m_lastLog = new CBasicString();
  // memset(m_isDuplicateMessage, 0, sizeof(m_isDuplicateMessage));
  m_opCount = 0;
}


CLogger::~CLogger()
{
  // if (m_lastFileName)
  // {
  //  delete m_lastFileName;
  //  m_lastFileName = NULL;
  // }
  
  // if (m_lastFunctionName)
  // {
  //  delete m_lastFunctionName;
  //  m_lastFunctionName = NULL;
  // }

  // if (m_lastLog)
  // {
  //  delete m_lastLog;
  //  m_lastLog = NULL;
  // }
}


void CLogger::initialize(const char* fileName, const char* basePath, unsigned int numberOfBackups, unsigned int maxFileSize, LOG_PRIORITY priority)
{
  setFileName(fileName, false);
  setBasePath(basePath, true);
  setNumberOfBackups(numberOfBackups);
  setMaxFileSize(maxFileSize);
  setPriority(priority);
}

void CLogger::logAscii(LOG_PRIORITY priority, const char* format, ...)
{
  FILE* fp;

  if (priority >= m_priority)
  {
    m_logMutex.request();
    fp = loadFile();

    if (fp)
    {
      //devemos pegar data e hora
      char dateHour[48];
      char str[256];
      va_list args;
      
      struct timeb now;
      ftime(&now);
      
      //time_t now;
      //time( &now );

      strftime(dateHour, sizeof (dateHour), "%d/%m/%Y,%H:%M:%S", localtime( &(now.time) ));
      sprintf(str, "%s.%03d,%d,", dateHour, now.millitm, priority);

      va_start( args, format );
      // if (!isDuplicate("", "", format, args))
      {
        fwrite( str, strlen(str), 1, fp);
        vfprintf( fp, format, args );
        fwrite( "\n", 1, 1, fp);
      }

      va_end(args);
      
      unloadFile();
      
    }
    m_logMutex.release();
  }
}


void CLogger::logAscii(const char* fileName, const char* functionName, LOG_PRIORITY priority, const char* format, ...)
{
  FILE* fp;

  if (priority >= m_priority)
  {
    m_logMutex.request();

    fp = loadFile();

    if (fp)
    {
      //devemos pegar data e hora
      char dateHour[48];
      char str[256];
      va_list args;

      struct timeb now;
      ftime(&now);

      strftime(dateHour, sizeof (dateHour), "%d/%m/%Y,%H:%M:%S", localtime( &(now.time) ));
      sprintf(str, "%s.%03d,%s,%s,%d,", dateHour, now.millitm, fileName, functionName, priority);

      va_start( args, format );
      // if (!isDuplicate(fileName, functionName, format, args))
      {
        fwrite( str, strlen(str), 1, fp);
        vfprintf( fp, format, args );
        fwrite( "\n", 1, 1, fp);
      }

      va_end(args);
      unloadFile();
    }
    m_logMutex.release();
  }
}

// void CLogger::logAsciiIfChange(const char* key, const char* fileName, const char* functionName, LOG_PRIORITY priority, const char* format, ...)
// {
//  static CIndexedStringTable hashList;
//  FILE* fp;

//  if (priority >= m_priority)
//  {
//    m_logMutex.request();

//    fp = loadFile();

//    if (fp)
//    {
//      //devemos pegar data e hora
//      char dateHour[48];
//      char str[256];
//      va_list args;

//      struct timeb now;
//      ftime(&now);

//      strftime(dateHour, sizeof (dateHour), "%d/%m/%Y,%H:%M:%S", localtime( &(now.time) ));
//      sprintf(str, "%s.%03d,%s,%s,%d,", dateHour, now.millitm, fileName, functionName, priority);

//      va_start( args, format );
      
//      const char* pHash;
//      int needed = vsnprintf(NULL, 0, format, args) + 1;
//      char* buf = new char[needed + 1];
//      memset (buf, 0, needed + 1);
//      vsnprintf(buf, needed, format, args);

//      char newHash[8 + 1];
//      sprintf (newHash, "%08X", crc32b((unsigned char*)buf));
      

//      pHash = hashList.getValue(key);
//      if (pHash == NULL ||
//        memcmp(pHash, newHash, 8) != 0)
//      {
//        fprintf(fp, str);
//        fprintf(fp, buf);
//        fwrite( "\n", 1, 1, fp);
//        hashList.setValue(key, newHash);
//      }

//      delete[] buf;
//      va_end(args);
//      unloadFile();
//    }
//    m_logMutex.release();
//  }
// }


// void CLogger::vLogAscii(const char* fileName, const char* functionName, LOG_PRIORITY priority, const char* format, va_list args)
// {
//  FILE* fp;

//  if (priority >= m_priority)
//  {
//    m_logMutex.request();

//    fp = loadFile();

//    if (fp)
//    {
//      //devemos pegar data e hora
//      char dateHour[48];
//      char str[256];

//      struct timeb now;
//      ftime(&now);

//      strftime(dateHour, sizeof (dateHour), "%d/%m/%Y,%H:%M:%S", localtime( &(now.time) ));
//      sprintf(str, "%s.%03d,%s,%s,%d,", dateHour, now.millitm, fileName, functionName, priority);

//      // if (!isDuplicate(fileName, functionName, format, args))
//      {
//        fwrite( str, strlen(str), 1, fp);
//        vfprintf( fp, format, args );
//        fwrite( "\n", 1, 1, fp);
//      }

//      unloadFile();
//    }
//    m_logMutex.release();
//  }
// }



void CLogger::logHex(const char* fileName, const char* functionName, LOG_PRIORITY priority, const unsigned char* hexMessage, unsigned int len)
{
  FILE* fp;

  if (priority >= m_priority)
  {
    m_logMutex.request();
    fp = loadFile();

    if (fp)
    {
      //devemos pegar data e hora
      char dateHour[48];
      char str[256];

      struct timeb now;
      ftime(&now);

      strftime(dateHour, sizeof (dateHour), "%d/%m/%Y,%H:%M:%S", localtime( &(now.time) ));
      sprintf(str, "%s.%03d,%s,%s,%d,[", dateHour, now.millitm, fileName, functionName, priority);

      fwrite( str, strlen(str), 1, fp);

      for (unsigned int i = 0; i < len; i++)
      {
        fprintf( fp, "%02X ", hexMessage[i] );
      }

      fwrite( "]\n", 1, 1, fp);

      unloadFile();
    }
    m_logMutex.release();
  }
}

void CLogger::logEsc(const char* fileName, const char* functionName, LOG_PRIORITY priority, int line, const char* asciiHeader, const unsigned char* hexMessage, unsigned int len)
{
  FILE* fp;

  if (priority >= m_priority)                                                               //  Se a prioridade do log está liberada 
  {                                                                                         //  [
    m_logMutex.request();                                                                   //    lock da gravação do log
    fp = loadFile();                                                                        //    abre o arquivo de log

    if (fp) {                                                                               //    Se achou arquivo [
      char dateHour[48];
      char str[256];
      char msgTrunc[] = {" - Truncated...."};
      int hdrLen, newLen;
      struct timeb now;

      ftime(&now);                                                                          //      pega momento atual
      strftime(dateHour, sizeof (dateHour), "%d/%m/%Y,%H:%M:%S", localtime( &(now.time) )); //      Monta data e hora
      hdrLen = snprintf(str, sizeof(str), "%s.%03d,%s,%s,%d, [%d] %s[%d]=[", 
            dateHour, now.millitm, fileName, functionName, priority, line, 
            asciiHeader, len);                                                              //      Monta as outras informações
      if (hdrLen >= (int) sizeof(str)) {                                                    //      Se o buffer não cabe [
        memcpy (&str[sizeof(str)-sizeof(msgTrunc)-1], msgTrunc, sizeof(msgTrunc));          //        Finaliza com " - Truncated...."
        hdrLen = sizeof(str)-1;
      }                                                                                     //      ]
      newLen = hdrLen;

      for (int ii=0; ii < (int) len; ii++) {                                                //      Para todos os bytes dos dados [
        if ((hexMessage[ii] < 0x20) ||(hexMessage[ii] > 0x7E)) {                            //        Se o byte não é imprimível [
          newLen += 4;                                                                      //          Adiciona 4 bytes no novo tamanho (\xNN)
        }                                                                                   //        ]
        else if (hexMessage[ii] == '\\') {                                                  //        Senao [
          newLen +=2;                                                                       //          só incrementa
        }                                                                                   //        ]
        else {                                                                              //        Senao [
          newLen++;                                                                         //          só incrementa
        }                                                                                   //        ]
      }                                                                                     //      ]
      newLen += 3;                                                                          //      Mais um byte para o NULL
      char newBuff[newLen], *pntTmp;
      memcpy(newBuff, str, hdrLen);
      pntTmp = &newBuff[hdrLen];
      for (int ii=0; ii < (int) len; ii++) {                                                //      Para todos os bytes dos dados [
        if ((hexMessage[ii] < 0x20) ||(hexMessage[ii] > 0x7E)) {                            //        Se o byte não é imprimível [
          sprintf(pntTmp,"\\x%02X", hexMessage[ii]);                                        //          Adiciona 4 bytes no novo tamanho (\xNN)
          pntTmp +=4;
        }                                                                                   //        ]
        else if (hexMessage[ii] == '\\') {                                                  //        Senao [
          *pntTmp++ = '\\';
          *pntTmp++ = '\\';
        }                                                                                   //        ]
        else {                                                                              //        Senao [
          *pntTmp++ = hexMessage[ii];
        }                                                                                   //        ]
      }                                                                                     //      ]
      *pntTmp++ = ']';
      *pntTmp++ = '\n';
      *pntTmp = 0;

      fwrite( newBuff, newLen-1, 1, fp);
      
      unloadFile();
    }
    m_logMutex.release();
  }
}


void CLogger::logHex(const char* fileName, const char* functionName, LOG_PRIORITY priority, int line, const char* asciiHeader, const unsigned char* hexMessage, unsigned int len)
{
  FILE* fp;

  if (priority >= m_priority)
  {
    m_logMutex.request();
    fp = loadFile();

    if (fp)
    {
      //devemos pegar data e hora
      char dateHour[48];
      char str[256];

      struct timeb now;
      ftime(&now);

      strftime(dateHour, sizeof (dateHour), "%d/%m/%Y,%H:%M:%S", localtime( &(now.time) ));
      sprintf(str, "%s.%03d,%s,%s,%d, [%d]", dateHour, now.millitm, fileName, functionName, priority, line);

      fwrite( str, strlen(str), 1, fp);
      fprintf( fp, "%s[",  asciiHeader);

      for (unsigned int i = 0; i < len; i++)
      {
        fprintf( fp, "%02X ", hexMessage[i] );
      }

      fwrite( "]\n", 2, 1, fp);
      
      unloadFile();
    }
    m_logMutex.release();
  }
}


FILE* CLogger::loadFile()
{
  //FILE* fp = NULL;

  if(m_fp == NULL)
  {
    //vamos tentar abrir
    m_fp = fopen(m_fullLogName, "a");

    if (m_fp)
    {
      //devemos verificar o tamanho
      if (getFileLength(m_fp) >= m_maxFileSize)
      {
        //devemos fazer backup
        //fechamos o arquivo atual
        fclose(m_fp);

        //fazemos o backup
        doBackup();

        //abrimos o arquivo novamente
        m_fp = fopen(m_fullLogName, "a");
      }
    }
  }
  return m_fp;
}


LOG_PRIORITY CLogger::getPriority()
{
  return m_priority;
}


unsigned int CLogger::getFileLength(FILE* fp)
{
  unsigned int retLen;

  //vamos para o final do arquivo
  fseek(fp, 0, SEEK_END);

  retLen = ftell(fp);
  return retLen;
}


void CLogger::doBackup()
{
  static char bakFullPathNew[CLOGGER_BASEPATH_MAX_LENGHT + CLOGGER_FILENAME_MAX_LENGHT];
  static char bakFullPath[CLOGGER_BASEPATH_MAX_LENGHT + CLOGGER_FILENAME_MAX_LENGHT];

  if (m_numberOfBackups <= 0)
  {
    //neste caso nao faz backup
    remove(m_fullLogName);
  }
  else
  {
    //apaga o mais velho
    sprintf(bakFullPath, "%s/%s%02d.%s", m_basePath, m_fileName, m_numberOfBackups, CLOGGER_BAK_EXTENSION);
    remove(bakFullPath);
  
    //renomeia
    for (int i = m_numberOfBackups - 1; i > 0; i--)
    {
      sprintf(bakFullPath, "%s/%s%02d.%s", m_basePath, m_fileName, i, CLOGGER_BAK_EXTENSION);
      sprintf(bakFullPathNew, "%s/%s%02d.%s", m_basePath, m_fileName, i + 1, CLOGGER_BAK_EXTENSION);
      rename(bakFullPath, bakFullPathNew);
    }
  
    rename(m_fullLogName, bakFullPath);
  }
}


void CLogger::setFileName(const char* fileName)
{
  setFileName(fileName, true);
}

void CLogger::setFileName(const char* fileName, bool updateFullName)
{
  strcpy(m_fileName, fileName);
  if( updateFullName )
  {
    updateFullLogName();
  }
}

void CLogger::setBasePath(const char* basePath)
{
  setBasePath(basePath, true);
}

void CLogger::setBasePath(const char* basePath, bool updateFullName)
{
  int len;
  char aux[CLOGGER_BASEPATH_MAX_LENGHT] = {0};

  strcpy(aux, basePath);
  len = strlen(aux);

  //se tiver / tiramos
  if (aux[len -1] == '/')
  {
    aux[len -1] = '\0';
  }
  strcpy(m_basePath, aux);

  if( updateFullName )
  {
    updateFullLogName();
  }
}

void CLogger::updateFullLogName()
{ 
  memset(m_fullLogName, 0, sizeof(m_fullLogName));
  sprintf(m_fullLogName, "%s/%s.%s", m_basePath, m_fileName, CLOGGER_EXTENSION);
}


void CLogger::setNumberOfBackups(unsigned int number)
{
  m_numberOfBackups = number;
}


void CLogger::setMaxFileSize(unsigned int max)
{
  m_maxFileSize = max;
}


void CLogger::setPriority(LOG_PRIORITY priority)
{
  m_priority = priority;
}

// bool CLogger::isDuplicate(const char* fileName, const char* functionName, 
//              const char* format, va_list args)
// {
//  vsnprintf(m_isDuplicateMessage, sizeof (m_isDuplicateMessage) - 1, format, args);

//  //Verificar se a mensagem anterior é igual a atual
//  if (!m_lastFileName->equals(fileName) ||
//    !m_lastFunctionName->equals(functionName) ||
//    !m_lastLog->equals(m_isDuplicateMessage))
//  {
//    m_lastFileName->clear();
//    m_lastFileName->append(fileName);
    
//    m_lastFunctionName->clear();
//    m_lastFunctionName->append(functionName);
    
//    m_lastLog->clear();   
//    m_lastLog->append(m_isDuplicateMessage);
    
//    return false;
//  }
  
//  return true;
// }


/* This is the basic CRC-32 calculation with some optimization but no
table lookup. The the byte reversal is avoided by shifting the crc reg
right instead of left and by using a reversed 32-bit word to represent
the polynomial.
   When compiled to Cyclops with GCC, this function executes in 8 + 72n
instructions, where n is the number of bytes in the input message. It
should be doable in 4 + 61n instructions.
   If the inner loop is strung out (approx. 5*8 = 40 instructions),
it would take about 6 + 46n instructions. */
unsigned int CLogger::crc32b(unsigned char *message) 
{
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (message[i] != 0) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

void CLogger::unloadFile()
{
  bool closeIt = true;
  if((m_opCount == m_defaultOperationCountLimit)||
    (m_defaultOperationCountLimit > 0 && m_timer.checkTimeout()))
  {
    m_opCount = 0;
    m_timer.start(TRACER_TIMEOUT_LIMIT);
  }
  else
  {
    closeIt = false;
    m_opCount++;
  }

  if(closeIt)
  {
    fclose(m_fp);
    m_fp = NULL;
  }
  
}

bool CLogger::checkPriority(LOG_PRIORITY priority)
{
 return (priority >= m_priority);
}

