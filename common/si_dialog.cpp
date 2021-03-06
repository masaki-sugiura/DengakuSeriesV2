//	$Id: si_dialog.cpp,v 1.29 2011/01/07 16:08:38 sugiura Exp $
/*
 *	si_dialog.cpp
 *	ダイアログ操作関数
 */

#include "session.h"
#include "dlgdata.h"
#include "ctrldata.h"
#include "cmdline.h"
#include "ddfile.h"
#include "pathname.h"
#include "dirlist.h"
#include "misc.h"
#include "str_tbl.h"

static BOOL
PreDispatchKeyPress(HWND hDlg, MSG* lpMsg)
{
	if (lpMsg->message != WM_KEYDOWN ||
		lpMsg->wParam != VK_TAB ||
		(GetKeyState(VK_CONTROL) & 0x8000) == 0) return FALSE;

	HWND hFocusedCtrl = ::GetFocus();
	if (hFocusedCtrl == NULL) return FALSE;
	CtrlListItem*
		pCtrl = (CtrlListItem*)::SendMessage(hFocusedCtrl, WM_GET_CTRL_PTR, 0, 0);
	if (!pCtrl->isValid() || pCtrl->getCtrlType() != CTRLID_TAB) return FALSE;

	// tab コントロールにメッセージを転送
	NMTCKEYDOWN nm;
	nm.hdr.code = TCN_KEYDOWN;
	nm.hdr.hwndFrom = hDlg;
	nm.hdr.idFrom = 0; // not used
	nm.wVKey = VK_TAB;
	nm.flags = lpMsg->lParam;
	pCtrl->onWmNotify(0, (LPARAM)&nm);

	return TRUE;
}


