///////////////////////////////////////////////////////////////////////////////
// Descricao:
// Autor: Amadeu Vilar Filho / Luis Gustavo de Brito
// Data de criacao: 11/12/2007
//
// Alteracoes:
//
///////////////////////////////////////////////////////////////////////////////

// CThread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _XHSTAPI_CTHREAD_H_
#define _XHSTAPI_CTHREAD_H_

#include <CTimeUtils.h> 

class CThread  
{
public:
	CThread ();

	virtual ~CThread ();

	void start ();
	bool stop();
	bool join ();
	bool isRunning ();
//	unsigned long getThreadId() const;
	
	//static void waitAllThreadsToDie (bool currentIsCThread);
	// static unsigned long getCurrentThreadId ();
	
protected:

	virtual void* run () = 0;
	bool isToExit ();

private :
	bool m_running;
	void* m_tid;
	bool m_exit;
	bool m_allExit;
	int m_threadObjectsCounter;

	static void* dispatch (void* param);
};

#endif // _CTHREAD_H_
