//	$Id: crs_func.cpp,v 1.2 2002-02-19 15:34:22 sugiura Exp $
/*
 *	cs_func.cpp
 *	共通サービスの関数
 */

#include "DengakuDLL.h"
#include "cmdline.h"

DENGAKUDLL_API HIDEDLL_NUMTYPE
CLR_SETNAME(LPCSTR cname, LPCSTR cref)
{
	try {
		return g_pSessionInstance->si_setcolorref(cname, cref);
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API LPCSTR
CLR_GETNAME(LPCSTR cref)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getcolorname(cref);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
CLR_GETRGB(LPCSTR cname)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getcolorref(cname);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
CLR_LOAD(LPCSTR fname)
{
	try {
		return g_pSessionInstance->si_loadcolorrefs(fname);
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
CLR_SAVE(LPCSTR fname)
{
	try {
		return g_pSessionInstance->si_savecolorrefs(fname);
	} catch (...) {
		return 0;
	}
}

