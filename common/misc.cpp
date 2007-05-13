//	$Id: misc.cpp,v 1.21 2007-05-13 17:02:41 sugiura Exp $
/*
 *	misc.cpp
 *	雑多なユーティリティ関数
 */

#include "strutils.h"
#include "strbuf.h"
#include "misc.h"

#include <stdarg.h>
#include <stdio.h>

void
DebugOutput(LPCSTR pszFormat, ...)
{
	char buf[1024];

	DWORD dwThreadID = ::GetCurrentThreadId();
	wsprintf(buf, "[tid:%08x] ", dwThreadID);

	int len = strlen(buf);

	va_list list;
	va_start(list, pszFormat);

	wvsprintf(buf + len, pszFormat, list);

	len += strlen(buf + len);
	if (len < sizeof(buf) - 1) {
		strcpy(buf + len, "\n");
	}

	::OutputDebugString(buf);
}

BOOL
isWinNT()
{
	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!::GetVersionEx(&osi)) return FALSE;
	return osi.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

DWORD
GetVersionInfo(HINSTANCE hInstance)
{
	HRSRC hVersion = ::FindResource((HMODULE)hInstance, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (hVersion == NULL)
	{
		return 0;
	}

	HGLOBAL hVersion2 = ::LoadResource((HMODULE)hInstance, hVersion);
	if (hVersion2 == NULL)
	{
		return 0;
	}

	LPVOID pVersionString = ::LockResource(hVersion2);
	if (pVersionString == NULL)
	{
		return 0;
	}

	VS_FIXEDFILEINFO* pVerInfo;
	UINT uLength;
	if (!::VerQueryValue(pVersionString, "\\", (LPVOID*)&pVerInfo, &uLength))
	{
		return 0;
	}

	int nMajor = (WORD)(pVerInfo->dwFileVersionMS >> 16), nMinor = (WORD)pVerInfo->dwFileVersionMS,
		nRevision = (WORD)(pVerInfo->dwFileVersionLS >> 16), nRelease = (WORD)pVerInfo->dwFileVersionLS;

	return ((BYTE)nMajor << 24) | ((BYTE)nMinor << 16) | ((BYTE)nRevision << 8) | (BYTE)nRelease;
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
MyGetWindowText(HWND hWnd, StringBuffer& strText)
{
	if (!hWnd) {
		MessageBox(NULL, "ウィンドウハンドルがNULLのため文字列の取得に失敗", NULL, MB_OK);
		return FALSE;
	}

	//	エラーコードをクリア
	::SetLastError(0);

	int nSize = ::GetWindowTextLength(hWnd);
	if (nSize == 0) {
		DWORD dwErr = ::GetLastError();
		if (dwErr != NOERROR) {
			TCHAR buf[80];
			wsprintf(buf, "エラーコード 0x%08x のため文字列の取得に失敗", dwErr);
			MessageBox(NULL, buf, NULL, MB_OK);
			return FALSE;
		}
		strText.reset();
		return TRUE;
	}

	TCHAR szBuf[128];
	LPTSTR pszText = szBuf;

	if (nSize + 1 > 128) {
		pszText = new TCHAR[nSize + 1];
	}

	::GetWindowText(hWnd, pszText, nSize + 1);

	strText.reset(pszText);

	if (pszText != szBuf) {
		delete [] pszText;
	}

	return TRUE;
}

BOOL
MySetWindowText(HWND hWnd, const StringBuffer& strText)
{
	if (!hWnd) {
		return FALSE;
	}

#if 0
	TCHAR wBuf[128];
	LPTSTR pszText = szBuf;

	int nSize = strText.toUnicode(NULL);
	if (nSize + 1 > 128) {
		pwszText = new WCHAR[nSize + 1];
	}

	strText.toUnicode(pwszText);

	::SetWindowTextW(hWnd, pwszText);

	if (pwszText != wBuf) {
		delete [] pwszText;
	}
#else
	::SetWindowText(hWnd, (LPCSTR)strText);
#endif

	return TRUE;
}

HWND SetFocusForced(HWND hwndFocus)
{
	DWORD dwCurThreadID = ::GetCurrentThreadId();

	DWORD dwDlgThreadID = ::GetWindowThreadProcessId(hwndFocus, NULL);

	if (dwDlgThreadID != dwCurThreadID) {
		DEBUG_OUTPUT(("Dialog Thread ID = %d, Current Thread ID = %d", dwDlgThreadID, dwCurThreadID));
		BOOL bRet = ::AttachThreadInput(dwDlgThreadID, dwCurThreadID, TRUE);
		if (!bRet) {
			DEBUG_OUTPUT(("Failed to attach thread!!"));
		}
	}

	HWND hwndPrevFocus = ::SetFocus(hwndFocus);
	if (hwndPrevFocus == NULL) {
		DWORD dwErr = ::GetLastError();
		DEBUG_OUTPUT(("Failed to set focus: err = %08x", dwErr));
	}

	if (dwDlgThreadID != dwCurThreadID) {
		::AttachThreadInput(dwDlgThreadID, dwCurThreadID, FALSE);
	}

	return hwndPrevFocus;
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

BOOL
MySetWindowText(HWND hWnd, LPCSTR pszText)
{
#if 0
	int size = ::MultiByteToWideChar(CP_THREAD_ACP,
									 MB_PRECOMPOSED,
									 pszText,-1,
									 NULL, 0);

	LPWSTR wbuf = new WCHAR[size + 1];

	::MultiByteToWideChar(CP_THREAD_ACP,
						  MB_PRECOMPOSED,
						  pszText,-1,
						  wbuf, size + 1);

	BOOL bRet = ::SetWindowTextW(hWnd, wbuf);

	delete [] wbuf;
#else
	BOOL bRet = ::SetWindowText(hWnd, pszText);
#endif

	return bRet;
}
