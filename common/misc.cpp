//	$Id: misc.cpp,v 1.3 2002-02-15 17:46:08 sugiura Exp $
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

