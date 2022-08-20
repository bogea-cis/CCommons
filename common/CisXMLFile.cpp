#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>

#include "CXMLFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

CXMLFile::CXMLFile( const char *szFileName)
{
  m_pcszValue = new char[255];
  m_pcszFileName = new char[MAX_NAME_SIZE];
  memset( m_pcszFileName, 0, MAX_NAME_SIZE);
  memcpy( m_pcszFileName, szFileName, strlen( szFileName));
}

CXMLFile::~CXMLFile()
{
  delete m_pcszValue;
	delete m_pcszFileName;
}
