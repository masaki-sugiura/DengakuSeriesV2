// $Id: cds_func.cpp,v 1.2 2002-01-16 16:31:04 sugiura Exp $
/*
 *	cds_func.cpp
 *	コモンダイアログサービスの関数
 */

#include "DengakuDLL.h"
#include "seldir.h"
#include "cmdline.h"

//	ファイルの選択
DENGAKUDLL_API LPCSTR
GETFILENAME(
	HIDEDLL_NUMTYPE hWnd,
	LPCSTR pszTitle,
	LPCSTR pszIniDir,
	LPCSTR pszFilter)
{
	RealCmdLineParser argv(pszFilter);
	g_strBuffer = g_pSessionInstance->getFileNameByDlg((HWND)hWnd,
														pszTitle,
														pszIniDir,
														argv);
	return g_strBuffer;
}

//	ディレクトリの選択
DENGAKUDLL_API LPCSTR
GETDIRNAME(HIDEDLL_NUMTYPE hWnd, LPCSTR pszTitle, LPCSTR pszIniDir)
{
	g_strBuffer = g_pSessionInstance->getDirNameByDlg((HWND)hWnd,
														pszTitle,
														pszIniDir,
														DBPA_DISABLEMODIFY);
	return g_strBuffer;
}

//	ディレクトリの選択(フォルダ操作可能)
DENGAKUDLL_API LPCSTR
GETDIRNAMEEX(
	HIDEDLL_NUMTYPE hWnd,
	LPCSTR pszTitle,
	LPCSTR pszIniDir,
	HIDEDLL_NUMTYPE flag)
{
	g_strBuffer
		= g_pSessionInstance->getDirNameByDlg((HWND)hWnd,
											  pszTitle,
											  pszIniDir,
											  (flag!=0) ? 0 : DBPA_DISABLEMODIFY);
	return g_strBuffer;
}

//	色の選択
DENGAKUDLL_API LPCSTR
GETCOLOR(HIDEDLL_NUMTYPE hWnd, LPCSTR pszTitle, LPCSTR pszIniColor)
{
	g_strBuffer = g_pSessionInstance->getColorByDlg((HWND)hWnd,
													pszTitle,
													pszIniColor);
	return g_strBuffer;
}

//	フォントの選択
DENGAKUDLL_API LPCSTR
GETFONT(HIDEDLL_NUMTYPE hWnd, LPCSTR pszTitle, LPCSTR pszIniFont)
{
	RealCmdLineParser argv(pszIniFont);
	g_strBuffer = g_pSessionInstance->getFontByDlg((HWND)hWnd,
													pszTitle,
													argv);
	return g_strBuffer;
}

