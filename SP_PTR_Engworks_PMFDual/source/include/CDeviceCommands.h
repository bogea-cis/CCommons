#ifndef CDEVICECOMMANDS_H_
#define CDEVICECOMMANDS_H_

#ifdef LIBUSB10
	#include "CUSBPrinterComm-10.h"
#else
	#include "CUSBPrinterComm-01.h"
#endif

class CDeviceCommands
{
public:
	CDeviceCommands();
	virtual ~CDeviceCommands();

	//Comandos Gerais
	int initialize();

	int validationMode(int operation);
	int documentMode(int operation);
	
	int blankLines(int numberOfBlankLines);
	int formFeed();
	int lineFeed();
	int carriageReturn();
	int characterAlignment(int position);
	int pageSize(int rows);

	int enableDisableCarriageReturn(int operation);

	//Comandos de Texto
	int setCharacterTable(int characterTable);
	int setUnderline(int operation);
	int setItalic(int operation);
	int setBold(int operation);
	int setSuperSubscript(int operation);
	int setNumberOfColumns(int operation);
	int setDoubleWidthChars(int operation);
	int setDoubleHeightChars(int operation);
	int cancelLastLine();
	int deleteLastChar();

	int setItalicForValidationMode(int operation);
	int setUnderlineForValidationMode(int operation);

	int cut(int cutType);

	int printData(unsigned char* data, int dataLen);
	int sendEtx();

private:
	CUSBPrinterComm* m_comm;
};

#endif /*JOBINTERFACEWRAPPER_H_*/
