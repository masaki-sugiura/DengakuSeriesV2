//	$Id: stack.h,v 1.4 2002-03-29 17:11:59 sugiura Exp $
/*
 *	stack.h
 *	スタッククラス(テンプレート)
 */

#ifndef	DENGAKUSERIES_CLASSES_STACK
#define	DENGAKUSERIES_CLASSES_STACK

#include "array.h"

class RawStack {
protected:
	RawStack(int num);
	virtual ~RawStack() = 0;

	int push_(LPCVOID val);
	LPVOID pop_();
	LPVOID* gethead_() const;

private:
	mutable Array<LPVOID> m_stack;
	int	m_init_num;
	int	m_hdr;

	RawStack(const RawStack&);
	RawStack& operator=(const RawStack&);
};

template<class T>
class Stack : private RawStack {
public:
	Stack(int n) : RawStack(n) {}

	int push(const T* val)
	{
		return push_(val);
	}
	T* pop()
	{
		return (T*)pop_();
	}
	T** gethead() const
	{
		return (T**)gethead_();
	}
};

#endif
