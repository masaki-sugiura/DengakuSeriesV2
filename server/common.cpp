//	$Id: common.cpp,v 1.39.2.1 2005-08-01 16:29:56 sugiura Exp $
/*
 *	common.cpp
 *	その他サーバ特有の定義
 */

#include "common.h"

LPCSTR svr_strings[] = {
	"",
	"2.80",
	"田楽(でんがく)サーバ",
	"DengakuServer",
	"HideMacroServer",
	"macserv.exe",
	"DengakuServerClass",
	"DGSvrMutex",
	"AllService\n"
		"CommonService\n"
		"FileService\n"
		"StringService\n"
		"MenuService\n"
		"DlgService\n"
		"ComDlgService\n"
		"SusieService\n"
		"GlobVarService\n"
		"BregexpService\n"
		"ColorService\n"
};

LPCSTR msg_strings[] = {
	"田楽サーバの起動に失敗しました。",
	"田楽サーバの初期化に失敗しました。",
	"田楽サーバの停止に失敗しました。",
	"ダイアログの作成に失敗しました。"
};

const StringBuffer existStr = "!";

#ifdef _DEBUG
DWORD
OutputErrorMsgToDebugTerminal(LPCSTR header)
{
	DWORD dwLastError = ::GetLastError();
	if (dwLastError != 0L) {
		static TCHAR buf[1024];
		wsprintf(buf,"%s:[%d] ",header,dwLastError);
		int	len = lstrlen(buf);
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dwLastError,
						MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
						buf+len,
						1024-len,
						NULL);
		::OutputDebugString(buf);
	}
	return dwLastError;
}
#endif
