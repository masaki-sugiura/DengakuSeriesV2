//	$Id: misc.cpp,v 1.4 2002-02-20 16:48:40 sugiura Exp $
/*
 *	misc.cpp
 *	雑多なユーティリティ関数
 */

#include "strutils.h"
#include "misc.h"

BOOL
isWinNT()
{
	static OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!::GetVersionEx(&osi)) return FALSE;
	return osi.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

DWORD
GetDialogBaseUnits(HWND hDlg, LPCSTR str)
{
	if (!::IsWindow(hDlg)) return 0L;
	HDC hDc = ::GetDC(hDlg);
	SIZE sz;
	::GetTextExtentPoint32(hDc,str,1,&sz);
	::ReleaseDC(hDlg,hDc);
	return (DWORD)MAKELONG((WORD)sz.cx,(WORD)sz.cy);
}

void
CenteringWindow(HWND hWnd, HWND hwndOwner, DWORD swparam)
{
	//	ウィンドウを画面中央に移動
	RECT rectOwner, rect;
	::GetWindowRect(hwndOwner, &rectOwner);
	::GetWindowRect(hWnd, &rect);
	POINT pos;
	pos.x = (rectOwner.left + rectOwner.right + rect.left - rect.right) >> 1;
	pos.y = (rectOwner.top + rectOwner.bottom + rect.top - rect.bottom) >> 1;
//	::ScreenToClient(hwndOwner, &pos);
	::SetWindowPos(hWnd, hwndOwner,
				   pos.x, pos.y, 0, 0,
				   swparam);
}

void
InternalError(LPCSTR file, int line)
{
	TCHAR msg[2 * MAX_PATH];
	lstrcpy(msg, "DengakuLib: 内部エラー\nFILE: ");
	lstrcat(msg, file);
	lstrcat(msg, "\nLINE: ");
	int len = lstrlen(msg);
	wsprintf(msg + len, "%d", line);

	::MessageBox(NULL, msg, "DengakuLib Message", MB_OK);
}

