//	$Id: misc.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	misc.cpp
 *	雑多なユーティリティ関数
 */

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

inline COLORREF
swap_bytes(COLORREF color)
{
	return ((0x000000ff & color) << 16) |
			(0x0000ff00 & color)        |
		   ((0x00ff0000 & color) >> 16);
}

COLORREF
ColorStrToColorRef(LPCSTR pcolor)
{
	COLORREF color = 0;
	if (pcolor != NULL && *pcolor == '#' && lstrlen(++pcolor) >= 6) {
		for (int i = 0; i < 6; i++) {
			color <<= 4;
			if (IsCharDigit(*pcolor))
				color += *pcolor - '0';
			else if (IsCharXDigit(*pcolor))
				color += *pcolor - (IsCharUpper(*pcolor) ? 'A' : 'a') + 10;
			pcolor++;
		}
		color = swap_bytes(color);
	}
	return color;
}

StringBuffer
ColorRefToColorStr(COLORREF color)
{
	TCHAR buf[8];
	wsprintf(buf,"#%-06.6x",0x00FFFFFF&swap_bytes(color));
	return buf;
}

