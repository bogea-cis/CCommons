//	Serial.cpp - Implementation of the CSerial class
//
//	Copyright (C) 1999-2003 Ramon de Klein (Ramon.de.Klein@ict.nl)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// ;
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//////////////////////////////////////////////////////////////////////
// Include the standard header files

#ifdef WINDOWS

#define STRICT
//#include <crtdbg.h>
#include <tchar.h>
#include <windows.h>


//////////////////////////////////////////////////////////////////////
// Include module headerfile

#include "CSerial.h"



//////////////////////////////////////////////////////////////////////
// Enable debug memory manager

#ifdef _DEBUG

#ifdef THIS_FILE
#undef THIS_FILE
#endif

static const char THIS_FILE[] = "CSerial.cpp";
#define new DEBUG_NEW

#endif


//////////////////////////////////////////////////////////////////////
// Helper methods

inline void CSerial::CheckRequirements (LPOVERLAPPED lpOverlapped, DWORD dwTimeout) const
{
#ifdef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (lpOverlapped || (dwTimeout != INFINITE))
	{
		// Quit application
		::MessageBox(0,_T("Overlapped I/O and time-outs are not supported, when overlapped I/O is disabled."),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
		::DebugBreak();
		::ExitProcess(0xFFFFFFF);
	}

#endif

#ifdef SERIAL_NO_CANCELIO

	// Check if 0 or INFINITE time-out has been specified, because
	// the communication I/O cannot be cancelled.
	if ((dwTimeout != 0) && (dwTimeout != INFINITE))
	{
		// Quit application
		::MessageBox(0,_T("Timeouts are not supported, when SERIAL_NO_CANCELIO is defined"),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
		::DebugBreak();
		::ExitProcess(0xFFFFFFF);
	}

#endif	// SERIAL_NO_CANCELIO

	// Avoid warnings
	(void) dwTimeout;
	(void) lpOverlapped;
}

inline bool CSerial::CancelCommIo (void)
{
#ifdef SERIAL_NO_CANCELIO
	// CancelIo shouldn't have been called at this point
	::DebugBreak();
	return FALSE;
#else

	// Cancel the I/O request
	return ::CancelIo(m_hFile) != 0;

#endif	// SERIAL_NO_CANCELIO
}


//////////////////////////////////////////////////////////////////////
// Code

CSerial::CSerial ()
	: m_lLastError(ERROR_SUCCESS)
	, m_hFile(0)
	, m_eEvent(EEventNone)
	, m_dwEventMask(0)
#ifndef SERIAL_NO_OVERLAPPED
	, m_hevtOverlapped(0)
#endif
{
}


CSerial::~CSerial ()
{
	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile)
	{
		// Display a warning

		// Close implicitly
		Close();
	}
}


CSerial::EPort CSerial::CheckPort (LPCTSTR lpszDevice)
{
	// Try to open the device
	HANDLE hFile = ::CreateFile(lpszDevice, 
						   GENERIC_READ|GENERIC_WRITE, 
						   0, 
						   0, 
						   OPEN_EXISTING, 
						   0,
						   0);

	// Check if we could open the device
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Display error
		switch (::GetLastError())
		{
		case ERROR_FILE_NOT_FOUND:
			// The specified COM-port does not exist
			return EPortNotAvailable;

		case ERROR_ACCESS_DENIED:
			// The specified COM-port is in use
			return EPortInUse;

		default:
			// Something else is wrong
			return EPortUnknownError;
		}
	}

	// Close handle
	::CloseHandle(hFile);

	// Port is available
	return EPortAvailable;
}

LONG CSerial::Open (LPCTSTR lpszDevice, DWORD dwInQueue, DWORD dwOutQueue, bool fOverlapped, bool buffering)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the port isn't already opened
	if (m_hFile)
	{
		m_lLastError = ERROR_ALREADY_INITIALIZED;
		return m_lLastError;
	}

	DWORD dwFlagsAndAttrib = 0;

	if (fOverlapped)
	{
		dwFlagsAndAttrib |= FILE_FLAG_OVERLAPPED;
	}
	if (buffering)
	{
		dwFlagsAndAttrib |= FILE_FLAG_NO_BUFFERING;
		dwFlagsAndAttrib |= FILE_FLAG_WRITE_THROUGH;
	}

	// Open the device
	m_hFile = ::CreateFile(lpszDevice,
							   GENERIC_READ|GENERIC_WRITE, 
							   FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, //0,
							   0,
							   OPEN_EXISTING,
							   dwFlagsAndAttrib,
							   0);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		// Reset file handle
		m_hFile = 0;

		// Display error
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}



