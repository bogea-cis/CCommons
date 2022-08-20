#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>

#include "CIniFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>


CIniFile::CIniFile( const char *szFileName)
{
  m_pcszValue = new char[255];
  m_pcszFileName = new char[MAX_NAME_SIZE];
  memset( m_pcszFileName, 0, MAX_NAME_SIZE);
  memcpy( m_pcszFileName, szFileName, strlen( szFileName));
}

CIniFile::~CIniFile()
{
  delete m_pcszValue;
	delete m_pcszFileName;
}

/*========================================================================================
     Fnc: void CIniFile::WriteInteger( const char* szSection, const char* szKey, int iValue)
  ===========================================*/ /**

  @brief  

  @details

  @param szSection    file section e.g.[Section] 
  @param szKey        key word for attribute
  @param iValue       integer value to key set

  @return none

  @author L.Bogea

  @warning

  */
void CIniFile::WriteInteger( const char* szSection, const char* szKey, int iValue)
{
  sprintf( m_pcszValue, "%d", iValue);
  if( !PutIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
 
   }
}

/*========================================================================================
     Fnc: void CIniFile::WriteFloat( const char* szSection, const char* szKey, float fValue)
  ===========================================*/ /**

  @brief

  @details

  @param szSection    file section e.g.[Section] 
  @param szKey        key word for attribute
  @param fValue       float value to key set

  @return none

  @author L.Bogea

  @warning

  */
void CIniFile::WriteFloat( const char* szSection, const char* szKey, float fValue)
{
  sprintf( m_pcszValue, "%f", fValue);
  if( !PutIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
    
  }
}

/*========================================================================================
     Fnc: void CIniFile::WriteBoolean( const char* szSection, const char* szKey, bool bValue)
  ===========================================*/ /**

  @brief

  @details

  @param szSection    file section e.g.[Section] 
  @param szKey        key word for attribute
  @param bValue       boolean value to key set

  @return none

  @author L.Bogea

  @warning

  */
void CIniFile::WriteBoolean( const char* szSection, const char* szKey, bool bValue)
{
  sprintf( m_pcszValue, "%s", bValue ? "true" : "false");
  if( !PutIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
    
  }
}

/*========================================================================================
     Fnc: void CIniFile::WriteString( const char* szSection, const char* szKey, char* szValue)
  ===========================================*/ /**

  @brief

  @details

  @param szSection    file section e.g.[Section] 
  @param szKey        key word for attribute
  @param szValue      string value to key set

  @return none

  @author L.Bogea

  @warning

  */
void CIniFile::WriteString( const char* szSection, const char* szKey, const char* szValue)
{
  if( !PutIniKeyString( szSection, szKey, szValue, m_pcszFileName)) {
    
  }
}

/*========================================================================================
     Fnc: int CIniFile::ReadInteger( const char* szSection, const char* szKey, int iDefaultValue)
  ===========================================*/ /**

  @brief  get value attributed to a specific [Section] and key=

  @details

  @param szSection      file section e.g.[Section] 
  @param szKey          key word attributed
  @param iDefaultValue  default value if none value was found

  @return integer value from key

  @author L.Bogea

  @warning

  */
int CIniFile::ReadInteger( const char* szSection, const char* szKey, int iDefaultValue)
{
  int iValue = iDefaultValue;
  if( !GetIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
    iValue = atoi( m_pcszValue);
  }
  return iValue;
}


int CIniFile::ReadHexInt( const char* szSection, const char* szKey, int iDefaultValue)
{
  int iValue = iDefaultValue;
  if( !GetIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
    iValue = 0;
    int iLen = strlen(m_pcszValue);
    for (int ii=0; ii < iLen; ii++) {
      char thisVal = toupper(m_pcszValue[ii]);
      if ( (thisVal >= '0') && (thisVal <= '9') ) { 
        iValue = iValue * 16 + thisVal - '0';
      }
      else if ( (thisVal >= 'A') && (thisVal <= 'F') ) { 
        iValue = iValue * 16 + thisVal - 'A' + 10;
      }
    }
  }
  return iValue;
}


/*========================================================================================
     Fnc: float CIniFile::ReadFloat( const char* szSection, const char* szKey, float fDefaultValue)
  ===========================================*/ /**

  @brief  get value attributed to a specific [Section] and key=

  @details

  @param szSection      file section e.g.[Section] 
  @param szKey          key word attributed
  @param fDefaultValue  default value if none value was found

  @return float value from key

  @author L.Bogea

  @warning

  */
float CIniFile::ReadFloat( const char* szSection, const char* szKey, float fDefaultValue)
{
  float fValue = fDefaultValue;
  if( !GetIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
    fValue = atof( m_pcszValue);
  }
  return fValue;
}

/*========================================================================================
     Fnc: bool CIniFile::ReadBoolean( const char* szSection, const char* szKey, bool bDefaultValue)
  ===========================================*/ /**

  @brief  get value attributed to a specific [Section] and key=

  @details

  @param szSection      file section e.g.[Section] 
  @param szKey          key word attributed
  @param bDefaultValue  default value if none value was found

  @return boolean value from key

  @author L.Bogea

  @warning

  */
bool CIniFile::ReadBoolean( const char* szSection, const char* szKey, bool bDefaultValue)
{
  bool bValue = bDefaultValue;
  if( !GetIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
    bValue = ( strcmp( (char*)m_pcszValue, "True") == 0 || strcmp( (char*)m_pcszValue, "true") == 0) ? true : false;
  }
  return bValue;
}

