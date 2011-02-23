//	$Id: thread.h,v 1.2 2006/03/16 14:46:56 sugiura Exp $
/*
 *	thread.h
 *	スレッドを生成＆管理する(基底)クラス
 */

#ifndef DENGAKUSERIES_CLASSES_THREAD
#define DENGAKUSERIES_CLASSES_THREAD

#include <windows.h>

#define THREAD_INVALID_EXITCODE 0x80000000

class Thread {
public:
	Thread(LPTHREAD_START_ROUTINE, LPVOID pParam = NULL);
	virtual ~Thread();

	DWORD getThreadID() const { return m_dwThreadID; }
	DWORD getExitCode() const { return m_dwExitCode; }
	LPVOID getProcArg() const { return m_pParam; }

	virtual BOOL run(BOOL bSuspend = FALSE);
	virtual DWORD stop(DWORD waittime = INFINITE);
	virtual DWORD wait(DWORD waittime = INFINITE);
	virtual DWORD resume();
	virtual DWORD suspend();

protected:
	HANDLE m_hThread;
	DWORD m_dwThreadID;
	DWORD m_dwExitCode;
	LPTHREAD_START_ROUTINE m_pfnThreadProc;
	LPVOID m_pParam;
};

#if 0
DWORD WaitObjectWithDispatchMsg(HANDLE hObj, DWORD time);
#endif
#define WaitObjectWithDispatchMsg(hObj,time) ::WaitForSingleObject(hObj,time)

#endif
