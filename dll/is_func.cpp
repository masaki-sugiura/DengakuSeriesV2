//	$Id: is_func.cpp,v 1.3 2002-02-19 15:34:22 sugiura Exp $
/*
 *	is_func.cpp
 *	画像の管理に関するクラス
 */

#include "DengakuDLL.h"

//	Susie Plug-in をロードする
DENGAKUDLL_API HIDEDLL_NUMTYPE
LOADSPI(LPCSTR spi_file)
{
	try {
		return g_pSessionInstance->si_loadspi(spi_file);
	} catch (...) {
		return 0;
	}
}

//	Susie Plug-in をアンロードする
DENGAKUDLL_API HIDEDLL_NUMTYPE
FREESPI()
{
	try {
		return g_pSessionInstance->si_freespi();
	} catch (...) {
		return 0;
	}
}

//	ロードした Plug-in の情報を返す
DENGAKUDLL_API LPCSTR
GETSPIINFO(HIDEDLL_NUMTYPE type)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getspiinfo(type);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	指定された画像ファイルが Plug-in でサポートされているかどうかを返す
DENGAKUDLL_API HIDEDLL_NUMTYPE
ISSUPPORTEDPIC(LPCSTR filename)
{
	try {
		return g_pSessionInstance->si_issupportedpic(filename);
	} catch (...) {
		return 0;
	}
}

//	指定された画像ファイルの情報を返す
DENGAKUDLL_API LPCSTR
GETPICINFO(LPCSTR filename)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getpicinfo(filename);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

