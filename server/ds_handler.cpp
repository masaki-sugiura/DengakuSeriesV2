//	$Id: ds_handler.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	ds_handler.cpp
 *	�_�C�A���O�T�[�r�X�̎���
 */

#include <exception>
#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"

//	tree �R���g���[���� item/setitem �̈����̏����� DLL �ƈقȂ邽�߂�
//	si_****() ���\�b�h�ɓn���O�ɏ����ύX������K�v������csigh..
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

StringBuffer
ConvData::On_getcurdlgpage(CmdLineParser&)
{
	return this->si_getcurdlgpage();
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
		CtrlListItem* pCtrl = this->getDlgFrame().getCtrl(); // ���݂� ctrl
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
ConvData::On_imestate(CmdLineParser& params)
{
	return this->si_setctrlimestate(nullStr,ival(params.getArgv(0)));
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
