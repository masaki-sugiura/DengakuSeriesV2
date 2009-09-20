//	$Id: ds_handler.cpp,v 1.9 2009-09-20 13:49:01 sugiura Exp $
/*
 *	ds_handler.cpp
 *	ダイアログサービスの実装
 */

#include <exception>
#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"

//	tree コントロールの item/setitem の引数の順序が DLL と異なるために
//	si_****() メソッドに渡す前に順序変更をする必要がある…sigh..
#include "ctrldata.h"

int
ConvData::On_dialog(CmdLineParser& params)
{
	HWND hwndOwner = DdeServer::gethwndDlg(), hwndTemp = NULL;
	BOOL bOnTop = FALSE;
	params.initSequentialGet(0);
	LPCSTR av;
	while ((av = params.getNextArgv()) != NULL) {
		if (!lstrcmpi(av,"alwaysontop")) {
			bOnTop = TRUE;
			break;
		} else if (!lstrcmpi(av,"lastactiveparent")) {
			hwndTemp = ::GetForegroundWindow();
		} else if (isnumber(av)) {
			hwndTemp = (HWND)lval(av);
		} else {
			LPCSTR wname = params.getNextArgv();
			if (wname != NULL && *wname == '\0') wname = NULL;
			hwndTemp = ::FindWindow(av,wname);
		}
		if (::IsWindow(hwndTemp)) hwndOwner = hwndTemp;
	}
	if (this->si_showdialog(hwndOwner, bOnTop)) return TRUE;
	ErrorMessageBox(MSG_ERR_CREATEDLG);
	return FALSE;
}

int
ConvData::On_enddialog(CmdLineParser& params)
{
	return this->si_enddialog();
}

int
ConvData::On_setstate(CmdLineParser& params)
{
	StringBuffer name = params.getArgvStr(0);
	params.delArgv(0);
	return this->si_setctrlstate(name,params);
}

int
ConvData::On_setstring(CmdLineParser& params)
{
	return this->si_setctrlstring(params.getArgvStr(0),params.getArgvStr(1));
}

int
ConvData::On_setimestate(CmdLineParser& params)
{
	return this->si_setctrlimestate(params.getArgvStr(0),
									ival(params.getArgvStr(1)));
}

int
ConvData::On_setsort(CmdLineParser& params)
{
	StringBuffer name = params.getArgvStr(0);
	params.delArgv(0);
	return this->si_setctrlsort(name, params);
}

int
ConvData::On_changeitem(CmdLineParser& params)
{
	StringBuffer name = params.getArgvStr(0);
	params.delArgv(0);
	StringBuffer pos = nullStr;
	if (params.itemNum() > 1) {
		pos = params.getArgvStr(-1);
		params.delArgv(-1);
	}
	return this->si_changectrlitem(name,params,pos);
}

int
ConvData::On_setitem(CmdLineParser& params)
{
	StringBuffer name = params.getArgvStr(0);
	params.delArgv(0);
	StringBuffer pos = nullStr;
	if (params.itemNum() > 1) {
		CtrlListItem* pCtrl = this->getDlgFrame().getCtrl(name);
		if (pCtrl != NULL && pCtrl->getCtrlType() == CTRLID_TREE) {
			pos = params.getArgvStr(0);
			params.delArgv(0);
		} else {
			pos = params.getArgvStr(-1);
			params.delArgv(-1);
		}
	}
	return this->si_setctrlitem(name,params,pos);
}

int
ConvData::On_deleteitem(CmdLineParser& params)
{
	return this->si_deletectrlitem(params.getArgvStr(0),
									params.getArgvStr(1));
}

int
ConvData::On_resetlist(CmdLineParser& params)
{
	return this->si_resetctrlitem(params.getArgvStr(0));
}

int
ConvData::On_enablectrl(CmdLineParser& params)
{
	BOOL bEnable = ival(params.getArgv(0));
	params.delArgv(0);
	return this->si_enablectrl(bEnable,params);
}

int
ConvData::On_setctrlfont(CmdLineParser& params)
{
	StringBuffer name = params.getArgvStr(0);
	params.delArgv(0);
	return this->si_setctrlfont(name,params);
}

int
ConvData::On_setfocuseditem(CmdLineParser& params)
{
	return this->si_setctrlfocuseditem(params.getArgvStr(0), params.getArgvStr(1));
}

int
ConvData::On_message(CmdLineParser& params)
{
	return this->si_showmessage(params.getArgvStr(0),params.getArgvStr(1));
}

int
ConvData::On_question(CmdLineParser& params)
{
	return this->si_showquestion(params.getArgvStr(0),
								params.getArgvStr(1),
								params.getArgvStr(2));
}

int
ConvData::On_savedialog(CmdLineParser& params)
{
	return this->si_savedialog(params.getArgvStr(0),params.getArgvStr(1));
}

int
ConvData::On_loaddialog(CmdLineParser& params)
{
	return this->si_loaddialog(params.getArgvStr(0),params.getArgvStr(1));
}

int
ConvData::On_setdlgtitle(CmdLineParser& params)
{
	return this->si_setdlgtitle(params.getArgvStr(0));
}

int
ConvData::On_setdlgpos(CmdLineParser& params)
{
	int x, y;
	x = ival(params.getArgv(0));
	y = ival(params.getArgv(1));
	return this->si_setdlgpos(x, y, params.getArgvStr(2), params.getArgvStr(3));
}

int
ConvData::On_setdlgimestate(CmdLineParser& params)
{
	return this->si_setdlgimestate(ival(params.getArgv(0)));
}

StringBuffer
ConvData::On_getdlgtitle(CmdLineParser&)
{
	return this->si_getdlgtitle();
}

