//	DengakuServer v1: Copyright(C) 1999-2000, M. Sugiura
/*
 *	bregexp_mngr.cpp
 *	BRegExp_Manager クラスの実装
 */

#include "bregexp_mngr.h"

BRegExp_Manager::BRegExp_Manager(const StringBuffer& filename)
	: m_pResultList(NULL)
{
	m_hModuleDll = ::LoadLibrary(filename);
	if (m_hModuleDll == NULL) throw DllNotFoundError();
	m_pfnBMatch = (PFN_BMATCH)::GetProcAddress(m_hModuleDll,BREGEXP_BMATCH);
	m_pfnBSubst = (PFN_BSUBST)::GetProcAddress(m_hModuleDll,BREGEXP_BSUBST);
	m_pfnBTrans = (PFN_BTRANS)::GetProcAddress(m_hModuleDll,BREGEXP_BTRANS);
	m_pfnBSplit = (PFN_BSPLIT)::GetProcAddress(m_hModuleDll,BREGEXP_BSPLIT);
	m_pfnBRegfree = (PFN_BREGFREE)::GetProcAddress(m_hModuleDll,BREGEXP_BREGFREE);
	m_pfnBRegexpversion
		= (PFN_BREGEXPVERSION)::GetProcAddress(m_hModuleDll,BREGEXP_BREGEXPVERSION);
	if (m_pfnBMatch == NULL || m_pfnBSubst == NULL || m_pfnBTrans == NULL ||
		m_pfnBSplit == NULL || m_pfnBRegfree == NULL || m_pfnBRegexpversion == NULL)
		throw DllProcAddressNotFoundError();
}

BRegExp_Manager::~BRegExp_Manager()
{
	// DLL のアンロードの順序によりアクセス違反が起こる可能性あり
	if (!::IsBadReadPtr(m_BRegExp_Pool.getItemByIndex(0), sizeof(BREGEXP))) {
		while (m_BRegExp_Pool.itemNum() > 0) {
			(*m_pfnBRegfree)(m_BRegExp_Pool.getItemByIndex(0));
			m_BRegExp_Pool.delItemByIndex(0);
		}
		::FreeLibrary(m_hModuleDll);
	}
}

DWORD
BRegExp_Manager::bMatch(const StringBuffer& ptn, const StringBuffer& str)
{
	BREGEXP* pBRegExp_Org = m_htblBRegExp.getValue(ptn);
	BREGEXP* pBRegExp = pBRegExp_Org;
	LPCSTR pstr = str;
	char msg[80];
	int ret = (*m_pfnBMatch)(pBRegExp_Org != NULL ? NULL : (LPCSTR)ptn,
							 pstr, pstr + str.length(),
							 &pBRegExp,
							 msg);
	m_strErrMsg = msg;

	m_strSplitted = str;
	if (pBRegExp_Org == NULL && pBRegExp != NULL) {
		m_BRegExp_Pool.addItem(pBRegExp);
		m_htblBRegExp.setValue(ptn, pBRegExp);
	}
	DWORD result;
	ResultList* pResultList = NULL;
	if (ret <= 0) {
		result = BREGEXP_RESULT_FAILED;
	} else {
		result = make_dword(pBRegExp->startp[0] - pstr,
							pBRegExp->endp[0] - pBRegExp->startp[0]);
		if (pBRegExp->nparens > 0) {
			pResultList = new ResultList(pBRegExp->nparens);
			for (int i = 1; i <= pBRegExp->nparens; i++)
				pResultList->m_pResults[i-1]
					= make_dword(pBRegExp->startp[i] - pstr,
								 pBRegExp->endp[i] - pBRegExp->startp[i]);
		}
	}
	m_pResultList = pResultList;
	return result;
}

StringBuffer
BRegExp_Manager::bSubst(const StringBuffer& ptn, const StringBuffer& str)
{
	BREGEXP* pBRegExp_Org = m_htblBRegExp.getValue(ptn);
	BREGEXP* pBRegExp = pBRegExp_Org;
	LPCSTR pstr = str;
	char msg[80];
	int ret = (*m_pfnBSubst)(pBRegExp_Org != NULL ? NULL : (LPCSTR)ptn,
							 pstr, pstr + str.length(),
							 &pBRegExp,
							 msg);
	m_strErrMsg = msg;

//	m_strSplitted = nullStr;
	if (pBRegExp_Org == NULL && pBRegExp != NULL) {
		m_BRegExp_Pool.addItem(pBRegExp);
		m_htblBRegExp.setValue(ptn, pBRegExp);
	}
//	m_pResultList = NULL;
	if (ret < 0) return nullStr;
	return pBRegExp->outp;
}

StringBuffer
BRegExp_Manager::bTrans(const StringBuffer& ptn, const StringBuffer& str)
{
	BREGEXP* pBRegExp_Org = m_htblBRegExp.getValue(ptn);
	BREGEXP* pBRegExp = pBRegExp_Org;
	LPCSTR pstr = str;
	char msg[80];
	int ret = (*m_pfnBTrans)(pBRegExp_Org != NULL ? NULL : (LPCSTR)ptn,
							 pstr, pstr + str.length(),
							 &pBRegExp,
							 msg);
	m_strErrMsg = msg;

//	m_strSplitted = nullStr;
	if (pBRegExp_Org == NULL && pBRegExp != NULL) {
		m_BRegExp_Pool.addItem(pBRegExp);
		m_htblBRegExp.setValue(ptn, pBRegExp);
	}
//	m_pResultList = NULL;
	if (ret < 0) return nullStr;
	return pBRegExp->outp;
}

int
BRegExp_Manager::bSplit(const StringBuffer& ptn, const StringBuffer& str, int limit)
{
	BREGEXP* pBRegExp_Org = m_htblBRegExp.getValue(ptn);
	BREGEXP* pBRegExp = pBRegExp_Org;
	LPCSTR pstr = str;
	char msg[80];
	int ret = (*m_pfnBSplit)(pBRegExp_Org != NULL ? NULL : (LPCSTR)ptn,
							 pstr, pstr + str.length(), limit,
							 &pBRegExp,
							 msg);
	m_strErrMsg = msg;

	m_strSplitted = str;
	if (pBRegExp_Org == NULL && pBRegExp != NULL) {
		m_BRegExp_Pool.addItem(pBRegExp);
		m_htblBRegExp.setValue(ptn, pBRegExp);
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
				const char** ppStr = pBRegExp->splitp + (i << 1);
				pResultList->m_pResults[i]
					= make_dword(*ppStr - pstr, *(ppStr + 1) - *ppStr);
			}
		}
	}
	m_pResultList = pResultList;
	return result;
}

const StringBuffer&
BRegExp_Manager::bRegexpversion()
{
	if (m_strVersion.length() == 0) m_strVersion = (*m_pfnBRegexpversion)();
	return m_strVersion;
}

DWORD
BRegExp_Manager::getNextResult()
{
	if (m_pResultList.ptr() == NULL ||
		m_pResultList->m_nHead >= m_pResultList->m_nSize)
		return (DWORD)-1;
	return m_pResultList->m_pResults[m_pResultList->m_nHead++];
}

StringBuffer
BRegExp_Manager::posToString(DWORD pos) const
{
	if (pos == (DWORD)-1 || m_strSplitted.length() == 0) return nullStr;
	return m_strSplitted.extract(LOWORD(pos), HIWORD(pos));
}

BOOL
BRegExp_Manager::hasMoreResults()
{
	return  m_pResultList.ptr() != NULL &&
			m_pResultList->m_nHead < m_pResultList->m_nSize;
}

