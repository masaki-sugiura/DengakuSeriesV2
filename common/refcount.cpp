//	$Id: refcount.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	refcount.cpp
 *	�Q�Ɛ��J�E���g�̊��N���X
 */

#include "refcount.h"

RCObject::RCObject()
	:	m_refCount(0),
		m_bShareable(TRUE)
{}

RCObject::RCObject(const RCObject&)
	:	m_refCount(0),
		m_bShareable(TRUE)
{}

RCObject::~RCObject()
{}

