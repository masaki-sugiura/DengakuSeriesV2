//	$Id: misc.cpp,v 1.9 2004-04-26 15:32:06 sugiura Exp $
/*
 *	misc.cpp
 *	雑多なユーティリティ関数
 */

#include "strutils.h"
#include "misc.h"

BOOL
isWinNT()
{
	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!::GetVersionEx(&osi)) return FALSE;
	return osi.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

DWORD
GetDialogBaseUnits(HWND hDlg, LPCSTR str)
{
	if (!::IsWindow(hDlg)) return 0L;
	HDC hDc = ::GetDC(hDlg);
//	SIZE sz;
//	::GetTextExtentPoint32(hDc,str,1,&sz);
//	sz.cx -= ::GetTextCharacterExtra(hDc);
	TEXTMETRIC tm;
	::GetTextMetrics(hDc, &tm);
	::ReleaseDC(hDlg,hDc);
	return (DWORD)MAKELONG((WORD)tm.tmAveCharWidth,(WORD)tm.tmHeight);
}

void
GetWindowCenter(HWND hWnd, HWND hwndOwner, RECT& rectRet)
{
	//	ウィンドウを画面中央に移動
	RECT rectOwner, rect;
	::GetWindowRect(hwndOwner, &rectOwner);
	::GetWindowRect(hWnd, &rect);
	rectRet.left = (rectOwner.left + rectOwner.right + rect.left - rect.right) >> 1;
	rectRet.top  = (rectOwner.top + rectOwner.bottom + rect.top - rect.bottom) >> 1;
	rectRet.right  = rectRet.left + rect.right - rect.left;
	rectRet.bottom = rectRet.top + rect.bottom - rect.top;
}

BOOL
TimeStampToFileTime(LPCSTR pszTimeStamp, FILETIME* pft)
{
	TCHAR buf[20];
	if (*(pszTimeStamp + 2) == '/') {
		//	"YY/MM/DD#hh:mm:ss" の場合
		buf[0] = '1'; buf[1] = '9';
		lstrcpyn(buf + 2,pszTimeStamp,18);
	} else lstrcpyn(buf,pszTimeStamp,20);

	//	正しい指定かどうかのチェック
	if (buf[4] != '/' || buf[7] != '/' ||
		buf[10] != '#' || buf[13] != ':' || buf[16] != ':')
		return FALSE;
	buf[4] = buf[7] = buf[10] = buf[13] = buf[16] = '\0';
	if (!isnumber(buf) || !isnumber(buf+5) || !isnumber(buf+8) ||
		!isnumber(buf+11) || !isnumber(buf+14) || !isnumber(buf+17))
		return FALSE;

	int	val;
	SYSTEMTIME st;
	if ((val = ival(buf)) < 1980) val = 1980;
	st.wYear	= (WORD)val;
	if ((val = ival(buf+5)) < 1 || val > 12) val = 1;
	st.wMonth	= (WORD)val;
	if ((val = ival(buf+8)) < 1 || val > 31) val = 1;
	st.wDay		= (WORD)val;
	st.wDayOfWeek	= 0;
	if ((val = ival(buf+11)) < 0 || val > 23) val = 0;
	st.wHour	= (WORD)val;
	if ((val = ival(buf+14)) < 0 || val > 59) val = 0;
	st.wMinute	= (WORD)val;
	if ((val = ival(buf+17)) < 0 || val > 59) val = 0;
	st.wSecond	= (WORD)val;
	st.wMilliseconds	= 0;
	FILETIME ftlocal;
	::SystemTimeToFileTime(&st,&ftlocal);
	::LocalFileTimeToFileTime(&ftlocal,pft);

	return TRUE;
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

