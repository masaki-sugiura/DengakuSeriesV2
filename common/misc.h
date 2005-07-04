//	$Id: misc.h,v 1.8 2005-06-19 17:53:49 sugiura Exp $
/*
 *	misc.h
 *	雑多なユーティリティ関数
 */

#ifndef DENGAKUSERIES_FUNC_MISC
#define DENGAKUSERIES_FUNC_MISC

#include <windows.h>

class StringBuffer;

BOOL isWinNT();

DWORD GetDialogBaseUnits(HWND hDlg, LPCSTR str = "M");
void GetWindowCenter(HWND hWnd, HWND hWndOwner, RECT& rect);
BOOL MyGetWindowText(HWND hWnd, StringBuffer& strText);
BOOL MySetWindowText(HWND hWnd, const StringBuffer& strText);

BOOL TimeStampToFileTime(LPCSTR pszTimeStamp, FILETIME* pft);

void InternalError(LPCSTR file, int line);

BOOL MySetWindowText(HWND hWnd, LPCSTR pszText);

#ifdef SetWindowText
#undef SetWindowText
#endif
#define SetWindowText(hWnd,lpString)	MySetWindowText(hWnd, lpString)

#endif
