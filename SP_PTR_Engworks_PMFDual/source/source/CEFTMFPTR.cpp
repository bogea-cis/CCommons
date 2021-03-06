#include "CEFTMFPTR.h"
#include "CDeviceCommands.h"
#include "CObjectContainer.h"
#include "CBuffer.h"
#include "PTRConstants.h"
#include "CTimeUtils.h"
#include <time.h>

#define THIS_FILE			"CEFTMFPTR.cpp"
#define MAX_STATUS_REQUESTS	3
//#define CHECK_STATUS

static CDeviceCommands m_device;
static bool m_isInitialized = false;
static bool m_ioControl = false;
static unsigned char lastStatus = 0x00;

int CEFTMFAPI CEF_TMF_PTR_Initialize(void)
{
	int ret = CEFTMF_ERR;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Initialize", P_TRACE, "Initializing, compile date: [%s]", __DATE__);

	if(!m_isInitialized)
	{
		ret = m_device.initialize();
		if(ret == CEFTMF_OK)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Initialize", P_TRACE, "Printer initialized, configuring");
			m_device.setCharacterTable(CHARACTER_TABLE_CODEPAGE850);
			m_device.enableDisableCarriageReturn(TEXT_OPERATION_OFF);
			m_device.validationMode(TEXT_OPERATION_OFF);
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Initialize", P_TRACE, "Printer configured");
			m_ioControl = true;
			m_isInitialized = true;
			lastStatus = 0x00;
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Initialize", P_HIGH, "Error on printer initialization");
			ret = CEFTMF_ERR;
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Initialize", P_MEDIUM, "Printer already initialized");
		ret = CEFTMF_ERR_INITIALIZED;
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Initialize", P_TRACE, "Command ret [%d]", ret);
	return ret;
}

int CEFTMFAPI CEF_TMF_PTR_Terminate(void)
{
	int ret = CEFTMF_OK;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Terminate", P_TRACE, "Entering");

	if(m_isInitialized)
	{
		CObjectContainer::getUsbPrinterCommInstance()->reset();
		CObjectContainer::getUsbPrinterCommInstance()->finalize();
		m_isInitialized = false;
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Terminate", P_TRACE, "Printer finalized");
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Terminate", P_HIGH, "Printer not initialized");
		ret = CEFTMF_ERR_NOTINITIALIZED;
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Terminate", P_TRACE, "Command ret [%d]", ret);
	return ret;
}

int CEFTMFAPI CEF_TMF_PTR_Reset(void)
{
	int ret = CEFTMF_OK;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Reset", P_TRACE, "Entering");

	if(m_isInitialized)
	{		
		CObjectContainer::getUsbPrinterCommInstance()->reset();
		m_ioControl = true;
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Reset", P_HIGH, "Printer not initialized");
		ret = CEFTMF_ERR_NOTINITIALIZED;
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Reset", P_TRACE, "Command ret [%d]", ret);
	return ret;
}

int CEFTMFAPI CEF_TMF_PTR_PrintLine(unsigned char * Data, unsigned int DataLength)
{
	int ret = CEFTMF_OK;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_TRACE, "Entering, data len[%d], IOControl[%d]", DataLength, m_ioControl);

	if(m_isInitialized)
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_TRACE, "Checking printer status before send the print command");
		
		#ifdef CHECK_STATUS
		if(CEF_TMF_PTR_GetStatus() > 0)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Printer is in error state, impossible to print");
			ret = CEFTMF_ERR;
			return ret;
		}
		else
		{
			if(CEF_TMF_PTR_DocumentInserted() == 1) //TRUE
			{
				CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Document present on validation slot, impossible to print");
				ret = CEFTMF_ERR_NOTREADY;
				return ret;
			}
		}
		#endif

		int rret;
		int tries = 0;
		unsigned char info[4] = {0};
		int infoLen = sizeof(info);

		bool exit = false;
		do
		{
			rret = CObjectContainer::getUsbPrinterCommInstance()->receive(info, infoLen, 500);
			if(rret == UPC_SUCCESS)
			{
				if((info[0] & 0x01) != 0x01)
				{
					CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_TRACE, "Printer is not working...");
					exit = true;
				}
				else
				{
					CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_TRACE, "Printer is printing");
					tries++;
				}
			}
			else
			{
				CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_TRACE, "Error or timeout getting printer busy status");
				CTimeUtils::msleep(10);
				tries++;
			}

			if (tries  >= 5)
			{
				exit = true;
			}
		}while(!exit);


		int intermediateRet = 0;
		CBuffer dataToSend;
		for(unsigned int i = 0, charCount = 0; i < DataLength; i++, charCount++)
		{
			if(charCount == 2500)	
			{
				CTimeUtils::msleep(100);
				charCount = 0;
			}

			if( Data[i] != SO  && Data[i] != SI && Data[i] != DC2 && Data[i] != DC4 && 
				Data[i] != ESC && Data[i] != LF && Data[i] != CR  && Data[i] != US  && Data[i] != CIRCUMFLEX )
			{
				dataToSend.append(Data[i]);
			}
			else if(Data[i] == CIRCUMFLEX && Data[i+1] != '\x5F')
			{
				dataToSend.append(Data[i]);
			}
			else
			{
				if(dataToSend.length() > 0)
				{
					intermediateRet = m_device.printData((unsigned char*)dataToSend.getBuffer(), dataToSend.length());
					dataToSend.clear();
				}

				switch(Data[i])
				{
					case SO: //ativa caracteres expandidos
						intermediateRet = m_device.setDoubleWidthChars(TEXT_OPERATION_ON);
						break;
					case SI: //ativa caracteres comprimidos
						intermediateRet = m_device.setNumberOfColumns(COLUMNS_NUMBER_64);
						break;
					case DC2: //desativa caracteres comprimidos
						intermediateRet = m_device.setNumberOfColumns(COLUMNS_NUMBER_48);
						break;
					case DC4: //desativa caracteres expandidos
						intermediateRet = m_device.setDoubleWidthChars(TEXT_OPERATION_OFF);
						break;
					case ESC:
						switch(Data[i+1])
						{
							case '\x34': //ativa modo italico
								intermediateRet = m_device.setItalic(TEXT_OPERATION_ON);
								i++;
								break;
							case '\x35': //desativa modo italico
								intermediateRet = m_device.setItalic(TEXT_OPERATION_OFF);
								i++;
								break;
							case '\x45': //ativa modo negrito
								intermediateRet = m_device.setBold(TEXT_OPERATION_ON);
								i++;
								break;
							case '\x46': //desativa modo negrito
								intermediateRet = m_device.setBold(TEXT_OPERATION_OFF);
								i++;
								break;
							case '\x5F':
							case '\x6D':
								intermediateRet = m_device.cut(PAPER_CUT_PARTIAL);
								i++;
								break;
							case '\x77':
								intermediateRet = m_device.cut(PAPER_CUT_TOTAL);
								i++;
								break;
							case '\x2D':
								switch(Data[i+2])
								{
									case '\x31': //ativa modo sublinhado
										intermediateRet = m_device.setUnderline(TEXT_OPERATION_ON);
										i += 2;
										break;
									case '\x30': //desativa modo sublinhado
										intermediateRet = m_device.setUnderline(TEXT_OPERATION_OFF);
										i += 2;
										break;
									default:
										//logar caractere desconhecido e nao fazer nada
										CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Unknown special caracter received [%x], no action taken", Data[i+2]);
										break;
								}
								break;

							default:
								//logar caractere desconhecido e nao fazer nada
								CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Unknown special caracter received [%x], no action taken", Data[i+1]);
								break;
						}
						break;
					case CIRCUMFLEX:
						if(Data[i+1] == '\x5F')
						{
							intermediateRet = m_device.cut(PAPER_CUT_PARTIAL);
							i++;
						}
						break;
					case US:
						intermediateRet = m_device.cut(PAPER_CUT_PARTIAL);
						break;
					case LF: //quebra de linha
						intermediateRet = m_device.lineFeed();
						break;
					case CR: //movimentacao do cursor para o comeco da linha
						intermediateRet = m_device.carriageReturn();
						break;
					default:
						//logar caractere desconhecido e nao fazer nada
						CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Unknown special caracter received [%x], no action taken", Data[i]);
						break;
				}

				if(intermediateRet != CEFTMF_OK)
				{
					CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Error(1) sending part of the print command");
					ret = CEFTMF_ERR;
					break;
				}
			}
		}

		if(dataToSend.length() > 0)
		{
			intermediateRet = m_device.printData((unsigned char*)dataToSend.getBuffer(), dataToSend.length());
			dataToSend.clear();
		}
		if(intermediateRet != CEFTMF_OK)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Error(2) sending part of the print command");
			ret = CEFTMF_ERR;
		}

		if(ret == CEFTMF_OK && m_ioControl)
		{
			//CTimeUtils::msleep(10);
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_TRACE, "Waiting for print to finish before return");

			CTimeUtils tu;
			tu.start(2000);
			m_device.sendEtx();
			while(true)
			{
				if(tu.checkTimeout() == true)
				{
					CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Timeout waiting for print to finish");
					ret = CEFTMF_ERR;
					break;
				}

				for(int i = 0; i < 3; i++)	CObjectContainer::getUsbPrinterCommInstance()->receive(info, infoLen, 200);

				rret = CObjectContainer::getUsbPrinterCommInstance()->receive(info, infoLen, 50);
				if(rret == UPC_SUCCESS)
				{
					if((info[0] & 0x01) == 0x01)
					{
						CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_TRACE, "Print finished");
						break;
					}
					tu.start(2000);
				}
				else if (rret == UPC_ERROR_TIMEOUT)
				{
					CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Timeout waiting printer signal");
				}
				else
				{
					CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Error waiting for print to finish");
					//ret = CEFTMF_ERR;
					//break;
				}
			}
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_HIGH, "Printer not initialized");
		ret = CEFTMF_ERR_NOTINITIALIZED;
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintLine", P_TRACE, "Command ret [%d]", ret);
	return ret;
}

