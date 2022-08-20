// CPosixSerial.h: interface for the CPosixSerial class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _CPOSIXSERIAL_H_
#define _CPOSIXSERIAL_H_

#include <termios.h> /* POSIX terminal control definitions */

#define ERROR_SUCCESS 0
#define ERROR_ERROR -1

class CPosixSerial
{
public :


		// Baudrate
		typedef enum
		{
			EBaudUnknown = -1,
			EBaud50		= B50,
			EBaud75		= B75,
			EBaud110	= B110,
			EBaud150	= B150,
			EBaud200	= B200,
			EBaud300	= B300,
			EBaud600	= B600,
			EBaud1200	= B1200,
			EBaud1800	= B1800,
			EBaud2400	= B2400,
			EBaud4800	= B4800,
			EBaud9600	= B9600,
			EBaud19200	= B19200,
			EBaud38400	= B38400,
			EBaud57600	= B57600,
			EBaud115200	= B115200
		}
		EBaudrate;

		// Data bits (5-8)
		typedef enum
		{
			EDataUnknown = -1,			// Unknown
			EData5       =  CS5,			// 5 bits per byte
			EData6       =  CS6,			// 6 bits per byte
			EData7       =  CS7,			// 7 bits per byte
			EData8       =  CS8			// 8 bits per byte (default)
		}
		EDataBits;

		// Parity scheme
		typedef enum
		{
			EParUnknown = -1,			// Unknown
			EParNone,					// No parity (default)
			EParOdd,					// Odd parity
			EParEven,					// Even parity
			EParSpace					// Space parity
		}
		EParity;

		// Stop bits
		typedef enum
		{
			EStopUnknown = -1,			// Unknown
			EStop1,						// 1 stopbit (default)
			EStop2						// 2 stopbits
		}
		EStopBits;
		

public:
	CPosixSerial();
	virtual ~CPosixSerial();

	virtual bool Open (const char* device, EBaudrate baund, EDataBits databits, EParity parity, EStopBits stop);
	virtual bool Close ();
	virtual int Write (const void* buffer, int len);
	virtual int Write (const char* buffer);
	virtual int Read (unsigned char* buffer, int len, int timeout);


	virtual bool SetBaundRate (EBaudrate baund);
	virtual bool SetDataBits (EDataBits databits);
	virtual bool SetParity (EParity parity);
	virtual bool SetStopbits (EStopBits stop);
	
	virtual bool EnableSoftwareFlowControl ();
	virtual bool DisableSoftwareFlowControl ();	
	virtual bool EnableHardwareFlowControl();
	virtual bool DisableHardwareFlowControl();

	virtual void SetRTS ();
	virtual void SetCTS ();
	virtual void SetDTR ();

	virtual void ClearRTS ();
	virtual void ClearCTS ();
	virtual void ClearDTR ();

	virtual bool CheckBytesAvailable (int timeout);
	virtual bool CheckBytesAvailable ();
	virtual int BytesAvailable ();
	virtual bool Purge ();

	virtual void SetSignal (int signal);
	virtual void ClearSignal (int signal);

	virtual bool GetSignal (int signal);
	virtual bool IsOpen ();

protected:



	int m_fd;


	
private:

	
	
};
#endif 
