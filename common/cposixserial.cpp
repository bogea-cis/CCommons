// CPosixSerial.cpp: implementation of the CPosixSerial class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <errno.h>   /* Error number definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <poll.h>   
#include <sys/ioctl.h>
#include "CLogger.h"


//select
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

#include "cposixserial.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLogger* m_trace = NULL;

CPosixSerial::CPosixSerial()
{
  m_fd = -1;
  m_trace = new CLogger("/home/term/base/log/cposixserial", "", 5, 5 * 1024 * 1024);
}

CPosixSerial::~CPosixSerial()
{

}

bool CPosixSerial::Open (const char* device, EBaudrate baund, EDataBits databits, EParity parity, EStopBits stop)
{
  bool ret = false;

  m_fd = -1;
  m_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

  if (m_fd != -1)
  {
    fcntl(m_fd, F_SETFL, O_NONBLOCK);

    this->SetBaundRate(baund);
    this->SetDataBits(databits);
    this->SetParity(parity);
    this->SetStopbits(stop);

    struct termios options;
    bzero(&options, sizeof(options)); /* clear struct for new port settings */
    tcgetattr(m_fd, &options); /* save current serial port settings */

    options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag     &= ~OPOST;
    options.c_iflag     &= ~(ICRNL | INLCR);
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 1;

    /* set the options */
    tcsetattr(m_fd, TCSANOW, &options);

    this->Purge();
    
    ret = true;
  }
  return ret;

}

bool CPosixSerial::Close ()
{
  close(m_fd);
  m_fd = -1;

  return true;
}


bool CPosixSerial::SetBaundRate (EBaudrate baund)
{
  struct termios options;

  /*
   * Get the current options for the port...
   */

  tcgetattr(m_fd, &options);

  /*
   * Set the baud rates to 19200...
   */

  cfsetispeed(&options, baund);
  cfsetospeed(&options, baund);

  /*
   * Enable the receiver and set local mode...
   */

  options.c_cflag |= (CLOCAL | CREAD);

  /*
   * Set the new options for the port...
   */

  return tcsetattr(m_fd, TCSANOW, &options) == 0;
}

bool CPosixSerial::SetDataBits (EDataBits databits)
{
  struct termios options;

  tcgetattr(m_fd, &options);

  options.c_cflag &= ~CSIZE; /* Mask the character size bits */
  options.c_cflag |= databits;    /* Select 8 data bits */


  options.c_cflag |= (CLOCAL | CREAD);
  return tcsetattr(m_fd, TCSANOW, &options) == 0;
}

bool CPosixSerial::SetParity (EParity parity)
{
  struct termios options;

  tcgetattr(m_fd, &options);


  switch (parity)
  {
    case CPosixSerial::EParNone:
    {
      options.c_cflag &= ~PARENB;
    }
    break;
    case CPosixSerial::EParOdd:
    {
      options.c_cflag |= PARENB;
      options.c_cflag |= PARODD;
    }
    break;
    case CPosixSerial::EParEven:
    {
      options.c_cflag |= PARENB;
      options.c_cflag &= ~PARODD;

    }
    break;
    case CPosixSerial::EParSpace:
    {
      options.c_cflag &= ~PARENB;
    }
    break;
    default:
    {

    }
    break;
  }

  options.c_cflag |= (CLOCAL | CREAD);
  return tcsetattr(m_fd, TCSANOW, &options) == 0;
}

bool CPosixSerial::SetStopbits (EStopBits stop)
{
  struct termios options;

  tcgetattr(m_fd, &options);

  switch (stop)
  {
    case CPosixSerial::EStop1:
    {
      options.c_cflag &= ~CSTOPB;
    }
    break;

    case CPosixSerial::EStop2:
    {
      options.c_cflag &= CSTOPB;
    }
    break;

    default:
    {

    }
    break;
  }

  options.c_cflag |= (CLOCAL | CREAD);
  return tcsetattr(m_fd, TCSANOW, &options) == 0;
}

