//	$Id: crs_func.cpp,v 1.1 2002-02-15 17:46:08 sugiura Exp $
/*
 *	cs_func.cpp
 *	共通サービスの関数
 */

#include "DengakuDLL.h"
#include "cmdline.h"

DENGAKUDLL_API HIDEDLL_NUMTYPE
CLR_SETNAME(LPCSTR cname, LPCSTR cref)
{
	return g_pSessionInstance->si_setcolorref(cname, cref);
}

DENGAKUDLL_API LPCSTR
CLR_GETNAME(LPCSTR cref)
{
	g_strBuffer = g_pSessionInstance->si_getcolorname(cref);
	return g_strBuffer;
}

DENGAKUDLL_API LPCSTR
CLR_GETRGB(LPCSTR cname)
{
	g_strBuffer = g_pSessionInstance->si_getcolorref(cname);
	return g_strBuffer;
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
CLR_LOAD(LPCSTR fname)
{
	return g_pSessionInstance->si_loadcolorrefs(fname);
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
CLR_SAVE(LPCSTR fname)
{
	return g_pSessionInstance->si_savecolorrefs(fname);
}

