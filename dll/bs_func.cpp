//	$Id: bs_func.cpp,v 1.2 2002-01-16 16:31:04 sugiura Exp $
/*
 *	bs_func.cpp
 *	BRegexp ‚ÉŠÖ‚·‚éƒNƒ‰ƒX
 */

#include "DengakuDLL.h"

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_LOAD(LPCSTR bre_file)
{
	return g_pSessionInstance->si_bregexp_load(bre_file);
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_FREE()
{
	return g_pSessionInstance->si_bregexp_free();
}

DENGAKUDLL_API LPCSTR
BRE_ERRORMSG()
{
	g_strBuffer = g_pSessionInstance->si_bregexp_errormsg();
	return g_strBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_VERSION()
{
	g_strBuffer = g_pSessionInstance->si_bregexp_version();
	return g_strBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_MATCH(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_bregexp_match(pattern, str);
	return g_strBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_SUBST(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_bregexp_subst(pattern, str);
	return g_strBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_TRANS(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_bregexp_trans(pattern, str);
	return g_strBuffer;
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_SPLIT(LPCSTR pattern, LPCSTR str, int limit)
{
	return g_pSessionInstance->si_bregexp_split(pattern, str, limit);
}

DENGAKUDLL_API LPCSTR
BRE_GETNEXT()
{
	g_strBuffer = g_pSessionInstance->si_bregexp_getnext();
	return g_strBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_POSTOSTR(LPCSTR pos)
{
	g_strBuffer = g_pSessionInstance->si_bregexp_postostr(pos);
	return g_strBuffer;
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_HASMORERESULTS()
{
	return g_pSessionInstance->si_bregexp_hasmoreresults();
}