StringBuffer
ConvData::On_getstate(CmdLineParser& params)
{
	return this->si_getctrlstate(params.getArgvStr(0));
}

StringBuffer
ConvData::On_getstring(CmdLineParser& params)
{
	return this->si_getctrlstring(params.getArgvStr(0));
}

StringBuffer
ConvData::On_getimestate(CmdLineParser& params)
{
	return this->si_getctrlimestate(params.getArgvStr(0));
}

StringBuffer
ConvData::On_getsort(CmdLineParser& params)
{
	return this->si_getctrlsort(params.getArgvStr(0));
}

StringBuffer
ConvData::On_getctrlfont(CmdLineParser& params)
{
	return this->si_getctrlfont(params.getArgvStr(0));
}

StringBuffer
ConvData::On_getitem(CmdLineParser& params)
{
	return this->si_getctrlitem(params.getArgvStr(0),params.getArgvStr(1));
}

StringBuffer
ConvData::On_getfocuseditem(CmdLineParser& params)
{
	return this->si_getctrlfocuseditem(params.getArgvStr(0));
}

StringBuffer
ConvData::On_getsignature(CmdLineParser& params)
{
	return this->si_getdlgsignature(params.getArgvStr(0));
}

int
ConvData::On_newdialog(CmdLineParser& params)
{
	RealCmdLineParser argv(nullStr);
	return this->si_newdialog(params.getArgvStr(1),
							ival(params.getArgv(0)),
							argv);
}

int
ConvData::On_newdialogex(CmdLineParser& params)
{
	StringBuffer title = params.getArgvStr(0);
	WORD width = ival(params.getArgv(3));
	params.delArgv(3);
	params.delArgv(0);
	return this->si_newdialog(title,width,params);
}

int
ConvData::On_newdlgpage(CmdLineParser& params)
{
	return this->si_newdlgpage(params.getArgvStr(0),ival(params.getArgv(1)));
}

int
ConvData::On_setcurdlgpage(CmdLineParser& params)
{
	return this->si_setcurdlgpage(params.getArgvStr(0));
}

int
ConvData::On_setfocusedctrl(CmdLineParser& params)
{
	return this->si_setfocusedctrl(params.getArgvStr(0));
}

StringBuffer
ConvData::On_getcurdlgpage(CmdLineParser&)
{
	return this->si_getcurdlgpage();
}

StringBuffer
ConvData::On_getfocusedctrl(CmdLineParser&)
{
	return this->si_getfocusedctrl();
}

StringBuffer
ConvData::On_getdlgpos(CmdLineParser&)
{
	return this->si_getdlgpos();
}

StringBuffer
ConvData::On_getdlgsize(CmdLineParser&)
{
	return this->si_getdlgsize();
}

StringBuffer
ConvData::On_getdlgimestate(CmdLineParser&)
{
	return this->si_getdlgimestate();
}

int
ConvData::On_setctrlexproperty(CmdLineParser& params)
{
	return this->si_setctrlexproperty(params.getArgvStr(0), params.getArgvStr(1), params.getArgvStr(2));
}

StringBuffer
ConvData::On_getctrlexproperty(CmdLineParser& params)
{
	return this->si_getctrlexproperty(params.getArgvStr(0), params.getArgvStr(1));
}

int
ConvData::On_newcontrol(CmdLineParser& params)
{
	return this->si_newcontrol(params.getArgvStr(0),
								params.getArgvStr(1),
								params.getArgvStr(2));
}

int
ConvData::On_item(CmdLineParser& params)
{
	StringBuffer pos = nullStr;
	int num = params.itemNum();
	if (num == 0) {
		params.addArgv(nullStr);
	} else if (num > 1) {
		CtrlListItem* pCtrl = this->getDlgFrame().getCtrl(); // 現在の ctrl
		if (pCtrl != NULL && pCtrl->getCtrlType() == CTRLID_TREE) {
			pos = params.getArgvStr(0);
			params.delArgv(0);
		}
	}
	return this->si_setctrlitem(nullStr,params,pos);
}

int
ConvData::On_height(CmdLineParser& params)
{
	return this->si_setctrlheight(nullStr,ival(params.getArgvStr(0)));
}

int
ConvData::On_default(CmdLineParser& params)
{
	return this->si_setctrlstate(nullStr,params);
}

int
ConvData::On_ctrlfont(CmdLineParser& params)
{
	return this->si_setctrlfont(nullStr,params);
}

int
ConvData::On_focuseditem(CmdLineParser& params)
{
	return this->si_setctrlfocuseditem(nullStr, params.getArgvStr(0));
}

int
ConvData::On_imestate(CmdLineParser& params)
{
	return this->si_setctrlimestate(nullStr,ival(params.getArgv(0)));
}

int
ConvData::On_sort(CmdLineParser& params)
{
	return this->si_setctrlsort(nullStr, params);
}

int
ConvData::On_notify(CmdLineParser& params)
{
	return this->On_notifyex(params);
}

int
ConvData::On_notifyex(CmdLineParser& params)
{
	return this->si_setctrlnotify(nullStr,params);
}

int
ConvData::On_newcolumn(CmdLineParser& params)
{
	return this->si_newcolumn(ival(params.getArgv(0)));
}

int
ConvData::On_newpage(CmdLineParser& params)
{
	return this->si_newpage(ival(params.getArgv(0)));
}

int
ConvData::On_part(CmdLineParser& params)
{
	WORD width;
	if (params.getArgvStr(0).compareTo("half") == 0) width = (WORD)-1;
	else width = ival(params.getArgv(0));
	return this->si_setctrlwidth(nullStr,width);
}

int
ConvData::On_dlgimestate(CmdLineParser& params)
{
	return this->si_setdlgimestate(ival(params.getArgv(0)));
}

