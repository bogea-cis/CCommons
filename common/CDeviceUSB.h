#ifndef _CDEVICEUSB_H_
#define _CDEVICEUSB_H_

#include <Windows.h>
#include <setupapi.h> 
#include <winioctl.h>
#include <initguid.h>


class CDeviceUSB
{
public:

  CDeviceUSB();
  virtual ~CDeviceUSB();

  virtual bool Open( const unsigned short vendorId, const unsigned short productId);
  virtual bool Close();
  virtual int Write( const void* buffer, int length);
  virtual int Write( const char* buffer);
  virtual int Read( unsigned char* buffer, int length, int timeout);
  virtual bool IsOpen();


private:

protected:
  
  bool connected;
  HANDLE hDevice;
  LONG	m_lLastError;		// Last serial error
  OVERLAPPED ReadOL;
  OVERLAPPED WriteOL;
  COMMTIMEOUTS cto;
};

#endif //_CDEVICEUSB_H_

