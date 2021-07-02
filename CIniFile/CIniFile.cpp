#include <iostream>

#include "CIniFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

CIniFile::CIniFile( char *szFileName)
{
  m_szFileName = new char[MAX_NAME_SIZE];
  memset( m_szFileName, 0, MAX_NAME_SIZE);
  memcpy( m_szFileName, szFileName, strlen( szFileName));
}

CIniFile::~CIniFile()
{
	delete m_szFileName;
}

void CIniFile::WriteInteger( char* szSection, char* szKey, int iValue)
{
  char szValue[255];
  sprintf( szValue, "%d", iValue);
  if( !PutIniKeyString( szSection, szKey, szValue, m_szFileName)) {
    std::cout << "m_szFileName=" << m_szFileName << std::endl;
  }
}

void CIniFile::WriteFloat( char* szSection, char* szKey, float fValue)
{
  char szValue[255];
  sprintf( szValue, "%f", fValue);
  if( !PutIniKeyString( szSection, szKey, szValue, m_szFileName)) {
    
  }
}

void CIniFile::WriteBoolean( char* szSection, char* szKey, bool bValue)
{
  char szValue[255];
  sprintf( szValue, "%s", bValue ? "true" : "false");
  if( !PutIniKeyString( szSection, szKey, szValue, m_szFileName)) {
    
  }
}

void CIniFile::WriteString( char* szSection, char* szKey, char* szValue)
{
  if( !PutIniKeyString( szSection, szKey, szValue, m_szFileName)) {
    
  }
}

int CIniFile::ReadInteger( char* szSection, char* szKey, int iDefaultValue)
{
  char szValue[255];
  int iValue = iDefaultValue;
  if( !GetIniKeyString( szSection, szKey, (char*)szValue, m_szFileName)) {
    iValue = atoi( szValue);
  }
  return iValue;
}

float CIniFile::ReadFloat( char* szSection, char* szKey, float fDefaultValue)
{
  char szValue[255];
  float fValue = fDefaultValue;
  if( !GetIniKeyString( szSection, szKey, (char*)szValue, m_szFileName)) {
    fValue = atof( szValue);
  }
  return fValue;
}

bool CIniFile::ReadBoolean( char* szSection, char* szKey, bool bDefaultValue)
{
  char szValue[255];
  bool bValue = bDefaultValue;
  if( !GetIniKeyString( szSection, szKey, (char*)szValue, m_szFileName)) {
    bValue = ( strcmp( (char*)szValue, "True") == 0 || strcmp( (char*)szValue, "true") == 0) ? true : false;
  }
  return bValue;
}

char* CIniFile::ReadString( char* szSection, char* szKey, char* szDefaultValue)
{
  char *szValue = new char[255];
  if( GetIniKeyString( szSection, szKey, (char*)szValue, m_szFileName)) {
    strcpy( szValue, szDefaultValue);
  }
  return szValue;  
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
int CIniFile::GetIniKeyString( char *title, char *key, char *buf, char *filename)  
{  
  FILE *fp;  
  int  flag = 0;  
  char sTitle[64], *wTmp;
  char sLine[1024];
  
  sprintf( sTitle, "[%s]", title);
                   
  if( NULL == ( fp = fopen( filename, "r"))) {  
      perror( "fopen");  
      return -1;
  }
  while( NULL != fgets( sLine, 1024, fp)) {  
                                                                                  // This is the comment line  
      if( 0 == strncmp( "//", sLine, 2))  continue;  
      if( '#' == sLine[0])                continue;        
      wTmp = strchr( sLine, '=');  
      if( ( NULL != wTmp) && ( 1 == flag)) {  
        if( 0 == strncmp( key, sLine, strlen( key))) {                            // The length depends on the file read  
          sLine[strlen( sLine) - 1] = '\0';  
          fclose( fp);
          while( *( wTmp + 1) == ' ') {
            wTmp++;
          }
          strcpy( buf,wTmp + 1);
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
int CIniFile::PutIniKeyString( char* title, char* key, char* val, char* filename)
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



