#include "CObjectContainer.h"
#include "CFileUtils.h"

CLogger* CObjectContainer::m_trace = NULL;
CUSBPrinterComm* CObjectContainer::m_comm = NULL;

void CObjectContainer::initialize ()
{
	CObjectContainer::getTraceInstance();
	CObjectContainer::getUsbPrinterCommInstance();
}


CLogger* CObjectContainer::getTraceInstance ()
{
	if (CObjectContainer::m_trace == NULL)
	{
#ifdef WIN32
		if(!CFileUtils::isDirectory("./Logs"))
		{
			CFileUtils::createDirectoryRecursive("./logs/CIS");
		}
		CObjectContainer::m_trace = new CLogger ("ptrEngeworks", "./logs/CIS", 5, 1024 * 1024 * 10, P_TRACE);
#else
		if(!CFileUtils::isDirectory("/opt/Positivo/logs"))
		{
			CFileUtils::createDirectoryRecursive("/opt/Positivo/logs");
		}
		CObjectContainer::m_trace = new CLogger ("ptrEngeworks", "/opt/Positivo/logs", 5, 1024 * 1024 * 10, P_TRACE);
#endif
	}
	return CObjectContainer::m_trace;
}

CUSBPrinterComm* CObjectContainer::getUsbPrinterCommInstance()
{
	if(CObjectContainer::m_comm == NULL)
	{
		CObjectContainer::m_comm = new CUSBPrinterComm();
	}
	return CObjectContainer::m_comm;
}
