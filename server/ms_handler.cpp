//	$Id: ms_handler.cpp,v 1.4 2003-07-06 16:27:46 sugiura Exp $
/*
 *	ms_handler.cpp
 *	���j���[�T�[�r�X�̎���
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

	//	���O�Ŏw�肳�ꂽ���j���[(�Ƃ��̃T�u���j���[)���\�z
	const StringBuffer& name = params.getNextArgvStr();

	//	���j���[�\���ʒu�̎擾
	BOOL bShowAtCursor = TRUE;
	POINT MenuPos;
	MenuPos.x = MenuPos.y = 0;
	if (ac > 2) {
		bShowAtCursor = FALSE;
		DWORD unit = GetDialogBaseUnits(DdeServer::gethwndDlg());
		LPCSTR av = params.getNextArgv();
		if (isnumber(av)) MenuPos.x = (ival(av)*LOWORD(unit));
		av = params.getNextArgv();
		if (isnumber(av)) MenuPos.y = (ival(av)*HIWORD(unit));
		if (ac > 3) {
			av = params.getNextArgv();
			HWND hwndTemp = NULL;
			if (lstrcmpi(av,"lastactiveparent") == 0) {
				hwndTemp = ::GetForegroundWindow();
			} else if (lstrcmpi(av,"cursor") == 0) {
				bShowAtCursor = TRUE;
			} else if (isnumber(av)) {
				hwndTemp = (HWND)ival(av);
			} else {
				LPCSTR	wname = params.getNextArgv();
				if (wname != NULL && *wname == '\0') wname = NULL;
				hwndTemp = ::FindWindow(av,wname);
			}
			if (hwndTemp && ::IsWindow(hwndTemp)) {
				POINT pt;
				::ClientToScreen(hwndTemp,&pt);
				MenuPos.x += pt.x;
				MenuPos.y += pt.y;
			}
		}
	}

	HWND hwndTop = ::GetForegroundWindow();
	if (hwndTop != NULL) {
		::UpdateWindow(hwndTop);
		::Sleep(1); // ������x�҂��Ȃ��ƍX�V���ǂ����Ȃ��H
	}

	//	���̃X���b�h�̒�~�� ConvData::onAdvReq() �ōs���Ă���
	return this->showMenuThread(name, bShowAtCursor, MenuPos);
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

