// $Id: cds_func.cpp,v 1.4 2002/02/19 15:34:22 sugiura Exp $
/*
 *	cds_func.cpp
 *	�R�����_�C�A���O�T�[�r�X�̊֐�
 */

#include "DengakuDLL.h"
#include "seldir.h"
#include "cmdline.h"

//	�t�@�C���̑I��
DENGAKUDLL_API LPCSTR
GETFILENAME(
	HIDEDLL_NUMTYPE hWnd,
	LPCSTR pszTitle,
	LPCSTR pszIniDir,
	LPCSTR pszFilter)
{
	try {
		RealCmdLineParser argv(pszFilter);
		g_strBuffer = g_pSessionInstance->getFileNameByDlg((HWND)hWnd,
															pszTitle,
															pszIniDir,
															argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�f�B���N�g���̑I��
DENGAKUDLL_API LPCSTR
GETDIRNAME(HIDEDLL_NUMTYPE hWnd, LPCSTR pszTitle, LPCSTR pszIniDir)
{
	try {
		g_strBuffer = g_pSessionInstance->getDirNameByDlg((HWND)hWnd,
															pszTitle,
															pszIniDir,
															DBPA_DISABLEMODIFY);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�f�B���N�g���̑I��(�t�H���_����\)
DENGAKUDLL_API LPCSTR
GETDIRNAMEEX(
	HIDEDLL_NUMTYPE hWnd,
	LPCSTR pszTitle,
	LPCSTR pszIniDir,
	HIDEDLL_NUMTYPE flag)
{
	try {
		g_strBuffer
			= g_pSessionInstance->getDirNameByDlg((HWND)hWnd,
												  pszTitle,
												  pszIniDir,
												  (flag!=0) ? 0 : DBPA_DISABLEMODIFY);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�F�̑I��
DENGAKUDLL_API LPCSTR
GETCOLOR(HIDEDLL_NUMTYPE hWnd, LPCSTR pszTitle, LPCSTR pszIniColor)
{
	try {
		RealCmdLineParser argv(pszIniColor);
		g_strBuffer = g_pSessionInstance->getColorByDlg((HWND)hWnd,
														pszTitle,
														argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�t�H���g�̑I��
DENGAKUDLL_API LPCSTR
GETFONT(HIDEDLL_NUMTYPE hWnd, LPCSTR pszTitle, LPCSTR pszIniFont)
{
	try {
		RealCmdLineParser argv(pszIniFont);
		g_strBuffer = g_pSessionInstance->getFontByDlg((HWND)hWnd,
														pszTitle,
														argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

