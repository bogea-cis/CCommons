#include "CObjectContainer.h"
#include "CFileUtils.h"

CLogger* CObjectContainer::m_trace = NULL;

void CObjectContainer::initialize()
{
  CObjectContainer::getTraceInstance();
}


CLogger* CObjectContainer::getTraceInstance()
{
  if( CObjectContainer::m_trace == NULL )
  {
    if( !CFileUtils::isDirectory( "./logs" ) )
    {
      CFileUtils::createDirectoryRecursive( "./logs" );
    }
    CObjectContainer::m_trace = new CLogger( "logfile", "./logs", 5, 1024 * 1024 * 10, TRACE );
  }
  return CObjectContainer::m_trace;
}