/*========================================================================================
     Fnc: char* CIniFile::ReadString( const char* szSection, const char* szKey, char* szDefaultValue)
  ===========================================*/ /**

  @brief  get value attributed to a specific [Section] and key=

  @details

  @param szSection        file section e.g.[Section] 
  @param szKey            key word attributed
  @param bszDefaultValue  default value if none value was found

  @return string value from key

  @author L.Bogea

  @warning

  */
char* CIniFile::ReadString( const char* szSection, const char* szKey, const char* szDefaultValue)
{
  if( GetIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
    strcpy( m_pcszValue, szDefaultValue);
  }
  return m_pcszValue;  
}

/**
 * @brief
 * @param szSection
 * @param szKey
 * @param szDefaultValue
 * @return 
 */
char* CIniFile::ReadHexData( const char* szSection, const char* szKey, const char* szDefaultValue)
{
  int i = 0;
  strcpy( m_pcszValue, szDefaultValue);
  if( !GetIniKeyString( szSection, szKey, m_pcszValue, m_pcszFileName)) {
    int dataLength = strlen( m_pcszValue) / 2;
    for( i = 0; i < dataLength; i++) {
      unsigned char tmpL = toupper( m_pcszValue[(i * 2) + 1]);
      unsigned char tmpH = toupper( m_pcszValue[i * 2]);

      if( tmpL > '9') {
        tmpL = tmpL - 'A' + 10;
      }
      else {
        tmpL = tmpL - '0';
      }
      if( tmpH > '9') {
        tmpH = tmpH - 'A' + 10;
      }
      else {
        tmpH = tmpH - '0';
      }
      m_pcszValue[i] = (tmpH << 4) + tmpL;
    }
      m_pcszValue[i]=0;
  }
  return m_pcszValue;  
}

/**
 * @fn      GetIniKeyString
 * @brief   Get the key value from file section
 * @param   title - file section
 * @param   key   - key id
 * @param   buf   - key value
 * @param   filename
 * @return  0 = ok, -1 error
 */    
int CIniFile::GetIniKeyString( const char *title, const char *key, char *buf, const char *filename)  
{  
  FILE *fp;  
  int  flag = 0;  
  char sTitle[64], *wTmp;
  char sLine[1024];
  
  sprintf( sTitle, "[%s]", title);
                   
  if( NULL == ( fp = fopen( filename, "r"))) {  
      //perror( "fopen");  
      return -1;
  }
  while( NULL != fgets( sLine, 1024, fp)) {  
      for (int ii = strlen(sLine-1); ii>=0; ii--) {
        if (isspace(sLine[ii])) {
          sLine[ii] = 0;
        }
        else {
          break;
        }
      }
      if( 0 == strncmp( "//", sLine, 2))  continue;  
      if( '#' == sLine[0])                continue;        
      wTmp = strchr( sLine, '=');
      if( ( NULL != wTmp) && ( 1 == flag)) {  
        if( 0 == strncmp( key, sLine, strlen( key))) {                            // The length depends on the file read  
          fclose( fp);
          while( *( wTmp + 1) == ' ') {
            wTmp++;
          }
          strcpy( buf,wTmp + 1);
          int strSize = strlen(buf);
          while (strSize > 0) {
            if (isspace(buf[strSize-1])) {
              strSize--;
              buf[strSize] = 0;
            }
            else {
              break;
            }
          }
          return 0;  
        }  
      } else {  
        if( 0 == strncmp( sTitle, sLine, strlen( sTitle))) {                      // The length depends on the file read  
          flag = 1;                                                               // find the title location  
        }  
      }  
  }  
  fclose(fp);  
  return -1;  
}        
      
/**
 * @fn        PutIniKeyString
 * @brief     Put the key value on file section
 * @param     title
 * @param     key
 * @param     val
 * @param     filename
 * @return    0 = ok, -1 = error
 */
int CIniFile::PutIniKeyString( const char* title, const char* key, const char* val, const char* filename)
{
  FILE *fdr, *fdw;
  char line[1024];
  bool bTitle = false;
  bool bKey = false;
  char *pKey;
  
  if( ( fdr = fopen( filename, "r")) == NULL) {
    return -1;
  }
  
  if( ( fdw = fopen( "tmp1", "w")) == NULL) {
    return -1;
  }
  
  while( ( fgets( line, 1024, fdr)) != NULL)
  {
    if( bTitle == false) {
      if( ( strstr( line, title)) == NULL) {
        bTitle = false;
        fputs( line, fdw);
        continue;
      } else {
        bTitle = true;
        fputs( line, fdw);
        continue;
      }
    } else {
      if( ( pKey = strstr( line, key)) == NULL) {
        fputs( line, fdw);
        continue;
      } else {
        pKey = strchr( line, '=');
        sprintf( pKey, "=%s        //\n", val);
        fputs( line, fdw);
        bKey = true;
      }
    }
  }
  
  if( bTitle == false) {
    sprintf( line, "[%s]\n", title);
    fputs( line, fdw);
  }
  if( bKey == false) {
    sprintf( line, "%s=%s        //\n", key, val);
    fputs( line, fdw);
  }
  fclose( fdr);
  fclose( fdw);
  rename( filename, "tmp2");
  rename( "tmp1", filename);
  remove( "tmp2");
  remove( "tmp1");
  return 0;
}


/**
 * @brief Limpa espaços na string
 * @param string
 * @param length
 */
void CIniFile::killSpaces( char* string, int length)
{
  for( int i = 0, pos = 0; i < length; i++, pos++) {
    if (string[pos] == ' ') pos++;
    string[i] = string[pos];
  }
}


