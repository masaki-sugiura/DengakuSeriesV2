//	$Id: thread.cpp,v 1.2 2006/03/16 14:46:56 sugiura Exp $
/*
 *	thread.cpp
 *	Thread ƒNƒ‰ƒX‚ÌŽÀ‘•
 */

#include "thread.h"

Thread::Thread(LPTHREAD_START_ROUTINE pfnThreadProc, LPVOID pParam)
	: m_hThread(NULL),
	  m_dwThreadID(0),
	  m_dwExitCode(THREAD_INVALID_EXITCODE),
	  m_pfnThreadProc(pfnThreadProc),
	  m_pParam(pParam)
{
	//	nothing to do more.
}

Thread::~Thread()
{
	if (m_hThread != NULL) this->stop(1000);
}

BOOL
Thread::run(BOOL bSuspend)
{
	if (m_hThread != NULL || m_pfnThreadProc == NULL)
		return FALSE;
	m_hThread = ::CreateThread(NULL,0,
								m_pfnThreadProc,
								m_pParam,
								bSuspend ? CREATE_SUSPENDED : 0,
								&m_dwThreadID);
	if (m_hThread != NULL) return TRUE;
	m_dwExitCode = THREAD_INVALID_EXITCODE;
	return FALSE;
}

DWORD
Thread::stop(DWORD wait)
{
	if (m_hThread == NULL) return WAIT_OBJECT_0;
	DWORD ret = this->wait(wait);
	if (ret != WAIT_OBJECT_0) {
		::TerminateThread(m_hThread,THREAD_INVALID_EXITCODE);
		::CloseHandle(m_hThread);
		m_hThread = NULL;
		m_dwExitCode = THREAD_INVALID_EXITCODE;
	}
	return ret;
}

DWORD
Thread::wait(DWORD wait)
{
	if (m_hThread == NULL) return THREAD_INVALID_EXITCODE;
	DWORD ret = WaitObjectWithDispatchMsg(m_hThread,wait);
	if (ret == WAIT_OBJECT_0) {
		::GetExitCodeThread(m_hThread,&m_dwExitCode);
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	return ret;
}

DWORD
Thread::resume()
{
	return ::ResumeThread(m_hThread);
}

DWORD
Thread::suspend()
{
	return ::SuspendThread(m_hThread);
}

#if 0
DWORD
WaitObjectWithDispatchMsg(HANDLE hObj, DWORD time)
{
	DWORD ret = WAIT_TIMEOUT;
	MSG msg;
	for (;;) {
		ret = ::MsgWaitForMultipleObjects(1, &hObj, FALSE, time, QS_ALLINPUT);
		if (ret != WAIT_OBJECT_0 + 1) break;
		while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return ret;
}

#endif

