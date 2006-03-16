//	$Id: hmjre_mngr.cpp,v 1.3 2006-03-16 14:46:56 sugiura Exp $
/*
 *	hmjre_mngr.cpp
 *	HmJre_Manager クラスの実装
 */

#include "hmjre_mngr.h"

HmJre_Manager::HmJre_Manager(const StringBuffer& filename)
	:	REDLL_Manager(filename)
{
	m_pfnJreGetVersion = (PFN_JREGETVERSION)::GetProcAddress(m_hModuleDll,HMJRE_JREGETVERSION);
	m_pfnDecodeEscSeq = (PFN_DECODEESCSEQ)::GetProcAddress(m_hModuleDll,HMJRE_DECODEESCSEQ);
	m_pfnGetJreMessage = (PFN_GETJREMESSAGE)::GetProcAddress(m_hModuleDll,HMJRE_GETJREMESSAGE);
	m_pfnJre2Open = (PFN_JRE2OPEN)::GetProcAddress(m_hModuleDll,HMJRE_JRE2OPEN);
	m_pfnJre2Compile = (PFN_JRE2COMPILE)::GetProcAddress(m_hModuleDll,HMJRE_JRE2COMPILE);
	m_pfnJre2GetMatchInfo
		= (PFN_JRE2GETMATCHINFO)::GetProcAddress(m_hModuleDll,HMJRE_JRE2GETMATCHINFO);
	m_pfnJre2Close = (PFN_JRE2CLOSE)::GetProcAddress(m_hModuleDll,HMJRE_JRE2CLOSE);
	if (m_pfnJreGetVersion == NULL ||
		m_pfnDecodeEscSeq == NULL || m_pfnGetJreMessage == NULL ||
		m_pfnJre2Open == NULL || m_pfnJre2Compile == NULL ||
		m_pfnJre2GetMatchInfo == NULL || m_pfnJre2Close == NULL)
		throw DllProcAddressNotFoundError();
}

HmJre_Manager::~HmJre_Manager()
{
	m_Jre_Pool.initSequentialGet();
	JRE2* pJre2;
	while ((pJre2 = m_Jre_Pool.getNextItem())) {
		(*m_pfnJre2Close)(pJre2);
	}
}

void
HmJre_Manager::setErrorMessage()
{
	int len = (*m_pfnGetJreMessage)(0, GJM_JPN, NULL, 0);
	if (!len) {
		m_strErrMsg = "メッセージの取得に失敗";
		return;
	}

	LPSTR buf = new char[len + 1];
	(*m_pfnGetJreMessage)(0, GJM_JPN, buf, len + 1);

	m_strErrMsg = buf;

	delete [] buf;
}

DWORD
HmJre_Manager::match(const StringBuffer& ptn, const StringBuffer& str, int nFlags)
{
	JRE2* pJre2 = m_htblJre.getValue(ptn);
	if (!pJre2) {
		// 初めて登場するパターン
		try {
			pJre2 = new JRE2;
		} catch (...) {
			return REDLL_RESULT_FAILED;
		}

		if (!(*m_pfnJre2Compile)(pJre2, (LPCSTR)ptn)) {
			setErrorMessage();
			return REDLL_RESULT_FAILED;
		}

		m_htblJre.setValue(ptn, pJre2);
		m_Jre_Pool.addItem(pJre2);
	}

	pJre2->nStart = 0;
	pJre2->wTranslate = (nFlags != 0);

	if (!(*m_pfnJre2GetMatchInfo)(pJre2, (LPCSTR)str)) {
		setErrorMessage();
		return REDLL_RESULT_FAILED;
	}

	DWORD result = make_dword(pJre2->nPosition, pJre2->nLength);

	return result;
}

const StringBuffer&
HmJre_Manager::jreGetVersion()
{
	if (m_strVersion.length() == 0) {
		WORD wVer = (*m_pfnJreGetVersion)();
		char buf[80];
		wsprintf(buf, "%01x.%02x", (wVer >> 8), (wVer & 0xFF));
		m_strVersion = buf;
	}
	return m_strVersion;
}

