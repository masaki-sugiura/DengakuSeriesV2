//	$Id: misc.cpp,v 1.5 2002-02-25 07:26:01 sugiura Exp $
/*
 *	misc.cpp
 *	�G���ȃ��[�e�B���e�B�֐�
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
GetWindowCenter(HWND hWnd, HWND hwndOwner, RECT& rectRet)
{
	//	�E�B���h�E����ʒ����Ɉړ�
	RECT rectOwner, rect;
	::GetWindowRect(hwndOwner, &rectOwner);
	::GetWindowRect(hWnd, &rect);
	rectRet.left = (rectOwner.left + rectOwner.right + rect.left - rect.right) >> 1;
	rectRet.top  = (rectOwner.top + rectOwner.bottom + rect.top - rect.bottom) >> 1;
	rectRet.right  = rectRet.left + rect.right - rect.left;
	rectRet.bottom = rectRet.top + rect.bottom - rect.top;
}

void
InternalError(LPCSTR file, int line)
{
	TCHAR msg[2 * MAX_PATH];
	lstrcpy(msg, "DengakuLib: �����G���[\nFILE: ");
	lstrcat(msg, file);
	lstrcat(msg, "\nLINE: ");
	int len = lstrlen(msg);
	wsprintf(msg + len, "%d", line);

	::MessageBox(NULL, msg, "DengakuLib Message", MB_OK);
}

