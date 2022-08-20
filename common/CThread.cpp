/**
  @file CThread.cpp

  @brief Thread handling class

  @details This class helps handling threads avoiding conflicts of types of methods and variables.
      To use this class, entend it in a new class, for instance connectionThread.cpp
      In this class, just create the constructor, desctructor and the method void run().
      In your program you can instanciate this class and call the method start to start the thread.
      This method run() is the method that will be running while the thread is running.
      When the method run() ends, the thread ends as well

  @ingroup GrCISLibs

  @author CIS/Ie

  @version 1.000
  History\n

  @todo

  @warning

  @mainpage 

  @image html cis.png

     */

#include "CThread.h"
//#include "CTimeUtils.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#ifdef WIN32
  #include "CTimeUtils.h"
#endif
CThread::CThread () : m_running( false ), m_exit ( false )
{
  m_tid = new pthread_t;
  // printf("CThread Constructor.\n");
  #ifdef WIN32
    #ifndef TDM_64
      ((pthread_t*)m_tid)->p = NULL;
      ((pthread_t*)m_tid)->x = 0;
    #endif
  #endif
}

// destrutor
CThread::~CThread ()
{
  // printf("CThread Destructor.\n");
  delete (pthread_t*)m_tid;
}

void CThread::start ()
{
  // printf("CThread start.\n");
  if ( !m_running )                   
  {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 0x40000);
    m_exit = false;
    // printf("CThread creating thread.\n");
    ::pthread_create((pthread_t*)m_tid, 
                  &attr, 
                  &CThread::dispatch, 
                  this);
    // printf("CThread thread created.\n");
  }
}


/*========================================================================================
     Fnc: void* CThread::dispatch (void* param)
  ===========================================*/ /**

  @brief

  @details

  @param none

  @return none

  @author Ie

  @warning

  */
void* CThread::dispatch (void* param)
{
  void* ret = NULL;
  
  ( (CThread*)param )->m_running = true;

  //printf("CThread starting to run. m_exit[%d]\n", ( (CThread*)param )->m_exit);
  ret = (( CThread* )param)->run();
  // printf("CThread finished.\n");

  ( (CThread*)param )->m_running = false;

  return ret;
}

/*========================================================================================
     Fnc: bool CThread::join ()
  ===========================================*/ /**

  @brief "Derruba" e aguarda a finalização da thread

  @details

  @param none

  @return true - sucesso, false - thread chamadora é a propria

  @author Ie

  @warning

  */
bool CThread::join ()
{
  bool ret = false;

  if (pthread_equal (pthread_self(), *(pthread_t*)m_tid) == 0)                              //  Se a thread chamadora (pthread_self()) NÃO é a atual (m_tip)
  {                                                                                         //  [
    m_exit = true;                                                                          //    marca para sair

    if (pthread_join(*(pthread_t*)m_tid, NULL) == 0)                                        //    Aguarda a thread atual finalizar
    {                                                                                       //    [
      m_exit = false;                                                                       //      limpa a flag de saída
      ret = true;                                                                           //      prepara para retornar sucesso
    }                                                                                       //    ]
  }                                                                                         //  ]
  // printf("Caller thread is the same as the thread to join\n");
  return ret;                                                                               //  Retorna status
}

bool CThread::isToExit ()
{
  return m_exit;
}

bool CThread::stop()
{
  //printf("CThread::stop\n");
  if (m_running) {
    m_exit = true;
    return true;
  }
  else {
    return false;
  }
}

bool CThread::isRunning ()
{
  return m_running;
}

void* CThread::run()
{
  while (isToExit() == false)
  {
      CTimeUtils::msleep( 100);
  }
  return NULL;
}
