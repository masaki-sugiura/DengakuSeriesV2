// $Id: ddeobjs.cpp,v 1.5 2007/04/29 16:11:35 sugiura Exp $
/*
 *	ddeobjs.cpp
 *	DdeString クラスの実装
 */

#include "ddeobjs.h"

DdeString::DdeString(DWORD ddeInst, const StringBuffer& str)
	:	m_ddeInst(ddeInst),
		m_Handle(::DdeCreateStringHandle(ddeInst,str,CP_WINANSI)),
		m_str(str)
{
	//	nothing to do more.
}

DdeString::DdeString(DWORD ddeInst, const HSZ hstr)
	:	m_ddeInst(ddeInst),
		m_Handle(hstr),
		m_str(::DdeQueryString(ddeInst,hstr,NULL,0,CP_WINANSI)+1)
{
	if (!::DdeKeepStringHandle(m_ddeInst,m_Handle)) {
		DEBUG_OUTPUT(("DdeKeepStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst)));
	}
	if (::DdeQueryString(m_ddeInst,m_Handle,
						 m_str.getBufPtr(),m_str.size(),
						 CP_WINANSI) == 0) {
		DEBUG_OUTPUT(("DdeQueryString() error = %08x", ::DdeGetLastError(m_ddeInst)));
	}
}

DdeString::DdeString(const DdeString& ds)
	:	m_ddeInst(ds.m_ddeInst),
		m_Handle(ds.m_Handle),
		m_str(ds.m_str)
{
	if (!::DdeKeepStringHandle(m_ddeInst,m_Handle)) {	//	increment reference count.
		DEBUG_OUTPUT(("DdeKeepStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst)));
	}
}

DdeString&
DdeString::operator=(const DdeString& ds)
{
	if (this != &ds && m_Handle != ds.m_Handle) {
		if (!::DdeFreeStringHandle(m_ddeInst,m_Handle)) {
			DEBUG_OUTPUT(("DdeFreeStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst)));
		}
		m_Handle = ds.m_Handle;
		if (!::DdeKeepStringHandle(m_ddeInst,m_Handle)) {
			DEBUG_OUTPUT(("DdeKeepStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst)));
		}
		m_str = ds.m_str;
	}
	return *this;
}

DdeData::~DdeData()
{
	// nothing to do.
}

