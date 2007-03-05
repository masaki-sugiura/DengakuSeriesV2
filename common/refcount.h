//	$Id: refcount.h,v 1.2 2007-03-05 13:04:54 sugiura Exp $
/*
 *	refcount.h
 *	参照数カウントの基底クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_REFCOUNT
#define	DENGAKUSERIES_CLASSES_REFCOUNT

#include <windows.h>

class RCObject {
public:
	void addReference()
	{
		++m_refCount;
	}
	void removeReference()
	{
		if (--m_refCount <= 0) delete this;
	}

	void markUnshareable()
	{
		m_bShareable = FALSE;
	}
	void markShareable()
	{
		m_bShareable = TRUE;
	}

	BOOL isShareable() const
	{
		return m_bShareable;
	}
	BOOL isShared() const
	{
		return m_refCount > 1;
	}

protected:
	RCObject();
	RCObject(const RCObject&);
	virtual ~RCObject() = 0;

	RCObject& operator=(const RCObject&)
	{
		return *this;
	}

private:
	int m_refCount;
	BOOL m_bShareable;
};

template<class T>
class RCPtr {
public:
	RCPtr(T* realPtr = NULL)
		: m_ptr(realPtr)
	{
		init();
	}
	RCPtr(const RCPtr& rhs)
		: m_ptr(rhs.m_ptr)
	{
		init();
	}
	~RCPtr()
	{
		if (m_ptr != NULL) m_ptr->removeReference();
	}

	RCPtr& operator=(const RCPtr& rhs)
	{
		if (m_ptr != rhs.m_ptr) {
			if (m_ptr != NULL) m_ptr->removeReference();
			m_ptr = rhs.m_ptr;
			init();
		}
		return *this;
	}

	T* operator->() const { return m_ptr; }
	T& operator*() const { return *m_ptr; }

private:
	T* m_ptr;

	void init()
	{
		if (m_ptr == NULL) return;
		if (!m_ptr->isShareable()) m_ptr = new T(*m_ptr);
		m_ptr->addReference();
	}
};

#endif
