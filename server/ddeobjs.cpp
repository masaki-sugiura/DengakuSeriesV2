// $Id: ddeobjs.cpp,v 1.3 2007-03-04 18:06:56 sugiura Exp $
/*
 *	ddeobjs.cpp
 *	DdeString ƒNƒ‰ƒX‚ÌŽÀ‘•
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
		DebugOutput("DdeKeepStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst));
	}
	if (::DdeQueryString(m_ddeInst,m_Handle,
						 m_str.getBufPtr(),m_str.size(),
						 CP_WINANSI) == 0) {
		DebugOutput("DdeQueryString() error = %08x", ::DdeGetLastError(m_ddeInst));
	}
}

DdeString::DdeString(const DdeString& ds)
	:	m_ddeInst(ds.m_ddeInst),
		m_Handle(ds.m_Handle),
		m_str(ds.m_str)
{
	if (!::DdeKeepStringHandle(m_ddeInst,m_Handle)) {	//	increment reference count.
		DebugOutput("DdeKeepStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst));
	}
}

DdeString&
DdeString::operator=(const DdeString& ds)
{
	if (this != &ds && m_Handle != ds.m_Handle) {
		if (!::DdeFreeStringHandle(m_ddeInst,m_Handle)) {
			DebugOutput("DdeFreeStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst));
		}
		m_Handle = ds.m_Handle;
		if (!::DdeKeepStringHandle(m_ddeInst,m_Handle)) {
			DebugOutput("DdeKeepStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst));
		}
		m_str = ds.m_str;
	}
	return *this;
}

DdeData::~DdeData()
{
	// nothing to do.
}

