
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "CDeviceUSB.h"

CDeviceUSB::CDeviceUSB()
{
  hDevice = NULL;
  ReadOL.hEvent = NULL;
}

CDeviceUSB::~CDeviceUSB()
{
  if( hDevice != NULL) {
    Close();
  }
}

/*========================================================================================
     Fnc: bool CDeviceUSB::Open( const unsigned short vendorId, const unsigned short productId)
  ===========================================*/ /**
  @brief
  @details
  @param    vendorId
  @param    productId
  @return   true or false
  @author   L.Bogea
  @warning
  */
bool CDeviceUSB::Open( const unsigned short vendorId, const unsigned short productId)
{
  HINSTANCE hSetUpApi = NULL;
  HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
  SP_DEVICE_INTERFACE_DATA InterfaceDataStructure = { 0 };
  PSP_DEVICE_INTERFACE_DETAIL_DATA InterfaceDetailStructure = { 0 };
  SP_DEVINFO_DATA DevInfoData = { 0 };
  char devicePath[512] = { 0};
  GUID guid = { 0xa5dcbf10, 0x6530, 0x11D2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } }; //GUID - Globally Unique Identifier - regedit para todas portas CDC
  int iVid;
  int iPid;

  hDevInfo = SetupDiGetClassDevsA( &guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if( hDevInfo == 0) {
    return false;
  }

  InterfaceDataStructure.cbSize = sizeof( SP_DEVICE_INTERFACE_DATA);
  DWORD memberIndex = 0;
  BOOL success = false;

  do {
    success = SetupDiEnumDeviceInterfaces( hDevInfo, NULL, &guid, memberIndex, &InterfaceDataStructure);
    if( success == true) {
      DWORD cbRequired = 0;
      SetupDiGetDeviceInterfaceDetail( hDevInfo, &InterfaceDataStructure, 0, 0, &cbRequired, 0);
      if( ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
        PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = ( PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, cbRequired);
        if( pdidd) {
          pdidd->cbSize = sizeof( *pdidd);
          if( SetupDiGetDeviceInterfaceDetail( hDevInfo, &InterfaceDataStructure, pdidd, cbRequired, &cbRequired, 0)) {
            //pTrace->logAscii( _FL_, _FN_, P_LOW, "[%04d] DevicePath=%s", _LN_, pdidd->DevicePath);
            strncpy( devicePath, pdidd->DevicePath, 511);
            
            HeapFree( GetProcessHeap(), 0, pdidd);
          } 
          else {
            HeapFree( GetProcessHeap(), 0, pdidd);
            success = false;
          }
        } 
      }
      memberIndex++;
      char *ptrs;
      char vid[10] = { 0 };
      char pid[10] = { 0 };
      ptrs = strstr( devicePath, "vid");
      strncpy( vid, ptrs, 8);

      ptrs = strstr( devicePath, "pid");
      strncpy( pid, ptrs, 8);

      printf( "%s %s\n", vid, pid);
      sscanf( &vid[4], "%x", &iVid);
      sscanf( &pid[4], "%x", &iPid);
    }
    else {
      success = false;
      break;
    }
  } while( vendorId != iVid || productId != iPid);

  SetupDiDestroyDeviceInfoList( hDevInfo);

  if( success == false) {
    return false;
  }

  printf( "devicePath=[%s]\n", devicePath);

  hDevice = CreateFile( devicePath, GENERIC_READ | GENERIC_WRITE,
                        FILE_READ_DATA | FILE_WRITE_DATA,
                        NULL,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

  printf( "hDevice=[%d]\n", hDevice);
  if( hDevice == INVALID_HANDLE_VALUE) {
    return false;
  }

  ReadOL.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
  printf( "ReadOL.hEvent=[%d]\n", ReadOL.hEvent);
  if( ReadOL.hEvent == 0)
  {
    CloseHandle( hDevice);
    hDevice = NULL;
    return false;
  }
  // Non-blocking reads is default

  if( !GetCommTimeouts( hDevice, &cto)) {
    printf("*** error %d\n", GetLastError());
    CloseHandle( hDevice);
    hDevice = NULL;
    return false;
  }
  cto.ReadIntervalTimeout = MAXDWORD;
  cto.ReadTotalTimeoutMultiplier = 0;
  cto.ReadTotalTimeoutConstant = 5000;
  cto.WriteTotalTimeoutMultiplier = MAXDWORD;
  cto.WriteTotalTimeoutConstant = MAXDWORD;

  SetCommTimeouts( hDevice, &cto);

  connected = true;
  return true;
}

/*========================================================================================
     Fnc: bool CDeviceUSB::Close()
  ===========================================*/ /**
  @brief
  @details
  @param    none
  @return   true
  @author   L.Bogea
  @warning
  */
bool CDeviceUSB::Close()
{
  if( ReadOL.hEvent) {
    CloseHandle( ReadOL.hEvent);
    ReadOL.hEvent = NULL;
  }
  if( hDevice != NULL && hDevice != INVALID_HANDLE_VALUE) {
    CloseHandle( hDevice);
    hDevice = NULL;
  }
  connected = false;
  return true;
}

/*========================================================================================
     Fnc: int CDeviceUSB::Write (const void* buffer, int length)
  ===========================================*/ /**
  @brief
  @details
  @param    buffer
  @param    length
  @return   number of bytes written
  @author   L.Bogea
  @warning
  */
int CDeviceUSB::Write( const void* buffer, int length)
{
  DWORD size = 0;
  m_lLastError = ERROR_SUCCESS;
  if( connected) {
    if( !WriteFile( hDevice, buffer, length, &size, NULL)) {
      long lLastError = ::GetLastError();
      printf( "[%04d]Write lLastError={%d]\n",__LINE__, lLastError);
      m_lLastError = lLastError;
    }
  }
  else {
    m_lLastError = ERROR_INVALID_HANDLE;
  }
  return m_lLastError;
}

/*========================================================================================
     Fnc: int CDeviceUSB::Write (const char* buffer)
  ===========================================*/ /**
  @brief
  @details
  @param    buffer
  @return   number of bytes written
  @author   L.Bogea
  @warning
  */
int CDeviceUSB::Write( const char* buffer)
{
  DWORD size = 0;
  m_lLastError = ERROR_SUCCESS;
  if( connected) {
    if( !WriteFile( hDevice, buffer, strlen( buffer), &size, NULL)) {
      long lLastError = ::GetLastError();
      printf( "[%04d]Write lLastError={%d]\n",__LINE__, lLastError);
      m_lLastError = lLastError;
    }
  }
  else {
    m_lLastError = ERROR_INVALID_HANDLE;
  }
  return m_lLastError;
}

/*========================================================================================
     Fnc: int CDeviceUSB::Read (unsigned char* buffer, int len, int timeout)
  ===========================================*/ /**
  @brief
  @details
  @param    buffer
  @param    length
  @param    timeout
  @return   number of bytes read
  @author   L.Bogea
  @warning
  */
int CDeviceUSB::Read( unsigned char* buffer, int length, int timeout)
{
  COMSTAT cstat;
  DWORD cerr;
  DWORD numberOfBytesRead = 0;
  m_lLastError = ERROR_SUCCESS;

  if( connected) {
    cto.ReadTotalTimeoutConstant = timeout;
    SetCommTimeouts( hDevice, &cto);

    if( ClearCommError( hDevice, &cerr, &cstat) && cstat.cbInQue) {
      if( !ReadFile( hDevice, buffer, length, &numberOfBytesRead, &ovInternal) ) {
        if( ERROR_IO_PENDING == GetLastError()) {
          if( GetOverlappedResult( hDevice, &ReadOL, &numberOfBytesRead, TRUE)) {
            ResetEvent( ReadOL.hEvent);
          }
        }
      }
    }
  }
  return m_lLastError;
}

/*========================================================================================
     Fnc: bool CDeviceUSB::IsOpen ()
  ===========================================*/ /**
  @brief
  @details
  @param    none
  @return   true or false
  @author   L.Bogea
  @warning
  */
bool CDeviceUSB::IsOpen ()
{
  return connected;
}


