//	$Id: stack.h,v 1.3 2002-02-19 15:34:22 sugiura Exp $
/*
 *	stack.h
 *	スタッククラス(テンプレート)
 */

#ifndef	DENGAKUSERIES_CLASSES_STACK
#define	DENGAKUSERIES_CLASSES_STACK

#include "array.h"

template<typename T>
class Stack {
public:
	Stack(int num)
		: m_stack(num <= 0 ? 8 : num),
		  m_init_num(m_stack.size()),
		  m_hdr(0)
	{}

	int push(const T* val)
	{
		if (m_hdr >= m_stack.size()) {
			m_stack.resize(m_stack.size() + m_init_num);
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
	mutable Array<T*> m_stack;
	int	m_init_num;
	int	m_hdr;

	Stack(const Stack&);
	Stack& operator=(const Stack&);
};

#endif
