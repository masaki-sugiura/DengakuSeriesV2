// $Id: ddeobjs.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
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
	::DdeKeepStringHandle(m_ddeInst,m_Handle);
	::DdeQueryString(m_ddeInst,m_Handle,
					m_str.getBufPtr(),m_str.size(),
					CP_WINANSI);
}

DdeString::DdeString(const DdeString& ds)
	:	m_ddeInst(ds.m_ddeInst),
		m_Handle(ds.m_Handle),
		m_str(ds.m_str)
{
	::DdeKeepStringHandle(m_ddeInst,m_Handle);	//	increment reference count.
}

DdeString&
DdeString::operator=(const DdeString& ds)
{
	if (this != &ds && m_Handle != ds.m_Handle) {
		::DdeFreeStringHandle(m_ddeInst,m_Handle);
		m_Handle = ds.m_Handle;
		::DdeKeepStringHandle(m_ddeInst,m_Handle);
		m_str.reset(ds.m_str);
	}
	return *this;
}

DdeData::~DdeData()
{
	// nothing to do.
}

