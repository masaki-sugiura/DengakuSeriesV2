//	$Id: misc.h,v 1.5 2002-02-25 07:26:01 sugiura Exp $
/*
 *	misc.h
 *	�G���ȃ��[�e�B���e�B�֐�
 */

#ifndef DENGAKUSERIES_FUNC_MISC
#define DENGAKUSERIES_FUNC_MISC

#include <windows.h>

BOOL isWinNT();

DWORD GetDialogBaseUnits(HWND hDlg, LPCSTR str = "M");
void GetWindowCenter(HWND hWnd, HWND hWndOwner, RECT& rect);

void InternalError(LPCSTR file, int line);

#endif
