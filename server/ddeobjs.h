// $Id: ddeobjs.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	ddeobjs.h
 *	DDE �ʐM�Ŏg���f�[�^�����b�v����N���X�̒�`
 */

#ifndef	DENGAKUSERVER_CLASSES_DDEOBJS
#define	DENGAKUSERVER_CLASSES_DDEOBJS

#include "strbuf.h"

//	DDE ������
class DdeString {
public:
	DdeString(DWORD, const StringBuffer&);
	DdeString(DWORD, const HSZ);
	DdeString(const DdeString&);

	~DdeString()
	{
		::DdeFreeStringHandle(m_ddeInst,m_Handle);
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

//	DDE �f�[�^�̊��N���X
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

//	HDDEDATA ���當��������o���N���X
class TemporalDdeData : public DdeData {
public:
	TemporalDdeData(HDDEDATA hData)
		:	DdeData(hData)
	{
		if (hData != NULL) {
			//	StringBuffer �̎g�����Ƃ��Ă͂悭�Ȃ����@�����A
			//	���x���v������镔�������Ɂc
			DWORD dwlen = ::DdeGetData(hData,NULL,0,0);
			m_str.resize(dwlen);
			::DdeGetData(hData,(UCHAR*)m_str.getBufPtr(),dwlen+1,0);
		}
	}
	//	�j������ DdeFreeDataHandle() �͌Ă΂Ȃ�
};

//	��M�p DDE �f�[�^
class ReceivedDdeData : public TemporalDdeData {
public:
	ReceivedDdeData(HDDEDATA hData)
		:	TemporalDdeData(hData)
	{
		//	nothing to do.
	}
	~ReceivedDdeData()
	{
		::DdeFreeDataHandle(m_hData);
	}
};

//	���M�p DDE �f�[�^
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
	}

	//	�j������ DdeFreeDataHandle() ���Ă΂Ȃ����Ƃɒ��ӁI�I
};

#endif

