//	$Id: si_hmjre.cpp,v 1.1 2008-06-29 15:19:43 sugiura Exp $
/*
 *	si_hmjre.cpp
 *	HmJre に関するクラス
 */

#include "session.h"
#include "cmdline.h"
#include "pathname.h"
#include "hmjre_mngr.h"

int
SessionInstance::si_hmjre_load(const StringBuffer& filename)
{
	if (m_pHmJre_Manager.ptr() != NULL) return TRUE;

	PathName path(HMJRE_FILENAME);
	// 空文字指定または"HMJRE.DLL"が指定された場合は
	// Windows のデフォルトのサーチに任せる
	if (filename.length() > 0 &&
		filename.compareTo(HMJRE_FILENAME, FALSE) != 0)
	{
		// パス指定あり
		if (!m_DirList.getPathName(filename, path, TRUE))
		{
			return FALSE;
		}
	}

	try
	{
		m_pHmJre_Manager = new HmJre_Manager(path);
	}
	catch (DllLoadError&)
	{
		m_pHmJre_Manager = NULL;
		return FALSE;
	}

	return TRUE;
}

int
SessionInstance::si_hmjre_free()
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return FALSE;
	}

	m_pHmJre_Manager = NULL;

	return TRUE;
}

StringBuffer
SessionInstance::si_hmjre_get_version()
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return nullStr;
	}

	return m_pHmJre_Manager->jreGetVersion();
}

StringBuffer
SessionInstance::si_hmjre_match(const StringBuffer& strFind, const StringBuffer& strTarget, int nOffset, int nFlags, int nRegExp)
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return nullStr;
	}

	return m_pHmJre_Manager->match(strFind, strTarget, nOffset, nFlags, nRegExp);
}

StringBuffer
SessionInstance::si_hmjre_get_tag_position(int nTagNumber)
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return nullStr;
	}

	return m_pHmJre_Manager->getTagPosition(nTagNumber);
}

StringBuffer
SessionInstance::si_hmjre_get_match_string(const StringBuffer& strTarget, const StringBuffer& strRegion)
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return nullStr;
	}

	return m_pHmJre_Manager->getMatchString(strTarget, strRegion);
}

int
SessionInstance::si_hmjre_find_regular(const StringBuffer& strRE, const StringBuffer& strTarget, int nOffset)
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return -1;
	}

	return m_pHmJre_Manager->findRegular(strRE, strTarget, nOffset);
}

int
SessionInstance::si_hmjre_find_regular_no_case_sense(const StringBuffer& strRE, const StringBuffer& strTarget, int nOffset)
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return -1;
	}

	return m_pHmJre_Manager->findRegularNoCaseSense(strRE, strTarget, nOffset);
}

int
SessionInstance::si_hmjre_get_last_match_length()
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return -1;
	}

	return m_pHmJre_Manager->getLastMatchLength();
}

int
SessionInstance::si_hmjre_option_dialog(int nHwnd, int nDisableFlags)
{
	if (m_pHmJre_Manager.ptr() == NULL)
	{
		return -1;
	}

	return m_pHmJre_Manager->optionDialog(nHwnd, nDisableFlags);
}
