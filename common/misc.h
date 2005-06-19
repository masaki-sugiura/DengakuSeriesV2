//	$Id: misc.h,v 1.7 2005-06-19 15:33:53 sugiura Exp $
/*
 *	misc.h
 *	雑多なユーティリティ関数
 */

#ifndef DENGAKUSERIES_FUNC_MISC
#define DENGAKUSERIES_FUNC_MISC

#include <windows.h>

BOOL isWinNT();

DWORD GetDialogBaseUnits(HWND hDlg, LPCSTR str = "M");
void GetWindowCenter(HWND hWnd, HWND hWndOwner, RECT& rect);

BOOL TimeStampToFileTime(LPCSTR pszTimeStamp, FILETIME* pft);

void InternalError(LPCSTR file, int line);

BOOL MySetWindowText(HWND hWnd, LPCSTR pszText);

#ifdef SetWindowText
#undef SetWindowText
#endif
#define SetWindowText(hWnd,lpString)	MySetWindowText(hWnd, lpString)

#endif
