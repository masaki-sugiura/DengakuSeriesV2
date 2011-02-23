//	$Id: refcount.cpp,v 1.2 2007/03/05 13:04:54 sugiura Exp $
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

