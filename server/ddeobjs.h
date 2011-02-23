// $Id: ddeobjs.h,v 1.4 2007/04/29 16:11:35 sugiura Exp $
/*
 *	ddeobjs.h
 *	DDE 通信で使うデータをラップするクラスの定義
 */

#ifndef	DENGAKUSERVER_CLASSES_DDEOBJS
#define	DENGAKUSERVER_CLASSES_DDEOBJS

#include "strbuf.h"
#include "misc.h"

//	DDE 文字列
class DdeString {
public:
	DdeString(DWORD, const StringBuffer&);
	DdeString(DWORD, const HSZ);
	DdeString(const DdeString&);

	~DdeString()
	{
		if (!::DdeFreeStringHandle(m_ddeInst,m_Handle)) {
			DEBUG_OUTPUT(("DdeFreeStringHandle() error = %08x", ::DdeGetLastError(m_ddeInst)));
		}
	}

	operator const HSZ() const
	{
		return this->getHandle();
	}

	operator LPCSTR() const
	{
		return (LPCSTR)m_str;
	}

	operator const StringBuffer&() const
	{
		return m_str;
	}

	DdeString& operator=(const DdeString&);

	const HSZ getHandle() const
	{
		return m_Handle;
	}

	int length() const
	{
		return m_str.length();
	}

	int compareTo(const HSZ hstr) const
	{
		return ::DdeCmpStringHandles(m_Handle,hstr);
	}

private:
	DWORD m_ddeInst;
	HSZ m_Handle;
	StringBuffer m_str;
};

//	DDE データの基底クラス
class DdeData {
public:
	virtual	~DdeData() = 0;	//	pure virtual

	operator LPCSTR() const { return (LPCSTR)m_str; }
	operator const StringBuffer&() const { return m_str; }

	HDDEDATA getHandle() const { return m_hData; }

protected:
	HDDEDATA m_hData;
	StringBuffer m_str;

	DdeData(HDDEDATA hData, const StringBuffer& strData = nullStr)
		:	m_hData(hData), m_str(strData)
	{}	//	disable calling from other classes/functions.
};

//	HDDEDATA から文字列を取り出すクラス
class TemporalDdeData : public DdeData {
public:
	TemporalDdeData(HDDEDATA hData)
		:	DdeData(hData)
	{
		if (hData != NULL) {
			//	StringBuffer の使い方としてはよくない方法だが、
			//	速度が要求される部分だけに…
			DWORD dwlen = ::DdeGetData(hData,NULL,0,0);
			m_str.resize(dwlen);
			::DdeGetData(hData,(UCHAR*)m_str.getBufPtr(),dwlen+1,0);
		}
	}
	//	破棄時に DdeFreeDataHandle() は呼ばない
};

//	受信用 DDE データ
class ReceivedDdeData : public TemporalDdeData {
public:
	ReceivedDdeData(HDDEDATA hData)
		:	TemporalDdeData(hData)
	{
		//	nothing to do.
	}
	~ReceivedDdeData()
	{
		if (!::DdeFreeDataHandle(m_hData)) {
			DEBUG_OUTPUT(("DdeFreeStringHandle()!!"));
		}
	}
};

//	送信用 DDE データ
class SendingDdeData : public DdeData {
public:
	SendingDdeData(DWORD ddeInst,
				const HSZ hszContent,
				const StringBuffer& strData)
		:	DdeData(NULL,strData)
	{
		m_hData = ::DdeCreateDataHandle(ddeInst,
										(UCHAR*)m_str.getBufPtr(),
										m_str.length() + 1,0,
										hszContent,
										CF_TEXT,
										0);
		if (m_hData == NULL) {
			DEBUG_OUTPUT(("DdeCreateDataHandle() error = %08x", ::DdeGetLastError(ddeInst)));
		}
	}

	//	破棄時に DdeFreeDataHandle() を呼ばないことに注意！！
};

#endif

