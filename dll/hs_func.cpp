//	$Id: hs_func.cpp,v 1.1 2008-08-15 05:47:40 sugiura Exp $
/*
 *	hs_func.cpp
 *	HmJre ‚ÉŠÖ‚·‚éAPI
 */

#include "DengakuDLL.h"

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_LOAD(LPCSTR hmjre_file)
{
	try {
		return g_pSessionInstance->si_hmjre_load(hmjre_file);
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_FREE()
{
	try {
		return g_pSessionInstance->si_hmjre_free();
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API LPCSTR
HMJRE_GET_VERSION()
{
	try {
		g_strBuffer = g_pSessionInstance->si_hmjre_get_version();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
HMJRE_MATCH(LPCSTR pszFind, LPCSTR pszTarget, int nOffset, int nFlags, int nRegExp)
{
	try {
		g_strBuffer = g_pSessionInstance->si_hmjre_match(pszFind, pszTarget, nOffset, nFlags, nRegExp);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
HMJRE_GET_TAG_POSITION(int nTagNumber)
{
	try {
		g_strBuffer = g_pSessionInstance->si_hmjre_get_tag_position(nTagNumber);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
HMJRE_GET_MATCH_STRING(LPCSTR pszTarget, LPCSTR pszRegion)
{
	try {
		g_strBuffer = g_pSessionInstance->si_hmjre_get_match_string(pszTarget, pszRegion);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_FIND_REGULAR(LPCSTR pszRE, LPCSTR pszTarget, int nOffset)
{
	try {
		return g_pSessionInstance->si_hmjre_find_regular(pszRE, pszTarget, nOffset);
	} catch (...) {
		return -1;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_FIND_REGULAR_NO_CASE_SENSE(LPCSTR pszRE, LPCSTR pszTarget, int nOffset)
{
	try {
		return g_pSessionInstance->si_hmjre_find_regular_no_case_sense(pszRE, pszTarget, nOffset);
	} catch (...) {
		return -1;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_GET_LAST_MATCH_LENGTH()
{
	try {
		return g_pSessionInstance->si_hmjre_get_last_match_length();
	} catch (...) {
		return -1;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_GET_LAST_MATCH_TAG_POSITION()
{
	try {
		return g_pSessionInstance->si_hmjre_get_last_match_tag_position();
	} catch (...) {
		return -1;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_GET_LAST_MATCH_TAG_LENGTH()
{
	try {
		return g_pSessionInstance->si_hmjre_get_last_match_tag_length();
	} catch (...) {
		return -1;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_ENV_CHANGED()
{
	try {
		return g_pSessionInstance->si_hmjre_env_changed();
	} catch (...) {
		return -1;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
HMJRE_OPTION_DIALOG(int nHwnd, int nDisableFlags)
{
	try {
		return g_pSessionInstance->si_hmjre_option_dialog(nHwnd, nDisableFlags);
	} catch (...) {
		return -1;
	}
}