#ifndef SERIAL_NO_OVERLAPPED
	// We cannot have an event handle yet

	// Create the event handle for internal overlapped operations (manual reset)
	//if (fOverlapped)	
	if (fOverlapped && m_hevtOverlapped == NULL)
	{
		m_hevtOverlapped = ::CreateEvent(0,true,false,0);
		if (m_hevtOverlapped == 0)
		{
			// Obtain the error information
			m_lLastError = ::GetLastError();

			// Close the port
			::CloseHandle(m_hFile);
			m_hFile = 0;

			// Return the error
			return m_lLastError;
		}
	}
#else
	
	// Overlapped flag shouldn't be specified
	_ASSERTE(!fOverlapped);

#endif

	// Setup the COM-port
	if (dwInQueue || dwOutQueue)
	{
		// Make sure the queue-sizes are reasonable sized. Win9X systems crash
		// if the input queue-size is zero. Both queues need to be at least
		// 16 bytes large.

		if (!::SetupComm(m_hFile,dwInQueue,dwOutQueue))
		{
			// Display a warning
			long lLastError = ::GetLastError();

			// Close the port
			Close();

			// Save last error from SetupComm
			m_lLastError = lLastError;
			return m_lLastError;	
		}
	}

	// Setup the default communication mask
	SetMask();

	// Non-blocking reads is default
	SetupReadTimeouts(EReadTimeoutNonblocking);

	// Setup the device for default settings
 	COMMCONFIG commConfig = {0};
	DWORD dwSize = sizeof(commConfig);
	commConfig.dwSize = dwSize;
	if (::GetDefaultCommConfig(lpszDevice,&commConfig,&dwSize))
	{
		// Set the default communication configuration
		if (!::SetCommConfig(m_hFile,&commConfig,dwSize))
		{
			// Display a warning
		}
	}
	else
	{
		// Display a warning
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::Close (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile == 0)
	{
		// Display a warning
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED
	// Free event handle
	if (m_hevtOverlapped)
	{
		::CloseHandle(m_hevtOverlapped);
		m_hevtOverlapped = 0;
	}
#endif

	// Close COM port
	::CloseHandle(m_hFile);
	m_hFile = 0;

	// Return successful
	return m_lLastError;
}

LONG CSerial::Setup (EBaudrate eBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::	GetLastError();

		// Display a warning
		return m_lLastError;
	}

	// Set the new data
	dcb.BaudRate = DWORD(eBaudrate);
	dcb.ByteSize = BYTE(eDataBits);
	dcb.Parity   = BYTE(eParity);
	dcb.StopBits = BYTE(eStopBits);

	// Determine if parity is used
	dcb.fParity  = (eParity != EParNone);

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::SetEventChar (BYTE bEventChar, bool fAdjustMask)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return m_lLastError;
	}

	// Set the new event character
	dcb.EvtChar = char(bEventChar);

	// Adjust the event mask, to make sure the event will be received
	if (fAdjustMask)
	{
		// Enable 'receive event character' event.  Note that this
		// will generate an EEventNone if there is an asynchronous
		// WaitCommEvent pending.
		SetMask(GetEventMask() | EEventRcvEv);
	}

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::SetMask (DWORD dwEventMask)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

	// Set the new mask. Note that this will generate an EEventNone
	// if there is an asynchronous WaitCommEvent pending.
	if (!::SetCommMask(m_hFile,dwEventMask))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return m_lLastError;
	}

	// Save event mask and return successful
	m_dwEventMask = dwEventMask;
	return m_lLastError;
}

LONG CSerial::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckRequirements(lpOverlapped,dwTimeout);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (!lpOverlapped && m_hevtOverlapped)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy

	// Wait for the COM event
	if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelCommIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}
#else

	// Wait for the COM event
	if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Unable to wait for COM event\n");
		return m_lLastError;
	}

#endif

	// Return successfully
	return m_lLastError;
}


