#ifndef COBJECTCONTAINER_H
#define COBJECTCONTAINER_H

#ifdef LIBUSB10
	#include "CUSBPrinterComm-10.h"
#else
	#include "CUSBPrinterComm-01.h"
#endif
#include "clogger.h"

class CObjectContainer
{
public:
	static void initialize();

	static CLogger* getTraceInstance ();
	static CUSBPrinterComm* getUsbPrinterCommInstance();
	
private:
	static CLogger* m_trace;
	static CUSBPrinterComm* m_comm;
};

#endif /* COBJECTCONTAINER_H */
