//	$Id: refcount.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	refcount.cpp
 *	参照数カウントの基底クラス
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