DWORD WINAPI
ShowDlgProc(LPDWORD pThreadArgs)
{
	if (pThreadArgs == NULL) {
		::ExitThread((DWORD)-1); // 有り得ない!!
	}

	::InitCommonControls();

//	::SetThreadLocale(MAKELCID(0x040c, SORT_DEFAULT));

	LPSHOWDLGPROCARGS psdpa = (LPSHOWDLGPROCARGS)pThreadArgs;
	SessionInstance* psi = psdpa->m_psi;
	DlgFrame& df = psi->getDlgFrame();

	DWORD ret = 1;
//	StringBuffer* ntf = new StringBuffer(okStr);
	const StringBuffer okStr("OK"), ngStr("NG");
	HWND hwndDlg;
	if ((hwndDlg = df.createFrame(psdpa->m_hwndOwner,psdpa->m_bOnTop))
		== NULL) {
//		*ntf = ngStr;
		psi->SessionInstance::setNotify(ngStr);
	} else {
//		psi->SessionInstance::setNotify(*ntf);

		psi->SessionInstance::setNotify(okStr);

		MSG	msg;
		while (::GetMessage(&msg,NULL,0,0)) {
			if (!PreDispatchKeyPress(hwndDlg, &msg) &&
				!::IsDialogMessage(hwndDlg, &msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}

		ret = (DWORD)msg.wParam;
	}
//	delete ntf;
	delete psdpa;

	::ExitThread(ret);

	return (DWORD)-1;	//	有り得ない！！
}

static WORD
ParseDlgPosOrigin(const StringBuffer& sflag)
{
	WORD wflags = DLGPOS_CLIENT_CENTER;
	if (sflag.length() > 0) {
		if (sflag.compareTo("screen", FALSE) == 0) {
			wflags = DLGPOS_SCREEN_COORD;
		} else if (sflag.compareTo("window", FALSE) == 0) {
			wflags = DLGPOS_CLIENT_COORD;
		} else if (sflag.compareTo("center", FALSE) == 0) {
			wflags = DLGPOS_SCREEN_CENTER;
		} else if (sflag.compareTo("caret", FALSE)  == 0) {
			wflags = DLGPOS_CARET_COORD;
		} else if (sflag.compareTo("cursor", FALSE) == 0) {
			wflags = DLGPOS_CURSOR_COORD;
		} else {
			wflags = DLGPOS_CLIENT_CENTER;
		}
	}
	return wflags;
}

static WORD
ParseDlgPosUnit(const StringBuffer& str)
{
	//	offset unit
	if (str.compareTo("px", FALSE) == 0) {
		return DLGPOS_UNIT_PIXEL;
	}
	return DLGPOS_UNIT_FONTSIZE;
}

//static HWND s_hwndOwner;

int
SessionInstance::si_showdialog(HWND hwndOwner, BOOL bOnTop)
{
	if (m_pDlgThread.ptr() != NULL) return FALSE;

	if (hwndOwner == NULL) {
		//	lastactiveparent
		hwndOwner = ::GetForegroundWindow();
	} else if (!::IsWindow(hwndOwner)) {
		hwndOwner = NULL;
	}

	LPSHOWDLGPROCARGS psdpa = new SHOWDLGPROCARGS;
	psdpa->m_psi = this;
	psdpa->m_hwndOwner = hwndOwner;
	psdpa->m_bOnTop = bOnTop;

	//	これを別スレッド内で行うと、ENDDIALOG -> SHOWDIALOG で落ちる？
	this->resetNotify();

	//	メニューを表示するスレッドの開始
	m_pDlgThread
		= new Thread((LPTHREAD_START_ROUTINE)ShowDlgProc,(LPDWORD)psdpa);
	if (m_pDlgThread.ptr() == NULL) return FALSE;
	if (!m_pDlgThread->run()) {
		m_pDlgThread = NULL;
		return FALSE;
	}

	StringBuffer buf(nullStr);
	while (!this->SessionInstance::getNotify(buf, 1)) {
		/* no op. */;
	}
	if (buf.compareTo(okStr) != 0) {
		m_pDlgThread = NULL;
		return FALSE;
	}

	::SetForegroundWindow(m_DlgFrame.getUserDlg());
	m_DlgFrame.setFocusedCtrl(m_DlgFrame.getFocusedCtrlName());
	m_DlgFrame.showFrame();

//	::EnableWindow(hwndOwner, FALSE);
//	s_hwndOwner = hwndOwner;

	return TRUE;
}

int
SessionInstance::si_enddialog(DWORD waittime)
{
	if (!m_DlgFrame.closeFrame()) return FALSE;
	//	this->resetNotify();
//	::EnableWindow(s_hwndOwner, TRUE);
	DEBUG_OUTPUT(("Enter DlgThread Stop"));
	m_pDlgThread->stop(waittime); // スレッドが終了するまで待つ
	DEBUG_OUTPUT(("Leave DlgThread Stop"));
	DWORD ret = m_pDlgThread->getExitCode();
	m_pDlgThread = NULL;
	DEBUG_OUTPUT(("Dengaku::enddialog finished!!"));
	return ret == 0;
}

int
SessionInstance::si_setctrlstate(
	const StringBuffer& str1,
	CmdLineParser& argv)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str1);
	return cli != NULL ? cli->onSetState(argv) : FALSE;
}

int
SessionInstance::si_setctrlstring(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str1);
	return cli != NULL ? cli->onSetString(str2) : FALSE;
}

int
SessionInstance::si_setctrlimestate(const StringBuffer& str, int state)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str);
	return cli != NULL ? cli->onSetImeState(state) : FALSE;
}

int
SessionInstance::si_changectrlitem(
	const StringBuffer& str1,
	CmdLineParser& argv,
	const StringBuffer& pos)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str1);
	return cli != NULL ? cli->onSetItem(argv,pos) : FALSE;
}

int
SessionInstance::si_setctrlitem(
	const StringBuffer& str1,
	CmdLineParser& argv,
	const StringBuffer& pos)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str1);
	return cli != NULL ? cli->onInsertItem(argv,pos) : FALSE;
}

int
SessionInstance::si_deletectrlitem(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str1);
	return cli != NULL ? cli->onDeleteItem(str2) : FALSE;
}

int
SessionInstance::si_resetctrlitem(const StringBuffer& str)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str);
	return cli != NULL ? cli->onResetList() : FALSE;
}