LONG CSerial::SetupHandshaking (EHandshake eHandshake)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return m_lLastError;
	}

	// Set the handshaking flags
	switch (eHandshake)
	{
	case EHandshakeOff:
		dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
		dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	case EHandshakeHardware:
		dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
		dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
		break;

	case EHandshakeSoftware:
		dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
		dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
		dcb.fOutX = true;							// Enable XON/XOFF for transmission
		dcb.fInX = true;							// Enable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	default:
		// This shouldn't be possible
		m_lLastError = E_INVALIDARG;
		return m_lLastError;
	}

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::SetupReadTimeouts (int interval, int total, int multiplier)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

	// Determine the time-outs
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return m_lLastError;
	}

	cto.ReadIntervalTimeout = interval;
	cto.ReadTotalTimeoutConstant = total;
	cto.ReadTotalTimeoutMultiplier = multiplier;


	// Set the new DCB structure
	if (!::SetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}



LONG CSerial::SetupReadTimeouts (EReadTimeout eReadTimeout)
{
	int interval, total, multiplier;

	interval = total = multiplier = 0;

	switch (eReadTimeout)
	{
		case EReadTimeoutBlocking:
			interval = 0;
			total = 0;
			multiplier = 0;
			break;
	
		case EReadTimeoutNonblocking:
			interval = MAXDWORD;
			total = 0;
			multiplier = 0;
			break;

		default:
			m_lLastError = E_INVALIDARG;
			return m_lLastError;
	}

	return SetupReadTimeouts(interval, total, multiplier);
}

CSerial::EBaudrate CSerial::GetBaudrate (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return EBaudUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return EBaudUnknown;
	}

	// Return the appropriate baudrate
	return EBaudrate(dcb.BaudRate);
}

CSerial::EDataBits CSerial::GetDataBits (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return EDataUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return EDataUnknown;
	}

	// Return the appropriate bytesize
	return EDataBits(dcb.ByteSize);
}

CSerial::EParity CSerial::GetParity (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return EParUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return EParUnknown;
	}

	// Check if parity is used
	if (!dcb.fParity)
	{
		// No parity
		return EParNone;
	}

	// Return the appropriate parity setting
	return EParity(dcb.Parity);
}

CSerial::EStopBits CSerial::GetStopBits (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return EStopUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return EStopUnknown;
	}

	// Return the appropriate stopbits
	return EStopBits(dcb.StopBits);
}

DWORD CSerial::GetEventMask (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return 0;
	}

	// Return the event mask
	return m_dwEventMask;
}

BYTE CSerial::GetEventChar (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return 0;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return 0;
	}

	// Set the new event character
	return BYTE(dcb.EvtChar);
}

CSerial::EHandshake CSerial::GetHandshaking (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return EHandshakeUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return EHandshakeUnknown;
	}

	// Check if hardware handshaking is being used
	if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
		return EHandshakeHardware;

	// Check if software handshaking is being used
	if (dcb.fOutX && dcb.fInX)
		return EHandshakeSoftware;

	// No handshaking is being used
	return EHandshakeOff;
}

LONG CSerial::Write (const void* pData, size_t iLen, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckRequirements(lpOverlapped,dwTimeout);

	// Overlapped operation should specify the pdwWritten variable

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Use our own variable for read count
	DWORD dwWritten;
	if (pdwWritten == 0)
	{
		pdwWritten = &dwWritten;
	}

	// Reset the number of bytes written
	*pdwWritten = 0;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (!lpOverlapped && m_hevtOverlapped)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy

	// Write the data
	if (!::WriteFile(m_hFile,pData,(DWORD)iLen,pdwWritten,lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwWritten,FALSE))
				{
					// Set the internal error code
					m_lLastError = ::GetLastError();

					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelCommIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}

#else

	// Write the data
	if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();

		// Issue an error and quit
		return m_lLastError;
	}

#endif

	// Return successfully
	return m_lLastError;
}

LONG CSerial::Write (LPCSTR pString, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckRequirements(lpOverlapped,dwTimeout);

	// Determine the length of the string
	return Write(pString,strlen(pString),pdwWritten,lpOverlapped,dwTimeout);
}