int CEFTMFAPI CEF_TMF_PTR_PrintDoc(unsigned char * Data, unsigned int DataLength)
{
	int ret = CEFTMF_OK;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_TRACE, "Entering");

	if(m_isInitialized)
	{
		#ifdef CHECK_STATUS
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_TRACE, "Checking printer status before send the print command");
		if(CEF_TMF_PTR_GetStatus() > 0)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_HIGH, "Printer is in error state, impossible to print");
			ret = CEFTMF_ERR;
			return ret;
		}
		else
		{
			if(CEF_TMF_PTR_DocumentInserted() == 0) //FALSE
			{
				CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_HIGH, "Document not present on validation slot, impossible to print");
				ret = CEFTMF_ERR_NOTREADY;
				return ret;
			}
		}
		#endif

		if(DataLength >= 48)
		{
			//logar que parte do texto vai ser impresso na parte termica
			if(DataLength > 48) CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_MEDIUM, "Text is too long, it will be truncated");

			Data[48] = '\x0A';
			Data[49] = '\x00';
			DataLength = 49;
		}
		else
		{
			if(Data[DataLength - 1] != '\x0A')
			{
				Data[DataLength] = '\x0A';
				Data[DataLength + 1] = '\x00';
				DataLength++;
			}
		}
		
		if(m_device.validationMode(TEXT_OPERATION_ON) == CEFTMF_OK)
		{
			ret = m_device.printData(Data, DataLength);
			if(ret == CEFTMF_OK)
			{
				CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_TRACE, "Text printed");
			}
			else
			{
				CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_HIGH, "Error sending data to be printed");
			}
			m_device.validationMode(TEXT_OPERATION_OFF);
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_HIGH, "Printer not initialized");
		ret = CEFTMF_ERR_NOTINITIALIZED;
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintDoc", P_TRACE, "Command ret [%d]", ret);
	return ret;
}

