//	$Id: bs_func.cpp,v 1.1 2001-11-22 13:42:41 sugiura Exp $
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
	*g_pStrBuffer = g_pSessionInstance->si_bregexp_errormsg();
	return *g_pStrBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_VERSION()
{
	*g_pStrBuffer = g_pSessionInstance->si_bregexp_version();
	return *g_pStrBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_MATCH(LPCSTR pattern, LPCSTR str)
{
	*g_pStrBuffer = g_pSessionInstance->si_bregexp_match(pattern, str);
	return *g_pStrBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_SUBST(LPCSTR pattern, LPCSTR str)
{
	*g_pStrBuffer = g_pSessionInstance->si_bregexp_subst(pattern, str);
	return *g_pStrBuffer;
}

DENGAKUDLL_API LPCSTR
BRE_TRANS(LPCSTR pattern, LPCSTR str)
{
	*g_pStrBuffer = g_pSessionInstance->si_bregexp_trans(pattern, str);
	return *g_pStrBuffer;
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_SPLIT(LPCSTR pattern, LPCSTR str, int limit)
{
	return g_pSessionInstance->si_bregexp_split(pattern, str, limit);
}

DENGAKUDLL_API LPCSTR
BRE_GETNEXT()
{
	*g_pStrBuffer = g_pSessionInstance->si_bregexp_getnext();
	return *g_pStrBuffer;
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
BRE_HASMORERESULTS()
{
	return g_pSessionInstance->si_bregexp_hasmoreresults();
}

