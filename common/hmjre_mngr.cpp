//	$Id: hmjre_mngr.cpp,v 1.1 2003-12-03 17:17:58 sugiura Exp $
/*
 *	hmjre_mngr.cpp
 *	HmJre_Manager ƒNƒ‰ƒX‚ÌŽÀ‘•
 */

#include "hmjre_mngr.h"

HmJre_Manager::HmJre_Manager(const StringBuffer& filename)
	: m_pResultList(NULL)
{
	m_hModuleDll = ::LoadLibrary(filename);
	if (m_hModuleDll == NULL) throw DllNotFoundError();
	m_pfnIsMatch = (PFN_ISMATCH)::GetProcAddress(m_hModuleDll,HMJRE_ISMATCH);
	m_pfnGlobalReplace = (PFN_GLOBALREPLACE)::GetProcAddress(m_hModuleDll,HMJRE_GLOBALREPLACE);
	m_pfnJreGetVersion = (PFN_JREGETVERSION)::GetProcAddress(m_hModuleDll,HMJRE_JREGETVERSION);
	m_pfnDecodeEscSeq = (PFN_DECODEESCSEQ)::GetProcAddress(m_hModuleDll,HMJRE_DECODEESCSEQ);
	m_pfnGetJreMessage = (PFN_GETJREMESSAGE)::GetProcAddress(m_hModuleDll,HMJRE_GETJREMESSAGE);
	m_pfnJre2Open = (PFN_JRE2OPEN)::GetProcAddress(m_hModuleDll,HMJRE_JRE2OPEN);
	m_pfnJre2Compile = (PFN_JRE2COMPILE)::GetProcAddress(m_hModuleDll,HMJRE_JRE2COMPILE);
	m_pfnJre2GetMatchInfo
		= (PFN_JRE2GETMATCHINFO)::GetProcAddress(m_hModuleDll,HMJRE_JRE2GETMATCHINFO);
	m_pfnJre2Close = (PFN_JRE2CLOSE)::GetProcAddress(m_hModuleDll,HMJRE_JRE2CLOSE);
	if (m_pfnIsMatch == NULL || m_pfnGlobalReplace == NULL || m_pfnJreGetVersion == NULL ||
		m_pfnDecodeEscSeq == NULL || m_pfnGetJreMessage == NULL ||
		m_pfnJre2Open == NULL || m_pfnJre2Compile == NULL ||
		m_pfnJre2GetMatchInfo == NULL || m_pfnJre2Close == NULL)
		throw DllProcAddressNotFoundError();
}

HmJre_Manager::~HmJre_Manager()
{
	::FreeLibrary(m_hModuleDll);
}

DWORD
HmJre_Manager::match(const StringBuffer& ptn, const StringBuffer& str, int nFlags)
{
	JRE2* pJre2_Org = m_htblJre.getValue(ptn);
	JRE2* pJre2 = pJre2_Org;
	LPCSTR pstr = str;
	char msg[80];
	int ret = (*m_pfnBMatch)(pHmJre_Org != NULL ? NULL : (LPCSTR)ptn,
							 pstr, pstr + str.length(),
							 &pHmJre,
							 msg);
	m_strErrMsg = msg;

	m_strSplitted = str;
	if (pHmJre_Org == NULL && pHmJre != NULL) {
		m_HmJre_Pool.addItem(pHmJre);
		m_htblHmJre.setValue(ptn, pHmJre);
	}
	DWORD result;
	ResultList* pResultList = NULL;
	if (ret <= 0) {
		result = BREGEXP_RESULT_FAILED;
	} else {
		result = make_dword(pHmJre->startp[0] - pstr,
							pHmJre->endp[0] - pHmJre->startp[0]);
		if (pHmJre->nparens > 0) {
			pResultList = new ResultList(pHmJre->nparens);
			for (int i = 1; i <= pHmJre->nparens; i++)
				pResultList->m_pResults[i-1]
					= make_dword(pHmJre->startp[i] - pstr,
								 pHmJre->endp[i] - pHmJre->startp[i]);
		}
	}
	m_pResultList = pResultList;
	return result;
}

