//	$Id: ms_handler.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	ms_handler.cpp
 *	メニューサービスの実装
 */

#include <exception>
#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "misc.h"

int
ConvData::On_menu(CmdLineParser& params)
{
	int	ac = params.initSequentialGet();
	if (ac < 1) return FALSE;

	//	名前で指定されたメニュー(とそのサブメニュー)を構築
	const StringBuffer& name = params.getNextArgvStr();

	//	メニュー表示位置の取得
	POINT MenuPos;
	LPPOINT pAt = NULL;
	if (ac > 2) {
		MenuPos.x = MenuPos.y = 0;
		pAt = &MenuPos;
		DWORD unit = GetDialogBaseUnits(DdeServer::gethwndDlg());
		LPCSTR av = params.getNextArgv();
		if (isnumber(av)) MenuPos.x = (ival(av)*LOWORD(unit));
		av = params.getNextArgv();
		if (isnumber(av)) MenuPos.y = (ival(av)*HIWORD(unit));
		if (ac > 3) {
			av = params.getNextArgv();
			HWND hwndTemp = NULL;
			if (lstrcmpi(av,"lastactiveparent") == 0)
				hwndTemp = ::GetForegroundWindow();
			else if (isnumber(av))
				hwndTemp = (HWND)ival(av);
			else {
				LPCSTR	wname = params.getNextArgv();
				if (wname != NULL && *wname == '\0') wname = NULL;
				hwndTemp = ::FindWindow(av,wname);
			}
			if (::IsWindow(hwndTemp)) {
				::ClientToScreen(hwndTemp,pAt);
			}
		}
	}
	//	このスレッドの停止は ConvData::onAdvReq() で行っている
	return this->showMenuThread(name,pAt);
}

int
ConvData::On_newmenu(CmdLineParser& params)
{
	return this->si_newmenu(params.getArgvStr(0),FALSE);
}

int
ConvData::On_newmenuex(CmdLineParser& params)
{
	return this->si_newmenu(params.getArgvStr(0),TRUE);
}

int
ConvData::On_menuitem(CmdLineParser& params)
{
	return this->si_addmenuitem(nullStr,
								params.getArgvStr(0),
								params.getArgvStr(1),
								nullStr);
}

int
ConvData::On_menuitemex(CmdLineParser& params)
{
	return this->si_addmenuitem(nullStr,
								params.getArgvStr(0),
								params.getArgvStr(1),
								params.getArgvStr(2));
}

int
ConvData::On_changemenuitemex(CmdLineParser& params)
{
	return this->si_changemenuitem(params.getArgvStr(0),
									ival(params.getArgv(1)),
									params.getArgvStr(2),
									params.getArgvStr(3));
}

int
ConvData::On_deletemenuitemex(CmdLineParser& params)
{
	return this->si_deletemenuitem(params.getArgvStr(0),
									ival(params.getArgv(1)));
}

