//	$Id: dts_func.cpp,v 1.1 2002-08-05 16:06:17 sugiura Exp $
/*
 *	dts_func.cpp
 *	日付・時刻サービスの関数
 */

#include "DengakuDLL.h"
#include "cmdline.h"

DENGAKUDLL_API LPCSTR
DT_FORMAT(LPCSTR fmt, HIDEDLL_NUMTYPE time)
{
	try {
		g_strBuffer = g_pSessionInstance->si_datetime_format(fmt, time);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
DT_GETTIME()
{
	return g_pSessionInstance->si_datetime_gettime();
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
DT_FILETIME_TO_TIME(HIDEDLL_NUMTYPE ftime)
{
	return g_pSessionInstance->si_datetime_filetime(ftime);
}

