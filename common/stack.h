//	$Id: stack.h,v 1.2 2002-01-16 15:57:23 sugiura Exp $
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
		m_init_num = m_num = num;
		m_hdr = 0;
		m_stack = new T*[m_num];
	}
	virtual ~Stack()
	{
		delete [] m_stack;
	}

	int push(const T* val)
	{
		if (m_hdr >= m_num) {
			T** new_stack = new T*[m_num + m_init_num];
			::CopyMemory(new_stack, m_stack, sizeof(T*) * m_num);
			delete [] m_stack;
			m_stack = new_stack;
		}
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
		m_stack[m_hdr] = NULL;
		return m_stack;
	}

protected:
	T**	m_stack;
	int	m_init_num, m_num;
	int	m_hdr;

	Stack(const Stack&);
	Stack& operator=(const Stack&);
};

#endif