int
SessionInstance::si_enablectrl(
	BOOL bEnable,
	CmdLineParser& argv)
{
	if (argv.initSequentialGet() <= 0) return FALSE;
	CtrlListItem* cli;
	for (;;) {
		const StringBuffer& name = argv.getNextArgvStr();
		if (name.length() <= 0) break;
		if ((cli = m_DlgFrame.getCtrl(name)) != NULL)
			cli->enableCtrl(bEnable,TRUE);
	}
	return TRUE;
}

int
SessionInstance::si_setctrlfont(
	const StringBuffer& str1,
	CmdLineParser& argv)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str1);
	return cli != NULL ? cli->onSetCtrlFont(argv) : FALSE;
}

int
SessionInstance::si_showmessage(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	HWND hDlg = m_DlgFrame.getUserDlg();
	if (hDlg == NULL) return FALSE;

	LPARAM notify = (WORD)ival(str1);
	StringBuffer* msgbuf = new StringBuffer(str2);
	return ::PostMessage(hDlg,WM_USER_MESSAGEBOX,(WPARAM)msgbuf,notify);
}

int
SessionInstance::si_showquestion(
	const StringBuffer& str1,
	const StringBuffer& str2,
	const StringBuffer& str3)
{
	HWND hDlg = m_DlgFrame.getUserDlg();
	if (hDlg == NULL) return FALSE;

	LPARAM notify = 0xFFFF0000&(ival(str1)<<16);
	notify += 0x0000FFFF&ival(str2);
	StringBuffer* msgbuf = new StringBuffer(str3);
	return ::PostMessage(hDlg,WM_USER_QUESTIONBOX,(WPARAM)msgbuf,notify);
}

int
SessionInstance::si_savedialog(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	PathName file;
	if (!m_DirList.getPathName(str1,file,FALSE)	|| file.isDirectory())
		return FALSE;

	DlgDataFile ddfile(file);
	ddfile.write(GetString(STR_VERSION),
				GetString(STR_DLGDATA_VERSION),
				GetString(STR_DLGDATA_COMMONPROPERTY));
	ddfile.write(str2,
				GetString(STR_DLGDATA_USERSIGNATURE),
				GetString(STR_DLGDATA_COMMONPROPERTY));

	return m_DlgFrame.dumpData(ddfile);
}

int
SessionInstance::si_loaddialog(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	PathName file;
	if (!m_DirList.getPathName(str1,file,TRUE) || file.isDirectory())
		return FALSE;

	DlgDataFile	ddfile(file);
	if (str2.length() > 0) {
		// signature のチェック
		StringBuffer sbuf(32);
		ddfile.read(sbuf,
					GetString(STR_DLGDATA_USERSIGNATURE),
					GetString(STR_DLGDATA_COMMONPROPERTY));
		if (sbuf.compareTo(str2) != 0) return FALSE;
	}

	return m_DlgFrame.loadData(ddfile);
}

int
SessionInstance::si_setdlgtitle(const StringBuffer& str)
{
	m_DlgFrame.setDlgTitle(str);
	return TRUE;
}

StringBuffer
SessionInstance::si_getdlgtitle()
{
	return m_DlgFrame.getDlgTitle();
}

StringBuffer
SessionInstance::si_getctrlstate(const StringBuffer& str)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str);
	return cli != NULL ? cli->onGetState() : nullStr;
}

StringBuffer
SessionInstance::si_getctrlstring(const StringBuffer& str)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str);
	return cli != NULL ? cli->onGetString() : nullStr;
}

StringBuffer
SessionInstance::si_getctrlimestate(const StringBuffer& str)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str);
	return cli != NULL ? cli->onGetImeState() : nullStr;
}

StringBuffer
SessionInstance::si_getctrlfont(const StringBuffer& str)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str);
	return cli != NULL ? cli->onGetCtrlFont() : nullStr;
}

StringBuffer
SessionInstance::si_getctrlitem(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(str1);
	return cli != NULL ? cli->onGetItem(str2) : nullStr;
}

