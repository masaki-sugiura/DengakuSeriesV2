//	$Id: sharedbuf.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	sharedbuf.h
 *	スレッド間で共有可能な文字列バッファ
 */

#ifndef	DENGAKUSERIES_CLASSES_SHAREDBUFFER
#define	DENGAKUSERIES_CLASSES_SHAREDBUFFER

#include "linklist.h"
#include "thread.h"

class Monitor {
public:
	Monitor() { hMutex = ::CreateMutex(NULL,FALSE,NULL); }
	virtual	~Monitor() { ::CloseHandle(hMutex); }

	friend class Condition;

protected:
	HANDLE hMutex;
	virtual DWORD lock() { return WaitObjectWithDispatchMsg(hMutex,INFINITE); }
	virtual void release() { ::ReleaseMutex(hMutex); }
};

class Semaphore {
public:
	Semaphore() { m_hSemaphore = ::CreateSemaphore(NULL,0,1,NULL); }
	~Semaphore() { ::CloseHandle(m_hSemaphore); }
	DWORD P(DWORD wait) { return WaitObjectWithDispatchMsg(m_hSemaphore,wait); }
	void V() { ::ReleaseSemaphore(m_hSemaphore,1,NULL); }

protected:
	HANDLE m_hSemaphore;
};

class Condition {
public:
	Condition(Monitor* mon) : m_semaCount(0), m_monitor(mon) {}
	~Condition() {}

	DWORD wait(int wait = 100)
	{
		m_semaCount++;
		m_monitor->release();
		DWORD ret = m_semaphore.P(wait);
		m_monitor->lock();
		m_semaCount--;
		return ret;
	}

	void signal()
	{
		if (m_semaCount > 0) m_semaphore.V();
	}

protected:
	Semaphore m_semaphore;
	int m_semaCount;
	Monitor* m_monitor;
};

template<class T>
class SharedBuffer : public Monitor {
public:
	SharedBuffer()
	{
		m_notEmpty = new Condition(this);
	}
	~SharedBuffer()
	{
		this->reset();
		delete m_notEmpty;
	}

	BOOL get(T& buf, int wait)
	{
		this->lock();
		while (m_queue.itemNum() <= 0) {
			if (m_notEmpty->wait(wait) != WAIT_OBJECT_0) {
				this->release();
				return FALSE;
			}
		}
		buf = *m_queue.getItemByIndex(0);
		m_queue.delItemByIndex(0);
		this->release();
		return TRUE;
	}

	void set(const T& newvalue)
	{
		this->lock();
		if (m_queue.addItem(new T(newvalue),-1) > 0) {
			m_notEmpty->signal();
		}
		this->release();
	}

	void reset()
	{
		this->lock();
		if (m_queue.itemNum() > 0) {
			for (int i = m_queue.itemNum(); i > 0; i--)
				m_queue.delItemByIndex(0);
		}
		this->release();
	}

private:
	LinkList<T> m_queue;
	Condition* m_notEmpty;
};

#endif