/*
LONG CSerial::ReadSync (void* pData, size_t iLen, DWORD* pdwRead)
{
	return ReadAsync (pData, iLen, NULL, INFINITE);
}

LONG CSerial::ReadAsync (void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
}
*/

LONG CSerial::Read (void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckRequirements(lpOverlapped,dwTimeout);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Use our own variable for read count
	DWORD dwRead;
	if (pdwRead == 0)
	{
		pdwRead = &dwRead;
	}

	// Reset the number of bytes read
	*pdwRead = 0;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

#ifdef _DEBUG
	// The debug version fills the entire data structure with
	// 0xDC bytes, to catch buffer errors as soon as possible.
	memset(pData,0xDC,iLen);
#endif

#ifndef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	
	// Read the data
	if (!::ReadFile(m_hFile,pData,(DWORD)iLen,pdwRead,lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwRead,FALSE))
				{
					// Set the internal error code
					m_lLastError = ::GetLastError();

					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelCommIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}

#else
	
	// Read the data
	if (!::ReadFile(m_hFile,pData,iLen,pdwRead,0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();

		// Issue an error and quit
		return m_lLastError;
	}

#endif

	// Return successfully
	return m_lLastError;
}

LONG CSerial::PurgeTx ()
{
	return Purge(PURGE_TXCLEAR | PURGE_TXABORT);
}

LONG CSerial::PurgeRx ()
{
	return Purge(PURGE_RXCLEAR | PURGE_RXABORT);
}

LONG CSerial::Purge(unsigned long flags)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

	//if (!::PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT))
	if (!::PurgeComm(m_hFile, flags))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();
	}
	
	// Return successfully
	return m_lLastError;
}

LONG CSerial::Break (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return m_lLastError;
	}

    // Set the RS-232 port in break mode for a little while
    ::SetCommBreak(m_hFile);
    ::Sleep(100);
    ::ClearCommBreak(m_hFile);

	// Return successfully
	return m_lLastError;
}

CSerial::EEvent CSerial::GetEventType (void)
{
#ifdef _DEBUG
	// Check if the event is within the mask
	if ((m_eEvent & m_dwEventMask) == 0)
	{
	}
#endif

	// Obtain the event (mask unwanted events out)
	EEvent eEvent = EEvent(m_eEvent & m_dwEventMask);

	// Reset internal event type
	m_eEvent = EEventNone;

	// Return the current cause
	return eEvent;
}

CSerial::EError CSerial::GetError (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		return EErrorUnknown;
	}

	// Obtain COM status
	DWORD dwErrors = 0;
	if (!::ClearCommError(m_hFile,&dwErrors,0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();

		// Issue an error and quit
		return EErrorUnknown;
	}

	// Return the error
	return EError(dwErrors);
}

bool CSerial::GetCTS (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return false;
	}

	// Determine if CTS is on
	return (dwModemStat & MS_CTS_ON) != 0;
}

bool CSerial::GetDSR (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return false;
	}

	// Determine if DSR is on
	return (dwModemStat & MS_DSR_ON) != 0;
}

bool CSerial::GetRing (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return false;
	}

	// Determine if Ring is on
	return (dwModemStat & MS_RING_ON) != 0;
}

bool CSerial::GetRLSD (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		return false;
	}

	// Determine if RLSD is on
	return (dwModemStat & MS_RLSD_ON) != 0;
}

void CSerial::ClearRTS ()
{
	::EscapeCommFunction(m_hFile, CLRRTS);
}

void CSerial::SetRTS ()
{
	::EscapeCommFunction(m_hFile, SETRTS);
}

void CSerial::ClearDTR ()
{
	::EscapeCommFunction(m_hFile, CLRDTR);
}

void CSerial::SetDTR ()
{
	::EscapeCommFunction(m_hFile, SETDTR);
}

void CSerial::ClearCommBreak ()
{
	::ClearCommBreak(m_hFile);
}

void CSerial::SetCommBreak ()
{
	::SetCommBreak(m_hFile);
}

int CSerial::BytesAvailable ()
{
	int ret = 0;
	if( m_hFile)
	{
		DWORD dwErrorFlags;
		COMSTAT comStat;
		ClearCommError( m_hFile, &dwErrorFlags, &comStat );
		ret = comStat.cbInQue;

	}

	return ret;

}

#endif
