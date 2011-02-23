//	$Id: stack.cpp,v 1.1 2002/03/29 17:11:59 sugiura Exp $
/*
 *	stack.h
 *	スタッククラス(テンプレート)
 */

#include "stack.h"

RawStack::RawStack(int num)
	: m_stack(num <= 0 ? 8 : num),
	  m_init_num(m_stack.size()),
	  m_hdr(0)
{
}

RawStack::~RawStack()
{
}

int
RawStack::push_(LPCVOID val)
{
	if (m_hdr >= m_stack.size()) {
		m_stack.resize(m_stack.size() + m_init_num);
	}
	m_stack[m_hdr++] = const_cast<LPVOID>(val);
	return m_hdr;
}

LPVOID
RawStack::pop_()
{
	if (m_hdr <= 0) return NULL;
	return m_stack[--m_hdr];
}

LPVOID*
RawStack::gethead_() const
{
	m_stack[m_hdr] = NULL;
	return m_stack;
}

