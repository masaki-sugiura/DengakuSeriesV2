//	$Id: si_bregexp.cpp,v 1.6 2005-01-15 06:53:48 sugiura Exp $
/*
 *	si_bregexp.cpp
 *	BRegexp に関するクラス
 */

#include "session.h"
#include "cmdline.h"
#include "pathname.h"
#include "bregexp_mngr.h"

int
SessionInstance::si_bregexp_load(const StringBuffer& filename)
{
	if (m_pBRegExp_Manager.ptr() != NULL) return TRUE;

	PathName path(BREGEXP_FILENAME);
	// 空文字指定または"BREGEXP.DLL"が指定された場合は
	// Windows のデフォルトのサーチに任せる
	if (filename.length() > 0 &&
		filename.compareTo(BREGEXP_FILENAME, FALSE) != 0) {
		// パス指定あり
		if (!m_DirList.getPathName(filename,path,TRUE)) {
			return FALSE;
		}
	}

	try {
		m_pBRegExp_Manager = new BRegExp_Manager(path);
	} catch (DllLoadError&) {
		m_pBRegExp_Manager = NULL;
		return FALSE;
	}

	return TRUE;
}

int
SessionInstance::si_bregexp_free()
{
	if (m_pBRegExp_Manager.ptr() == NULL) return FALSE;
	m_pBRegExp_Manager = NULL;
	return TRUE;
}

StringBuffer
SessionInstance::si_bregexp_errormsg()
{
	if (m_pBRegExp_Manager.ptr() == NULL) return nullStr;
	return m_pBRegExp_Manager->getErrorMessage();
}

StringBuffer
SessionInstance::si_bregexp_version()
{
	if (m_pBRegExp_Manager.ptr() == NULL) return nullStr;
	return m_pBRegExp_Manager->bRegexpversion();
}

StringBuffer
SessionInstance::si_bregexp_match(const StringBuffer& pattern, const StringBuffer& str)
{
	if (m_pBRegExp_Manager.ptr() == NULL) return nullStr;
	return make_result(m_pBRegExp_Manager->bMatch(pattern,str));
}

StringBuffer
SessionInstance::si_bregexp_subst(const StringBuffer& pattern, const StringBuffer& str)
{
	if (m_pBRegExp_Manager.ptr() == NULL) return nullStr;
	return m_pBRegExp_Manager->bSubst(pattern,str);
}

StringBuffer
SessionInstance::si_bregexp_trans(const StringBuffer& pattern, const StringBuffer& str)
{
	if (m_pBRegExp_Manager.ptr() == NULL) return nullStr;
	return m_pBRegExp_Manager->bTrans(pattern,str);
}

int
SessionInstance::si_bregexp_split(
	const StringBuffer& pattern,
	const StringBuffer& str,
	int limit)
{
	if (m_pBRegExp_Manager.ptr() == NULL) return (int)BREGEXP_RESULT_FAILED;
	return m_pBRegExp_Manager->bSplit(pattern,str,limit);
}

StringBuffer
SessionInstance::si_bregexp_getnext()
{
	if (m_pBRegExp_Manager.ptr() == NULL) return nullStr;
	return make_result(m_pBRegExp_Manager->getNextResult());
}

StringBuffer
SessionInstance::si_bregexp_postostr(const StringBuffer& pos)
{
	if (m_pBRegExp_Manager.ptr() == NULL) return nullStr;
	return m_pBRegExp_Manager->posToString(make_dword_from_pos(pos));
}

int
SessionInstance::si_bregexp_hasmoreresults()
{
	if (m_pBRegExp_Manager.ptr() == NULL) return FALSE;
	return (int)m_pBRegExp_Manager->hasMoreResults();
}