int CEFTMFAPI CEF_TMF_PTR_PrintRawData(unsigned char * Data, 
                                       unsigned int DataLength)
{
	int ret = CEFTMF_OK;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintRawData", P_TRACE, "Entering");

	if(m_isInitialized)
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintRawData", P_TRACE, "Checking printer status before send the print command");
		
		#ifdef CHECK_STATUS
		if(CEF_TMF_PTR_GetStatus() > 0)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintRawData", P_HIGH, "Printer is in error state, impossible to print");
			ret = CEFTMF_ERR;
			return ret;
		}
		else
		{
			if(CEF_TMF_PTR_DocumentInserted() != 0)
			{
				CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintRawData", P_HIGH, "There is a dociument present on validation slot, unable to print data");
				ret = CEFTMF_ERR_NOTREADY;
				return ret;
			}
		}
		#endif

		ret = m_device.printData(Data, DataLength);

		if(ret == CEFTMF_OK)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintRawData", P_TRACE, "Data printed");
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintRawData", P_HIGH, "Error sending data to be printed");
		}
		//m_device.validationMode(TEXT_OPERATION_OFF);
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_PrintRawData", P_HIGH, "Printer not initialized");
		ret = CEFTMF_ERR_NOTINITIALIZED;
	}
	
	return ret;
}