bool CPosixSerial::EnableSoftwareFlowControl()
{
  struct termios options;

  tcgetattr(m_fd, &options);

  options.c_iflag |= (IXON | IXOFF | IXANY);

  return tcsetattr(m_fd, TCSANOW, &options) == 0;
}

bool CPosixSerial::DisableSoftwareFlowControl()
{
  struct termios options;

  tcgetattr(m_fd, &options);

  options.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR);

  return tcsetattr(m_fd, TCSANOW, &options) == 0;
}

bool CPosixSerial::EnableHardwareFlowControl()
{
  struct termios options;

  tcgetattr(m_fd, &options);

  options.c_iflag |= CRTSCTS;

  return tcsetattr(m_fd, TCSANOW, &options) == 0;
}

bool CPosixSerial::DisableHardwareFlowControl()
{
  struct termios options;

  tcgetattr(m_fd, &options);

  options.c_iflag &= ~CRTSCTS;

  return tcsetattr(m_fd, TCSANOW, &options) == 0;
}

int CPosixSerial::Write (const char* buffer)
{
  return this->Write ((void*)buffer, strlen(buffer));
}

int CPosixSerial::Write (const void* buffer, int len)
{
  return write(m_fd, buffer, len);
}

/*  ========================================================================================
     Fnc: int CPosixSerial::Read (unsigned char* buffer, int len, int timeout);
     ================================ */ /**

  @brief Faz a leitura da serial de "len" bytes até um timeout de "timeout"
  
  @details
  
  @param none 
  
  @return numero de bytes lidos
          0 - Timeout
          -1 se deu erro de read 
          -2 TImeout com dados incompletos
  
  @author Ie 
  
  @warning 
  
  */     
int CPosixSerial::Read (unsigned char* buffer, int len, int timeout)
{
  bool exit = false;
  int idx = 0;
  int rc;
  bool bTmpFlag;

  while (!exit)                                                                             //  Enquanto não terminar por timeout ou total de dados
  {                                                                                         //  [
//  	m_trace->logAscii(_FL_, _FN_, P_LOW,                                      
//  	        "[%04d] CheckBytesAvailable..Called", _LN_);                            
    bTmpFlag=this->CheckBytesAvailable(timeout);                                            //    Verifica se tem dados diponiveis
//  	m_trace->logAscii(_FL_, _FN_, P_LOW,                                      
//  	        "[%04d] CheckBytesAvailable..returned[%d]", _LN_, (int) bTmpFlag);      
    if (bTmpFlag)                                                                           //    Se tem dados disponíveis
    {                                                                                       //    [
//    	m_trace->logAscii(_FL_, _FN_, P_LOW, "[%04d] read..Called", _LN_);                                               
      rc = read (m_fd, &buffer[idx], 1);                                                    //      Lê 1 byte
//    	m_trace->logAscii(_FL_, _FN_, P_LOW,                                    
//    	        "[%04d] read..returned[%d]", _LN_, rc);                                   
                                                                                            
      if (rc > 0)                                                                           //      Se não deu erro na leitura
      {                                                                                     //      [
        idx++;                                                                              //        Incrementa contador de dados lidos
        if (idx == len)                                                                     //        Se já completou a quantidade esperada
        {                                                                                   //        [
          exit = true;                                                                      //          Prepara para sair do loop
        }                                                                                   //        ]
      }                                                                                     //      ]
      else                                                                                  //      Senao (Erro no read)
      {                                                                                     //      [
        exit = true;                                                                        //        prepara para sair do loop
        idx = -1;                                                                           //        prepara para retornar erro
      }                                                                                     //      ]
    }                                                                                       //    ]
    else                                                                                    //    Senao (timeout)
    {                                                                                       //    [
      if (idx != 0) {                                                                       //      Se já recebeu algo e deu timeout
        idx = -2;                                                                           //        Sai com erro de dado incompleto
      }                                                                                     //      ]
      exit = true;                                                                          //      aborta o loop
    }                                                                                       //    ]
  }                                                                                         //  ]

//	m_trace->logAscii(_FL_, _FN_, P_LOW, "[%04d] Leaving: %d", _LN_, idx);                                               
  return idx;                                                                               //
}


