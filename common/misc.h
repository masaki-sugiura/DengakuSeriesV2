//	$Id: misc.h,v 1.4 2002-02-20 16:48:40 sugiura Exp $
/*
 *	misc.h
 *	�G���ȃ��[�e�B���e�B�֐�
 */

#ifndef DENGAKUSERIES_FUNC_MISC
#define DENGAKUSERIES_FUNC_MISC

#include <windows.h>

BOOL isWinNT();

DWORD GetDialogBaseUnits(HWND hDlg, LPCSTR str = "M");
void CenteringWindow(HWND hWnd, HWND hWndPos, DWORD swparam);

void InternalError(LPCSTR file, int line);

#endif