void CEF_TMF_PTR_UpdateLastStatus(unsigned char* status)
{
	if(lastStatus != status[0])
	{
		lastStatus = status[0];
	}
}

int CEFTMFAPI CEF_TMF_PTR_GetStatus(void)
{
	int ret = CEFTMF_ERR;
	//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_TRACE, "Entering");
	clock_t tStart = clock();

	if(m_isInitialized)
	{
		unsigned char info[4] = {0};

		int infoLen = sizeof(info);
		for(int i = 0; i < MAX_STATUS_REQUESTS; i++)
		{
			memset(info, 0, sizeof(info)); 
			//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_TRACE, "Entering libusb");
			ret = CObjectContainer::getUsbPrinterCommInstance()->receive(info, infoLen, 1000);
			//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_TRACE, "Exited libusb");
			
			if(ret != UPC_SUCCESS)
			{
				if(info[0] != '\0')
				{
					if((ret & CEF_TMF_PTR_IOERROR) != CEF_TMF_PTR_IOERROR)		ret += CEF_TMF_PTR_IOERROR; //adicionando status IOError
				}
				else
				{
					if((ret & CEF_TMF_PTR_POWEROFF) != CEF_TMF_PTR_POWEROFF)	ret += CEF_TMF_PTR_POWEROFF; //adicionando status PrinterOff
				}
			}
			else
			{
				CEF_TMF_PTR_UpdateLastStatus(info);
				//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_TRACE, "Status: [%X]", info[0]);

				if((ret & CEF_TMF_PTR_IOERROR) == CEF_TMF_PTR_IOERROR)		ret -= CEF_TMF_PTR_IOERROR;	 //removendo status IOError
				if((ret & CEF_TMF_PTR_POWEROFF) == CEF_TMF_PTR_POWEROFF)	ret -= CEF_TMF_PTR_POWEROFF; //removendo status PrinterOff

				
				if((info[0] & 0x80) == 0x80)
				{
					if((ret & CEF_TMF_PTR_OUTOFPAPER) != CEF_TMF_PTR_OUTOFPAPER)	ret += CEF_TMF_PTR_OUTOFPAPER; //adicionando status OutOfPaper
				}
				else
				{
					if((ret & CEF_TMF_PTR_OUTOFPAPER) == CEF_TMF_PTR_OUTOFPAPER)	ret -= CEF_TMF_PTR_OUTOFPAPER; //removendo status OutOfPaper
				}

				if((info[0] & 0x20) == 0x20)
				{
					if((ret & CEF_TMF_PTR_NOTREADY) != CEF_TMF_PTR_NOTREADY)	ret += CEF_TMF_PTR_NOTREADY; //adicionando status NotReady (tampa aberta)
				}
				else
				{
					if((ret & CEF_TMF_PTR_NOTREADY) == CEF_TMF_PTR_NOTREADY)	ret -= CEF_TMF_PTR_NOTREADY; //removendo status NotReady (tampa aberta)
				}
			
				if((info[0] & 0x40) == 0x40)
				{
					if((ret & CEF_TMF_PTR_DOCINSERTED) != CEF_TMF_PTR_DOCINSERTED)	ret += CEF_TMF_PTR_DOCINSERTED; //adicionando status Documento inserido
				}
				else
				{
					if((ret & CEF_TMF_PTR_DOCINSERTED) == CEF_TMF_PTR_DOCINSERTED)	ret -= CEF_TMF_PTR_DOCINSERTED; //removendo status Documento inserido
				}
			}
		}

		if(ret == CEFTMF_OK)
		{
			//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_TRACE, "Printer status: [GOOD]");
		}
		else
		{
			if((ret & CEF_TMF_PTR_OUTOFPAPER) == CEF_TMF_PTR_OUTOFPAPER)
			{
				//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_HIGH, "Printer status: [OUT OF PAPER]");
			}
			if((ret & CEF_TMF_PTR_POWEROFF) == CEF_TMF_PTR_POWEROFF)
			{
				//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_HIGH, "Printer status: [POWER OFF]");
			}
			if((ret & CEF_TMF_PTR_IOERROR) == CEF_TMF_PTR_IOERROR)
			{
				//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_HIGH, "Printer status: [IO ERROR]");
			}
			if((ret & CEF_TMF_PTR_NOTREADY) == CEF_TMF_PTR_NOTREADY)
			{
				//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_HIGH, "Printer status: [NOT READY]");
			}
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_HIGH, "Printer not initialized");
		ret = CEFTMF_ERR_NOTINITIALIZED;
	}

	//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_HIGH, "Time taken to get Status: %.3fs",(double)(clock() - tStart)/CLOCKS_PER_SEC);
	//CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_GetStatus", P_TRACE, "Command ret [%d]", ret);
	return ret;
}

