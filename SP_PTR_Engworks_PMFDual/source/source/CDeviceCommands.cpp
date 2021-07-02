#include "CDeviceCommands.h"
#include "CBuffer.h"
#include "CObjectContainer.h"
#include "PTRConstants.h"
#include "CEFTMF.h"

#define THIS_FILE			"CDeviceCommands.cpp"

CDeviceCommands::CDeviceCommands()
{
	CObjectContainer::initialize();
	m_comm = CObjectContainer::getUsbPrinterCommInstance();
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CDeviceCommands", P_TRACE, "Driver initialized");
}

CDeviceCommands::~CDeviceCommands()
{
	m_comm = NULL;
	delete m_comm;
}

int CDeviceCommands::initialize()  
{
	int ret = 0;
	CBuffer data;
	data.setBuffer("\x1B\x40");

	if(m_comm->initialize() == UPC_SUCCESS)
	{
		/**
		 *	comunicacao usb e tratamento do retorno
		 *	testar se com um novo initialize os parametros internos da impressora resetam
		 */
		if(m_comm->open(0x1781, 0x0E88) == UPC_SUCCESS)
		{
			if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
			{
				ret = CEFTMF_OK;
			}
			else
			{
				CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "initialize", P_HIGH, "Error sending initialize command");
				ret = CEFTMF_ERR;
			}
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "initialize", P_HIGH, "Error opening usb connection");
			m_comm->finalize();
			ret = CEFTMF_ERR;
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "initialize", P_HIGH, "Error initializing usb connection");
		ret = CEFTMF_ERR;
	}

	return ret;
}

int CDeviceCommands::validationMode(int operation)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1D\x70");
	data.append((unsigned char)operation);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "validationMode", P_HIGH, "Error sending validationMode command");
		ret = CEFTMF_ERR;
	}

	return ret;
}

int CDeviceCommands::documentMode(int operation)
{
	int ret = 0;
	
	CBuffer data;
	if(operation == DOCUMENT_MODE_OFF)	data.setBuffer("\x1B\x4F");
	else								data.setBuffer("\x1B\x4E");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "documentMode", P_HIGH, "Error sending documentMode command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::blankLines(int numberOfBlankLines)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B\x41");
	data.append((unsigned char)numberOfBlankLines);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "blankLines", P_HIGH, "Error sending blankLines command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::formFeed()
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x0C");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "formFeed", P_HIGH, "Error sending formFeed command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::lineFeed()
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x0A");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "lineFeed", P_HIGH, "Error sending lineFeed command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::carriageReturn()
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x0D");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "carriageReturn", P_HIGH, "Error sending carriageReturn command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::characterAlignment(int position)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B\x61");
	data.append((unsigned char)position);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "characterAlignment", P_HIGH, "Error sending characterAlignment command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::pageSize(int rows)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B\x43");
	data.append((unsigned char)rows);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "pageSize", P_HIGH, "Error sending pageSize command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::enableDisableCarriageReturn(int operation)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B\x35");
	data.append((unsigned char)operation);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "enableDisableCarriageReturn", P_HIGH, "Error sending enableDisableCarriageReturn command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setCharacterTable(int characterTable)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B\x74");
	data.append((unsigned char)characterTable);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setCharacterTable", P_HIGH, "Error sending setCharacterTable command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setUnderline(int operation)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B\x2D");
	data.append((unsigned char)operation);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setUnderline", P_HIGH, "Error sending setUnderline command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setItalic(int operation)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B\x4A");
	data.append((unsigned char)operation);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setItalic", P_HIGH, "Error sending setItalic command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setBold(int operation)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B");
	if(operation == TEXT_OPERATION_ON)	data.append("\x45");
	else								data.append("\x46");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setBold", P_HIGH, "Error sending setBold command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setSuperSubscript(int operation)
{
	int ret = 0;
	
	CBuffer data;
	data.setBuffer("\x1B");
	if(operation == ACTIVATE_TEXT_SUPERSCRIPT || operation == ACTIVATE_TEXT_SUBSCRIPT)
	{
		data.append("\x53");
		data.append((unsigned char)operation);
	}
	else //DEACTIVATE_TEXT_SCRIPT
	{
		data.setBuffer("\x54");
	}

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setSuperSubscript", P_HIGH, "Error sending setSuperSubscript command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setNumberOfColumns(int operation)
{
	int ret = 0;
	
	CBuffer data;
	if(operation == COLUMNS_NUMBER_DEFAULT) data.setBuffer("\x12");
	else if(operation == COLUMNS_NUMBER_48) data.setBuffer("\x1B\x48");
	else									data.setBuffer("\x0F"); //COLUMNS_NUMBER_64

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setNumberOfColumns", P_HIGH, "Error sending setNumberOfColumns command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setDoubleWidthChars(int operation)
{
	int ret = 0;

	CBuffer data;
	data.setBuffer("\x1B\x57");
	data.append((unsigned char)operation); 

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setDoubleWidthChars", P_HIGH, "Error sending setDoubleWidthChars command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setDoubleHeightChars(int operation)
{
	int ret = 0;

	CBuffer data;
	data.setBuffer("\x1B\x64");
	data.append((unsigned char)operation); 

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setDoubleHeightChars", P_HIGH, "Error sending setDoubleHeightChars command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::cancelLastLine()
{
	int ret = 0;

	CBuffer data;
	data.setBuffer("\x18");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "cancelLastLine", P_HIGH, "Error sending cancelLastLine command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::deleteLastChar()
{
	int ret = 0;

	CBuffer data;
	data.setBuffer("\x7F");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "deleteLastChar", P_HIGH, "Error sending deleteLastChar command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setItalicForValidationMode(int operation)
{
	int ret = 0;

	CBuffer data;
	data.setBuffer("\x1D\x69");
	data.append((unsigned char)operation);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setItalicForValidationMode", P_HIGH, "Error sending setItalicForValidationMode command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::setUnderlineForValidationMode(int operation)
{
	int ret = 0;

	CBuffer data;
	data.setBuffer("\x1D\x73");
	data.append((unsigned char)operation);

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "setUnderlineForValidationMode", P_HIGH, "Error sending setUnderlineForValidationMode command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::cut(int cutType)
{
	int ret = 0;

	CBuffer data;
	if(cutType == PAPER_CUT_TOTAL)	data.setBuffer("\x1B\x77");
	else							data.setBuffer("\x1B\x6D");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "cut", P_HIGH, "Error sending cut command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::printData(unsigned char* data, int dataLen)
{
	//para imprimir basta apenas enviar o texto para a impressora
	int ret = 0;

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send(data, dataLen) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "printData", P_HIGH, "Error sending printData command");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}

int CDeviceCommands::sendEtx()
{
	int ret = 0;

	CBuffer data;
	data.setBuffer("\x03");

	/**
	 *	comunicacao usb e tratamento do retorno
	 */
	if(m_comm->send((unsigned char*)data.getBuffer(), data.length()) == UPC_SUCCESS)
	{
		ret = CEFTMF_OK;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "printData", P_HIGH, "Error sending ETX to control the printing");
		ret = CEFTMF_ERR;
	}
	
	return ret;
}
