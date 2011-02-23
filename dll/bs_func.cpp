//	$Id: bs_func.cpp,v 1.3 2002/02/19 15:34:22 sugiura Exp $
/*
 *	bs_func.cpp
 *	BRegexp ‚ÉŠÖ‚·‚éƒNƒ‰ƒX
 */

#include "DengakuDLL.h"

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_LOAD(LPCSTR bre_file)
{
	try {
		return g_pSessionInstance->si_bregexp_load(bre_file);
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_FREE()
{
	try {
		return g_pSessionInstance->si_bregexp_free();
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API LPCSTR
BRE_ERRORMSG()
{
	try {
		g_strBuffer = g_pSessionInstance->si_bregexp_errormsg();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
BRE_VERSION()
{
	try {
		g_strBuffer = g_pSessionInstance->si_bregexp_version();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
BRE_MATCH(LPCSTR pattern, LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_bregexp_match(pattern, str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
BRE_SUBST(LPCSTR pattern, LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_bregexp_subst(pattern, str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
BRE_TRANS(LPCSTR pattern, LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_bregexp_trans(pattern, str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_SPLIT(LPCSTR pattern, LPCSTR str, int limit)
{
	try {
		return g_pSessionInstance->si_bregexp_split(pattern, str, limit);
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API LPCSTR
BRE_GETNEXT()
{
	try {
		g_strBuffer = g_pSessionInstance->si_bregexp_getnext();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
BRE_POSTOSTR(LPCSTR pos)
{
	try {
		g_strBuffer = g_pSessionInstance->si_bregexp_postostr(pos);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_HASMORERESULTS()
{
	try {
		return g_pSessionInstance->si_bregexp_hasmoreresults();
	} catch (...) {
		return 0;
	}
}

