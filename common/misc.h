//	$Id: misc.h,v 1.11 2007-02-10 19:00:01 sugiura Exp $
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
HWND SetFocusForced(HWND hwndFocus);

BOOL TimeStampToFileTime(LPCSTR pszTimeStamp, FILETIME* pft);

void InternalError(LPCSTR file, int line);

BOOL MySetWindowText(HWND hWnd, LPCSTR pszText);

#if 0
#ifdef SetWindowText
#undef SetWindowText
#endif
#define SetWindowText(hWnd,lpString)	MySetWindowText(hWnd, lpString)
#endif

void DebugOutput(LPCSTR pszFormat, ...);
#ifdef	_DEBUG
#define	DEBUG_OUTPUT(x)	DebugOutput x
#else
#define	DEBUG_OUTPUT(x)	/* nothing to do */
#endif

#endif
