//	$Id: stack.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	stack.h
 *	スタッククラス(テンプレート)
 */

#ifndef	DENGAKUSERIES_CLASSES_STACK
#define	DENGAKUSERIES_CLASSES_STACK

#include <windows.h>

template<typename T>
class Stack {
public:
	Stack(int num)
	{
		if (num <= 0) num = 8;
		m_num = num;
		m_hdr = 0;
		m_stack = new T*[m_num];
		::ZeroMemory(m_stack,m_num * sizeof(T*));
	}
	virtual ~Stack()
	{
		delete [] m_stack;
	}

	int push(const T* val)
	{
		if (m_hdr >= m_num) return 0;
		m_stack[m_hdr++] = const_cast<T*>(val);
		return m_hdr;
	}

	T* pop()
	{
		if (m_hdr <= 0) return NULL;
		T* val = m_stack[--m_hdr];
		return val;
	}

	T** gethead() const
	{
		return m_stack;
	}

protected:
	T**	m_stack;
	int	m_num;
	int	m_hdr;

	Stack(const Stack&);
	Stack& operator=(const Stack&);
};

#endif