StringBuffer
HmJre_Manager::bSubst(const StringBuffer& ptn, const StringBuffer& str)
{
	BREGEXP* pHmJre_Org = m_htblHmJre.getValue(ptn);
	BREGEXP* pHmJre = pHmJre_Org;
	LPCSTR pstr = str;
	char msg[80];
	int ret = (*m_pfnBSubst)(pHmJre_Org != NULL ? NULL : (LPCSTR)ptn,
							 pstr, pstr + str.length(),
							 &pHmJre,
							 msg);
	m_strErrMsg = msg;

//	m_strSplitted = nullStr;
	if (pHmJre_Org == NULL && pHmJre != NULL) {
		m_HmJre_Pool.addItem(pHmJre);
		m_htblHmJre.setValue(ptn, pHmJre);
	}
//	m_pResultList = NULL;
	if (ret < 0) return nullStr;
	return pHmJre->outp;
}

StringBuffer
HmJre_Manager::bTrans(const StringBuffer& ptn, const StringBuffer& str)
{
	BREGEXP* pHmJre_Org = m_htblHmJre.getValue(ptn);
	BREGEXP* pHmJre = pHmJre_Org;
	LPCSTR pstr = str;
	char msg[80];
	int ret = (*m_pfnBTrans)(pHmJre_Org != NULL ? NULL : (LPCSTR)ptn,
							 pstr, pstr + str.length(),
							 &pHmJre,
							 msg);
	m_strErrMsg = msg;

//	m_strSplitted = nullStr;
	if (pHmJre_Org == NULL && pHmJre != NULL) {
		m_HmJre_Pool.addItem(pHmJre);
		m_htblHmJre.setValue(ptn, pHmJre);
	}
//	m_pResultList = NULL;
	if (ret < 0) return nullStr;
	return pHmJre->outp;
}

int
HmJre_Manager::bSplit(const StringBuffer& ptn, const StringBuffer& str, int limit)
{
	BREGEXP* pHmJre_Org = m_htblHmJre.getValue(ptn);
	BREGEXP* pHmJre = pHmJre_Org;
	LPCSTR pstr = str;
	char msg[80];
	int ret = (*m_pfnBSplit)(pHmJre_Org != NULL ? NULL : (LPCSTR)ptn,
							 pstr, pstr + str.length(), limit,
							 &pHmJre,
							 msg);
	m_strErrMsg = msg;

	m_strSplitted = str;
	if (pHmJre_Org == NULL && pHmJre != NULL) {
		m_HmJre_Pool.addItem(pHmJre);
		m_htblHmJre.setValue(ptn, pHmJre);
	}
	int result;
	ResultList* pResultList = NULL;
	if (ret <= 0) {
		result = BREGEXP_RESULT_FAILED;
	} else {
		result = ret;
		if (ret > 0) {
			pResultList = new ResultList(ret);
			for (int i = 0; i < ret; i++) {
				const char** ppStr = pHmJre->splitp + (i << 1);
				pResultList->m_pResults[i]
					= make_dword(*ppStr - pstr, *(ppStr + 1) - *ppStr);
			}
		}
	}
	m_pResultList = pResultList;
	return result;
}

const StringBuffer&
HmJre_Manager::bRegexpversion()
{
	if (m_strVersion.length() == 0) m_strVersion = (*m_pfnBRegexpversion)();
	return m_strVersion;
}

DWORD
HmJre_Manager::getNextResult()
{
	if (m_pResultList.ptr() == NULL ||
		m_pResultList->m_nHead >= m_pResultList->m_nSize)
		return (DWORD)-1;
	return m_pResultList->m_pResults[m_pResultList->m_nHead++];
}

StringBuffer
HmJre_Manager::posToString(DWORD pos) const
{
	if (pos == BREGEXP_RESULT_FAILED || m_strSplitted.length() == 0)
		return nullStr;
	return m_strSplitted.extract(LOWORD(pos), HIWORD(pos));
}

BOOL
HmJre_Manager::hasMoreResults()
{
	return  m_pResultList.ptr() != NULL &&
			m_pResultList->m_nHead < m_pResultList->m_nSize;
}

