//	$Id: is_func.cpp,v 1.2 2002-01-16 16:31:04 sugiura Exp $
/*
 *	is_func.cpp
 *	画像の管理に関するクラス
 */

#include "DengakuDLL.h"

//	Susie Plug-in をロードする
DENGAKUDLL_API HIDEDLL_NUMTYPE
LOADSPI(LPCSTR spi_file)
{
	return g_pSessionInstance->si_loadspi(spi_file);
}

//	Susie Plug-in をアンロードする
DENGAKUDLL_API HIDEDLL_NUMTYPE
FREESPI()
{
	return g_pSessionInstance->si_freespi();
}

//	ロードした Plug-in の情報を返す
DENGAKUDLL_API LPCSTR
GETSPIINFO(HIDEDLL_NUMTYPE type)
{
	g_strBuffer = g_pSessionInstance->si_getspiinfo(type);
	return g_strBuffer;
}

//	指定された画像ファイルが Plug-in でサポートされているかどうかを返す
DENGAKUDLL_API HIDEDLL_NUMTYPE
ISSUPPORTEDPIC(LPCSTR filename)
{
	return g_pSessionInstance->si_issupportedpic(filename);
}

//	指定された画像ファイルの情報を返す
DENGAKUDLL_API LPCSTR
GETPICINFO(LPCSTR filename)
{
	g_strBuffer = g_pSessionInstance->si_getpicinfo(filename);
	return g_strBuffer;
}