StringBuffer
SessionInstance::si_getdlgsignature(const StringBuffer& str)
{
	PathName file;
	if (!m_DirList.getPathName(str,file,TRUE) || file.isDirectory())
		return nullStr;
	DlgDataFile	ddfile(file);
	StringBuffer ret(32);
	ddfile.read(ret,
				GetString(STR_DLGDATA_USERSIGNATURE),
				GetString(STR_DLGDATA_COMMONPROPERTY));
	return ret;
}

int
SessionInstance::si_newdialog(
	const StringBuffer& str1,
	WORD width,
	CmdLineParser& argv)
{
	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	//	reset frame
	m_DlgFrame.resetFrame();

	//	dialog width
	if (width < 2) width = 2;

	m_DlgFrame.addPage(strRootPageName,width);

	//	more frame options
	POINTS pt = {0,0};
	WORD wflags = 0;
	StringBuffer strFont = nullStr;
	WORD wFontSize = 0;
	WORD wPosUnit = DLGPOS_UNIT_FONTSIZE;

	if (argv.itemNum() > 0) {
		argv.initSequentialGet();

		//	frame position
		LPCSTR pstr = argv.getNextArgv();
		pt.x = ival(pstr);
		pstr = argv.getNextArgv();
		pt.y = ival(pstr);

		//	frame position flag
		wflags = ParseDlgPosOrigin(argv.getNextArgvStr());

		//	font properties
		strFont = argv.getNextArgvStr();
		pstr = argv.getNextArgv();
		wFontSize = ival(pstr);

		//	offset unit
		wPosUnit = ParseDlgPosUnit(argv.getNextArgvStr());
	}
	m_DlgFrame.setFrameProperty(str1,&pt,wflags,strFont,wFontSize,wPosUnit);

	return TRUE;
}

int
SessionInstance::si_newdlgpage(
	const StringBuffer& name,
	WORD width)
{
	if (name.length() <= 0) return FALSE;

	if (width < 2) width = 2;

	return m_DlgFrame.addPage(name,width);
}

int
SessionInstance::si_setcurdlgpage(const StringBuffer& name)
{
	return m_DlgFrame.setCurrentPage(name);
}

int
SessionInstance::si_setfocusedctrl(const StringBuffer& name)
{
	return m_DlgFrame.setFocusedCtrl(name);
}

StringBuffer
SessionInstance::si_getcurdlgpage()
{
	DlgPage* pdp = m_DlgFrame.getPage();
	return pdp != NULL ? pdp->getName() : nullStr;
}

StringBuffer
SessionInstance::si_getfocusedctrl()
{
//	return m_DlgFrame.getFocusedCtrl();
	return (LPCSTR)::SendMessage(m_DlgFrame.getUserDlg(),
								 WM_USER_GETFOCUSEDCTRL,
								 0, 0);
}

int
SessionInstance::si_newcontrol(
	const StringBuffer& type,
	const StringBuffer& name,
	const StringBuffer& text)
{
	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	DlgPage* pCurPage = m_DlgFrame.getPage();
	if (pCurPage == NULL) return FALSE;

	CTRL_ID	ctrltype = CtrlListItem::getCtrlTypeFromName(type);
	if (ctrltype == CTRLID_UNKNOWN) return FALSE;

	return pCurPage->addCtrl(ctrltype,name,text) > 0;
}

int
SessionInstance::si_setctrlwidth(const StringBuffer& name, WORD width)
{
	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	CtrlListItem* cli = m_DlgFrame.getCtrl(name);
	return cli != NULL ? cli->onSetWidth(width) : FALSE;
}

int
SessionInstance::si_setctrlheight(const StringBuffer& name, WORD height)
{
	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	CtrlListItem* cli = m_DlgFrame.getCtrl(name);
	return cli != NULL ? cli->onSetHeight(height) : FALSE;
}

int
SessionInstance::si_setctrlnotify(
	const StringBuffer& name,
	CmdLineParser& ntf)
{
	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	CtrlListItem* cli = m_DlgFrame.getCtrl(name);
	return cli != NULL ? cli->onSetNotify(ntf) : FALSE;
}