int CEFTMFAPI CEF_TMF_PTR_DocumentInserted(void)
{
	int ret = CEFTMF_OK;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_DocumentInserted", P_TRACE, "Entering");

	if(m_isInitialized)
	{
		if((lastStatus & 0x40) == 0x40)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_DocumentInserted", P_TRACE, "Document present on validation slot");
			ret = 1; //TRUE
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_DocumentInserted", P_TRACE, "Document not present on validation slot");
			ret = 0; //FALSE
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_DocumentInserted", P_TRACE, "Device not initialized, returning document not present");
		ret = 0; //FALSE
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_DocumentInserted", P_TRACE, "Command ret [%d]", ret);
	return ret;
}

int CEFTMFAPI CEF_TMF_PTR_IoControl(int Command, void * pArg)
{
	int ret = CEFTMF_OK;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_IoControl", P_TRACE, "Entering");

	if(m_isInitialized)
	{
		if(Command == CEF_TMF_PTR_IO_ENABLEETX)
		{
			m_ioControl = true;
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_IoControl", P_TRACE, "IO Control activated");
		}
		else
		{
			m_ioControl = false;
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_IoControl", P_TRACE, "IO Control deactivated");
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_IoControl", P_HIGH, "Printer not initialized");
		ret = CEFTMF_ERR_NOTINITIALIZED;
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_IoControl", P_TRACE, "Command ret [%d]", ret);
	return ret;
}

int CEFTMFAPI CEF_TMF_PTR_Who(void * pArg)
{
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "Entering");

	if(!m_isInitialized)
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "Initializing printer just to get the requested information");
		if(m_device.initialize() != CEFTMF_OK)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_HIGH, "Error on printer initialization");
			return CEFTMF_ERR;
		}
	}

	CEFTMF_VERSION* versionData = (CEFTMF_VERSION*)pArg;

	unsigned char manufactorer[40];
	unsigned char product[40];
	unsigned char serialNumber[40];
	unsigned short manufactorerLen = sizeof(manufactorer);
	unsigned short productLen = sizeof(product);
	unsigned short serialNumberLen = sizeof(serialNumber);

	if( CObjectContainer::getUsbPrinterCommInstance()->getManufactorer(manufactorer, &manufactorerLen) <= 0 ||
		CObjectContainer::getUsbPrinterCommInstance()->getProduct(product, &productLen)  <= 0 ||
		CObjectContainer::getUsbPrinterCommInstance()->getSerialNumber(serialNumber, &serialNumberLen) <= 0 )
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_HIGH, "Error getting information from printer");
		return CEFTMF_ERR;
	}

	char formatDateDevRelease[9] = {0};
	sprintf(formatDateDevRelease, "%c%c.%c%c.%c%c", product[16],product[17],product[14],product[15],product[12],product[13]);
	char formatDeviceRelease[6] = {0};
	sprintf(formatDeviceRelease,  "%c%c.%c%c", product[10], product[11], product[8], product[9]);

	memcpy(versionData->Provider, (const char*)&manufactorer[0], manufactorerLen < 40 ? manufactorerLen : 40);
	memcpy(versionData->DeviceName, (const char*)&product[0], 6);
	memcpy(versionData->DeviceRelease, (const char*)&formatDeviceRelease[0], 6);
	memcpy(versionData->DateDevRelease, (const char*)&formatDateDevRelease[0], 9); //
	memcpy(versionData->ReleaseDLL, "01.00\0" , 6); //
	memcpy(versionData->DateDLL, "14.02.17\0" , 9); //
	memcpy(versionData->DevicePropertyNumber, (const char*)&serialNumber[0], 13); //
	memcpy(versionData->DeviceSerialNumber, (const char*)&serialNumber[0], serialNumberLen);

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "Printer information received:");
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "Provider: [%s]", versionData->Provider);
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "DeviceName: [%s]", versionData->DeviceName);
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "DeviceRelease: [%s]", versionData->DeviceRelease);
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "DateDevRelease: [%s]", versionData->DateDevRelease);
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "ReleaseDLL: [%s]", versionData->ReleaseDLL);
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "DateDLL: [%s]", versionData->DateDLL);
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "DevicePropertyNumber: [%s]", versionData->DevicePropertyNumber);
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Who", P_TRACE, "DeviceSerialNumber: [%s]", versionData->DeviceSerialNumber);

	if(!m_isInitialized)
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Terminate", P_TRACE, "Finalizing printer because it was not initialized before this command");
		CObjectContainer::getUsbPrinterCommInstance()->finalize();
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_Terminate", P_TRACE, "Printer finalized");
	}

	return CEFTMF_OK;
}

int CEFTMFAPI CEF_TMF_PTR_ReceiptCut(int uiMode)
{
	int ret = CEFTMF_OK;
	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_ReceiptCut", P_TRACE, "Entering");

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_ReceiptCut", P_TRACE, "Checking printer status before send the cut command");
	
	#ifdef CHECK_STATUS
	if(CEF_TMF_PTR_GetStatus() > 0)
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_ReceiptCut", P_HIGH, "Printer is in error state, impossible to cut");
		ret = CEFTMF_ERR;
		return ret;
	}
	#endif

	if(m_isInitialized)
	{
		ret = m_device.cut(uiMode);
		if(ret == CEFTMF_OK)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_ReceiptCut", P_TRACE, "Paper cut executed");
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_ReceiptCut", P_HIGH, "Error sending cut command");
			ret = CEFTMF_ERR;
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_ReceiptCut", P_HIGH, "Printer not initialized");
		ret = CEFTMF_ERR_NOTINITIALIZED;
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "CEF_TMF_PTR_ReceiptCut", P_TRACE, "Command ret [%d]", ret);
	return ret;
}