void CPosixSerial::ClearSignal (int signal)
{
  int status;
  ioctl(m_fd, TIOCMGET, &status);
  status &= ~signal;
  ioctl(m_fd, TIOCMSET, &status);
}

void CPosixSerial::SetSignal (int signal)
{
  int status;
  ioctl(m_fd, TIOCMGET, &status);
  status &= signal;
  ioctl(m_fd, TIOCMSET, &status);
}

bool CPosixSerial::GetSignal (int signal)
{
  bool ret = false;
  int status;
  ioctl(m_fd, TIOCMGET, &status);
  if ((status & signal) == signal)
  {
    ret = true;
  }

  return ret;
}

void CPosixSerial::SetRTS ()
{
  SetSignal (TIOCM_RTS);
}

void CPosixSerial::SetCTS ()
{
  SetSignal (TIOCM_CTS);
}

void CPosixSerial::SetDTR ()
{
  SetSignal (TIOCM_DTR);
}

void CPosixSerial::ClearRTS ()
{
  ClearSignal (TIOCM_RTS);
}

void CPosixSerial::ClearCTS ()
{
  ClearSignal (TIOCM_CTS);
}

void CPosixSerial::ClearDTR ()
{
  ClearSignal (TIOCM_DTR);
}


bool CPosixSerial::CheckBytesAvailable ()
{
  return CheckBytesAvailable(50);
}

/*
retorno true - Tem dado na serial
*/
bool CPosixSerial::CheckBytesAvailable (int timeout)
{
  bool ret = false;
  int nfds = 1;
  struct pollfd pfds;
  int pollReady;

  if (m_fd ==  -1) {
    return false;
  }
                               
  pfds.fd = m_fd;
  pfds.events = POLLIN;
  pfds.revents = 0;                                
  pollReady = poll(&pfds, nfds, timeout);
  if (pollReady == 1) {
//   	m_trace->logAscii(_FL_, _FN_, P_LOW, "[%04d] OK pollReady: 1, revents: %04x", _LN_, pfds.revents);                            
    if (pfds.revents & POLLIN) {
      return true;
    }
    else {
//   	  m_trace->logAscii(_FL_, _FN_, P_LOW, "[%04d] ER pollReady: 1, revents: %04x", _LN_, pollReady, pfds.revents);                            
    }
  }
  else if (pollReady < 0) {
//   	m_trace->logAscii(_FL_, _FN_, P_LOW, "[%04d] ER pollReady: %d, revents: %04x", _LN_, pollReady, pfds.revents);                            
  }
  else {
//   	m_trace->logAscii(_FL_, _FN_, P_LOW, "[%04d] pollReady: 0", _LN_);                            
  }
  return false;

//  fd_set readfds;
//  struct timeval tv;
//
//  // clear the set ahead of time
//  FD_ZERO(&readfds);
//
//  // add our descriptors to the set
//  FD_SET(m_fd, &readfds);
//
//  // wait until either socket has data ready to be recv()d
//  tv.tv_sec = timeout / 1000;
//  tv.tv_usec = (timeout % 1000) * 1000;
//
//  int rc = select(m_fd + 1, &readfds, NULL, NULL, &tv);
//
//  if (rc > 0)
//  {
//   	m_trace->logAscii(_FL_, _FN_, P_LOW, "[%04d] select %d", _LN_, rc);                            
//
//    ret = true;
//  }

  return ret;
}

int CPosixSerial::BytesAvailable ()
{
  //Getting the Number of Bytes Available
  //The FIONREAD ioctl gets the number of bytes in the serial port input buffer. As with TIOCMGET you pass in a pointer to an integer to hold the number of bytes, as shown in Listing 7.

  int bytes = 0;
  ioctl(m_fd, FIONREAD, &bytes);

  return bytes;
}

bool CPosixSerial::Purge ()
{
  tcflush(m_fd, TCIFLUSH);
  return true;
}

bool CPosixSerial::IsOpen ()
{
  return (m_fd != -1);
}