int
SessionInstance::si_setctrlsort(
	const StringBuffer& name,
	CmdLineParser& sstate)
{
//	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	CtrlListItem* cli = m_DlgFrame.getCtrl(name);
	return cli != NULL ? cli->onSetSort(sstate) : FALSE;
}

StringBuffer
SessionInstance::si_getctrlsort(const StringBuffer& name)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(name);
	return cli != NULL ? cli->onGetSort() : nullStr;
}

int
SessionInstance::si_newcolumn(WORD width)
{
	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	DlgPage* pCurPage = m_DlgFrame.getPage();
	if (pCurPage == NULL) return FALSE;

	return pCurPage->addCtrl(CTRLID_NEWCOLUMN,
							nullStr,
							StringBuffer(4).append(width)) > 0;
}

int
SessionInstance::si_newpage(WORD width)
{
	if (m_DlgFrame.getUserDlg() != NULL) return FALSE;

	DlgPage* pCurPage = m_DlgFrame.getPage();
	if (pCurPage == NULL) return FALSE;

	return pCurPage->addCtrl(CTRLID_NEWPAGE,nullStr,
							StringBuffer(4).append(width)) > 0;
}

StringBuffer
SessionInstance::si_getdlgpos()
{
	POINTS pos = m_DlgFrame.getDlgPos();
	TCHAR buf[32];
	wsprintf(buf, "%d,%d", pos.x, pos.y);
	return buf;
}

int
SessionInstance::si_setdlgpos(int x, int y,
							  const StringBuffer& strOrigin,
							  const StringBuffer& strUnit)
{
	POINTS pos;
	pos.x = x;  pos.y = y;
	m_DlgFrame.setDlgPos(pos,
						 ParseDlgPosOrigin(strOrigin),
						 ParseDlgPosUnit(strUnit));
	return 1;
}

StringBuffer
SessionInstance::si_getdlgsize()
{
	HWND hwndDlg = m_DlgFrame.getUserDlg();
	if (!hwndDlg) {
		return "";
	}

	RECT rcDlg;
	::GetWindowRect(hwndDlg, &rcDlg);

	TCHAR buf[32];
	wsprintf(buf, "%d,%d", rcDlg.right - rcDlg.left, rcDlg.bottom - rcDlg.top);
	return buf;
}

int
SessionInstance::si_setdlgimestate(int nState)
{
	return m_DlgFrame.setImeState(nState);
}

StringBuffer
SessionInstance::si_getdlgimestate()
{
	int nState = m_DlgFrame.getImeState();
	TCHAR buf[32];
	wsprintf(buf, "%d", nState);
	return buf;
}

int
SessionInstance::si_setctrlfocuseditem(const StringBuffer& ctrl, const StringBuffer& item)
{
	return m_DlgFrame.setCtrlFocusedItem(ctrl, item);
}

StringBuffer
SessionInstance::si_getctrlfocuseditem(const StringBuffer& ctrl)
{
	return m_DlgFrame.getCtrlFocusedItem(ctrl);
}

int
SessionInstance::si_setctrlexproperty(const StringBuffer& ctrl, const StringBuffer& key, const StringBuffer& value)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(ctrl);
	return cli != NULL ? cli->onSetCtrlExProperty(key, value) : 0;
}

StringBuffer
SessionInstance::si_getctrlexproperty(const StringBuffer& ctrl, const StringBuffer& key)
{
	CtrlListItem* cli = m_DlgFrame.getCtrl(ctrl);
	return cli != NULL ? cli->onGetCtrlExProperty(key) : nullStr;
}

int
SessionInstance::si_setdlgexproperty(const StringBuffer& name, const StringBuffer& key, const StringBuffer& value)
{
	DlgPage*	page = m_DlgFrame.getPage(name);
	if (page == NULL)
	{
		return 0;
	}

	return page->setExProperty(key, value);
}

StringBuffer
SessionInstance::si_getdlgexproperty(const StringBuffer& name, const StringBuffer& key)
{
	DlgPage*	page = m_DlgFrame.getPage(name);
	if (page == NULL)
	{
		return nullStr;
	}

	return page->getExProperty(key);
}
