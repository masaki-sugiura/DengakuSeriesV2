//	$Id: misc.h,v 1.6 2002-08-06 13:33:04 sugiura Exp $
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

#endif
