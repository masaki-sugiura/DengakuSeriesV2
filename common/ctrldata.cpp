//	$Id: ctrldata.cpp,v 1.9 2002-02-20 15:16:53 sugiura Exp $
/*
 *	ctrldata.cpp
 *	コントロールを扱うクラス
 */

//	チェックリストで使うチェックボックスのビットマップを得るためのおまじない
#define	OEMRESOURCE

#include "strutils.h"
#include "session.h"
#include "ctrldata.h"
#include "dlgdata.h"
#include "cmdline.h"
#include "tokenizer.h"
#include "misc.h"
#include "str_tbl.h"
#include "colortbl.h"

#include <exception>
#include <imm.h>

BOOL
CtrlListItem::States::dumpData(DlgDataFile& ddfile) const
{
	StringBuffer buf(32);
	int last = (int)m_states.getSize();
	for (int i = 0; i < last; i++) {
		if (m_states.getBit(i)) {
			if (buf.length() > 0) buf.append((TCHAR)',');
			buf.append(i+1);
		}
	}
	ddfile.write(buf,GetString(STR_DLGDATA_STATES));
	return TRUE;
}

BOOL
CtrlListItem::States::loadData(DlgDataFile& ddfile)
{
	m_states.clear();
	StringBuffer buf(32);
	ddfile.read(buf,GetString(STR_DLGDATA_STATES));
	Tokenizer tknStates(buf, ", ", TRUE); // skip null str
	while (tknStates.hasMoreTokens()) {
		int index = ival(tknStates.getNextToken()) - 1;
		if (index > 0) m_states.setBit(index, TRUE);
	}
	return TRUE;
}


CtrlListItem::CtrlProperty::CtrlProperty(CtrlListItem* pCtrl)
	:	m_pCtrl(pCtrl),
		m_hwndCtrl(NULL),
		m_classname(NULL),
		m_pfnDefCallback(NULL),
		m_style(0), m_exstyle(0),
		m_id(0),
		m_bufsize(0),
		m_text(32)
{
//	::ZeroMemory(&m_fontprop, sizeof(CtrlFontProperty));
}

CtrlListItem::CtrlProperty::~CtrlProperty()
{
	if (m_fontprop.m_hfont) ::DeleteObject(m_fontprop.m_hfont);
}

void
CtrlListItem::CtrlProperty::setCtrlTemplate(CtrlTemplateArgs &cta)
/*
	CtrlTemplateArgs::
		WORD*	m_hdr	:	header of DLGITEMTEMPLATE buffer
		WORD	m_id	:	ctrl-id
			Note: In this method above two values are incremented for the next control.
		WORD	m_x - m_cy	:	metrices of the control.
*/
{
	LPWORD& lphdr = cta.m_hdr;
	DLGITEMTEMPLATE* lpdit = (DLGITEMTEMPLATE*)lphdr;
	lpdit->style			= m_style;
	lpdit->dwExtendedStyle	= m_exstyle;
	lpdit->x				= cta.m_x;
	lpdit->y				= cta.m_y;
	lpdit->cx				= cta.m_cx;
	lpdit->cy				= cta.m_cy;
	lpdit->id				= m_id = cta.m_id++;	//	increment internally.
	lphdr += sizeof(DLGITEMTEMPLATE) / sizeof(WORD);
	if (m_bufsize == 1) {
		*lphdr++ = 0xFFFF;
		*lphdr++ = LOWORD((DWORD)m_classname);
	} else {
		lphdr += StringBuffer(m_classname).toUnicode((LPWSTR)lphdr);
	}
	lphdr += m_text.toUnicode((LPWSTR)lphdr);
	/*	EXDATA of DLGITEMTEMPLATE should be aligned to DWORD boundary */
	if (((DWORD)lphdr) & (sizeof(DWORD) - 1)) lphdr++;
	else lphdr += 2;
}

void
CtrlListItem::CtrlProperty::changeFont()
{
	HFONT hFont = reinterpret_cast<HFONT>(::SendMessage(m_hwndCtrl,
														WM_GETFONT,0,0));
	if (hFont == NULL)
		hFont = reinterpret_cast<HFONT>(::GetStockObject(SYSTEM_FIXED_FONT));
	LOGFONT lf;
	::GetObject(hFont,sizeof(LOGFONT), &lf);
	lf.lfWeight		= (m_fontprop.m_fface&1) != 0 ? 700 : 400;
	lf.lfItalic		= (m_fontprop.m_fface&2) != 0;
	lf.lfUnderline	= (m_fontprop.m_fface&4) != 0;
	lf.lfStrikeOut	= (m_fontprop.m_fface&8) != 0;
	if (m_fontprop.m_fname.length() == 0) {
		lstrcpyn(lf.lfFaceName,
				 m_pCtrl->getParentPage().getDlgFrame().getFontName(), 32);
	} else {
		lstrcpyn(lf.lfFaceName, m_fontprop.m_fname, 32);
	}
	if (m_fontprop.m_hfont != NULL) ::DeleteObject(m_fontprop.m_hfont);
	m_fontprop.m_hfont = ::CreateFontIndirect(&lf);
	if (m_fontprop.m_hfont != NULL)
		::SendMessage(m_hwndCtrl, WM_SETFONT, (WPARAM)m_fontprop.m_hfont, TRUE);
	m_fontprop.m_bchanged = FALSE;
}

LRESULT CALLBACK
CtrlListItemProc(HWND hCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CtrlListItem::CtrlProperty*
		pCProp = (CtrlListItem::CtrlProperty*)::GetWindowLong(hCtrl, GWL_USERDATA);
	return pCProp->m_pCtrl->dispatchRawMsg(pCProp, hCtrl, uMsg, wParam, lParam);
}

BOOL
CtrlListItem::CtrlProperty::init(HWND hDlg)
{
	m_hwndCtrl = ::GetDlgItem(hDlg, m_id);
	if (m_hwndCtrl == NULL) return FALSE;
	m_pfnDefCallback = (WNDPROC)::SetWindowLong(m_hwndCtrl,
												GWL_WNDPROC,
												(LONG)CtrlListItemProc);
	::SetWindowLong(m_hwndCtrl, GWL_USERDATA, (LONG)this);
	return TRUE;
}


BOOL
TreeItemData::initItem(HWND hCtrl, HTREEITEM hParent)
{
	TV_INSERTSTRUCT	tvis;
	tvis.hParent		= hParent;
	tvis.hInsertAfter	= TVI_LAST;
	tvis.item.mask		= TVIF_PARAM|TVIF_TEXT;
	tvis.item.lParam	= (LPARAM)this;
	tvis.item.pszText	= m_text.getBufPtr();
	m_hItem = TreeView_InsertItem(hCtrl,&tvis);
	if (m_hItem == NULL) return FALSE;
	this->initSequentialGet();
	TreeItemData* tid;
	while ((tid = this->getNextItem()) != NULL)
		if (!tid->initItem(hCtrl,m_hItem)) return FALSE;
	return TRUE;
}

BOOL
TreeItemData::dumpData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	StringBuffer keybuf(32);
	keybuf.append(m_name).append((TCHAR)':');
	int	len = keybuf.length(),
		num = this->initSequentialGet();
	keybuf.append(GetString(STR_DLGDATA_TEXT));
	ddfile.write(m_text,keybuf);
	keybuf.setlength(len);
	keybuf.append(GetString(STR_DLGDATA_ITEMNUM));
	ddfile.write(num,keybuf);
	keybuf.setlength(len);
	keybuf.append(GetString(STR_DLGDATA_CHILD)).append((TCHAR)'_');
	len = keybuf.length();
	TreeItemData* tid;
	for (int i = 0; i < num; i++) {
		if ((tid = this->getNextItem()) == NULL) continue;
		keybuf.setlength(len);
		keybuf.append(i);
		ddfile.write(tid->getName(),keybuf);
		tid->dumpData(ddfile);
	}
	return TRUE;
}

BOOL
TreeItemData::loadData(DlgDataFile& ddfile, TreeHashTable& hash)
{
	if (!ddfile.isValid()) return FALSE;
	StringBuffer keybuf(32);
	keybuf.append(m_name).append((TCHAR)':');
	int len = keybuf.length();
	keybuf.append(GetString(STR_DLGDATA_TEXT));
	ddfile.read(m_text,keybuf);
	keybuf.setlength(len);
	keybuf.append(GetString(STR_DLGDATA_ITEMNUM));
	int num;
	ddfile.read(&num,keybuf);
	keybuf.setlength(len);
	keybuf.append(GetString(STR_DLGDATA_CHILD)).append((TCHAR)'_');
	len = keybuf.length();
	TreeItemData* tid;
	StringBuffer namebuf(32);
	for (int i = 0; i < num; i++) {
		keybuf.setlength(len);
		keybuf.append(i);
		ddfile.read(namebuf,keybuf);
		if (namebuf.length() <= 0) continue;
		tid = new TreeItemData(namebuf,nullStr);
		if (tid->loadData(ddfile,hash)) {
			this->addItem(tid);
			hash.setValue(tid->getName(),tid);
		}
	}
	return TRUE;
}


//	class CtrlListItem, extends LinkListItem
CtrlListItem::CtrlListItem(
	CTRL_ID type,
	const StringBuffer& name,
	const StringBuffer& text)
	:	m_dwMagic(CTRL_PTR_MAGIC),
		m_type((DWORD)type),
		m_name(name),
		m_text(text),
		m_cnum(0),
		m_pcp(NULL),
		m_item(NULL),
		m_bInitDone(FALSE),
		m_bEnable(TRUE)
{
	m_x = m_y = m_cx = m_cy = m_width = m_height = 0;
	::FillMemory(&m_notify,sizeof(WORD)*4,0xFF);
}

CtrlListItem::~CtrlListItem()
{
	// nothing to do.
}

//	コントロールクラスの生成
CtrlListItem*
CtrlListItem::createCtrl(
	int ctrltype,
	const StringBuffer& name,
	const StringBuffer& text)
{
	CtrlListItem* nc = NULL;
	try {
		switch (ctrltype) {
		case CTRLID_BUTTON:
			nc = new BtnCtrl(name,text,CTRLID_BUTTON);
			break;
		case CTRLID_DEFBUTTON:
			nc = new BtnCtrl(name,text,CTRLID_DEFBUTTON);
			break;
		case CTRLID_CHECK:
			nc = new CheckCtrl(name,text);
			break;
		case CTRLID_RDBTN:
			nc = new CheckCtrl(name,text,CTRLID_RDBTN);
			break;
		case CTRLID_RADIO:
			nc = new RadioCtrl(name,text);
			break;
		case CTRLID_TEXT:
			nc = new TextCtrl(name,text);
			break;
		case CTRLID_EDIT:
		case CTRLID_MLEDIT:
		case CTRLID_PWDEDIT:
			nc = new EditCtrl(name,text,static_cast<CTRL_ID>(ctrltype));
			break;
		case CTRLID_LIST:
			nc = new ListCtrl(name,text);
			break;
		case CTRLID_COMBO:
		case CTRLID_CMBLIST:
			nc = new ComboCtrl(name,text,static_cast<CTRL_ID>(ctrltype));
			break;
		case CTRLID_REFFILEBUTTON:
		case CTRLID_REFDIRBUTTON:
		case CTRLID_REFCOLORBUTTON:
		case CTRLID_REFFONTBUTTON:
			nc = new RefBtnCtrl(name,text,static_cast<CTRL_ID>(ctrltype));
			break;
		case CTRLID_SPIN:
			nc = new SpinCtrl(name,text);
			break;
		case CTRLID_TRACK:
			nc = new TrackCtrl(name,text);
			break;
		case CTRLID_HLINE:
		case CTRLID_VLINE:
			nc = new LineCtrl(name,text,static_cast<CTRL_ID>(ctrltype));
			break;
		case CTRLID_LVIEW:
			nc = new LViewCtrl(name,text);
			break;
		case CTRLID_CHKLIST:
			nc = new ChkListCtrl(name,text);
			break;
		case CTRLID_FRAME:
		case CTRLID_GROUP:
			nc = new FrameCtrl(name,text,static_cast<CTRL_ID>(ctrltype));
			break;
		case CTRLID_TAB:
			nc = new TabCtrl(name,text);
			break;
		case CTRLID_TREE:
			nc = new TreeCtrl(name,text);
			break;
		case CTRLID_OKCANCEL:
			nc = new OkCancelCtrl();
			break;
		case CTRLID_NEWCOLUMN:
		case CTRLID_NEWPAGE:
			nc = new PageFormatCtrl(text,static_cast<CTRL_ID>(ctrltype));
			break;
		}
	} catch (...) {
		return NULL;
	}
	return nc;
}

POINT
CtrlListItem::getRect() const
{
	POINT pt;
	pt.x = (int)MAKELONG(m_x,m_cx);
	pt.y = (int)MAKELONG(m_y,m_cy);
	return pt;
}

void
CtrlListItem::setRect(WORD x, WORD y, WORD cx, WORD cy)
{
	m_x = x;
	m_y = y;
	m_cx = cx;
	m_cy = cy;
}

WORD
CtrlListItem::onWmCommand(WPARAM wParam, LPARAM lParam)
{
	return m_bInitDone ? this->onCommand(wParam,lParam) : 0xFFFF;
}

WORD
CtrlListItem::onWmNotify(WPARAM wParam, LPARAM lParam)
{
	return m_bInitDone ? this->onNotify(wParam,lParam) : 0xFFFF;
}

HBRUSH
CtrlListItem::onWmCtlColor(WPARAM wParam, LPARAM lParam)
{
	return m_bInitDone ? this->onCtlColor((HDC)wParam) : NULL;
}

WORD
CtrlListItem::onWmImeNotify(WPARAM wParam, LPARAM lParam)
{
	return m_bInitDone ? this->onImeNotify(wParam,lParam) : 0xFFFF;
}

BOOL
CtrlListItem::onInitCtrl(HWND hDlg)
{
	if (!this->initCtrl(hDlg) || !this->sendData()) return FALSE;
	m_bInitDone = TRUE;
	return TRUE;
}

BOOL
CtrlListItem::onUninitCtrl()
{
	if (!this->receiveData() || !this->uninitCtrl()) return FALSE;
	m_bInitDone = FALSE;
	return TRUE;
}

int
CtrlListItem::getDataSize()
{
	return 0;
}

WORD
CtrlListItem::getWidth()
{
	return m_width;
}

WORD
CtrlListItem::getHeight()
{
	return NARROWHEIGHT * m_height;
}

BOOL
CtrlListItem::createCtrlTemplate(CtrlTemplateArgs &cta)
{
	return FALSE;
}

BOOL
CtrlListItem::initCtrl(HWND hDlg)
{
	return hDlg != NULL;
}

BOOL
CtrlListItem::uninitCtrl()
{
	return TRUE;	//	this should be TRUE.
}

BOOL
CtrlListItem::enableCtrl(BOOL bEnable, BOOL bChange)
{
	if (bChange) m_bEnable = bEnable;
	return TRUE;
}

BOOL
CtrlListItem::showCtrl(BOOL bVisible)
{
	return TRUE;
}

BOOL
CtrlListItem::sendData()
{
	return TRUE;	//	::IsWindow(m_hwndDlg);
}

BOOL
CtrlListItem::receiveData()
{
	return TRUE;	//	::IsWindow(m_hwndDlg);
}

BOOL
CtrlListItem::onSetState(CmdLineParser&)
{
	return FALSE;
}

BOOL
CtrlListItem::onSetString(const StringBuffer&)
{
	return FALSE;
}

BOOL
CtrlListItem::onSetCtrlFont(CmdLineParser&)
{
	return FALSE;
}

BOOL
CtrlListItem::onSetItem(CmdLineParser&, const StringBuffer&)
{
	return FALSE;
}

BOOL
CtrlListItem::onInsertItem(CmdLineParser&, const StringBuffer&)
{
	return FALSE;
}

BOOL
CtrlListItem::onDeleteItem(const StringBuffer&)
{
	return FALSE;
}

BOOL
CtrlListItem::onResetList()
{
	return FALSE;
}

StringBuffer
CtrlListItem::onGetState()
{
	return nullStr;
}

StringBuffer
CtrlListItem::onGetString()
{
	return nullStr;
}

StringBuffer
CtrlListItem::onGetCtrlFont()
{
	return nullStr;
}

StringBuffer
CtrlListItem::onGetItem(const StringBuffer&)
{
	return nullStr;
}

BOOL
CtrlListItem::onSetWidth(WORD width)
{
	m_width = width;
	return TRUE;
}

BOOL
CtrlListItem::onSetHeight(WORD height)
{
	m_height = height;
	return TRUE;
}

BOOL
CtrlListItem::onSetNotify(CmdLineParser& argv)
{
	argv.initSequentialGet();
	LPCSTR av;
	int	num = 0;
	while ((av = argv.getNextArgv()) != NULL && num < 4)
		m_notify[num++] = (WORD)ival(av);
	return TRUE;
}

BOOL
CtrlListItem::onSetImeState(int)
{
	return FALSE;
}

int
CtrlListItem::onGetImeState()
{
	return -1;
}

BOOL
CtrlListItem::isCommand(WORD)
{
	return FALSE;
}

LRESULT
CtrlListItem::dispatchRawMsg(
	CtrlListItem::CtrlProperty* pCProp,
	HWND hCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_GET_CTRL_PTR:
		return (LRESULT)pCProp->m_pCtrl;
	default:
		return ::CallWindowProc(pCProp->m_pfnDefCallback, hCtrl, uMsg, wParam, lParam);
	}
	return 0;
}

WORD
CtrlListItem::onCommand(WPARAM, LPARAM)
{
	return 0xFFFF;	//	indicate no effect.
}

WORD
CtrlListItem::onNotify(WPARAM, LPARAM)
{
	return 0xFFFF;	//	indicate no effect.
}

HBRUSH
CtrlListItem::onCtlColor(HDC)
{
	return 0;
}

WORD
CtrlListItem::onImeNotify(WPARAM, LPARAM)
{
	return 0xFFFF;
}

BOOL
CtrlListItem::dumpData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	ddfile.write(m_type,GetString(STR_DLGDATA_TYPE));
	ddfile.write(m_name,GetString(STR_DLGDATA_NAME));
	ddfile.write(m_text,GetString(STR_DLGDATA_TEXT));
	ddfile.write(m_cnum,GetString(STR_DLGDATA_INTERNALCTRLNUM));
	ddfile.write(m_bEnable,GetString(STR_DLGDATA_ENABLE));
	StringBuffer buf(GetString(STR_DLGDATA_NOTIFY),-1,2);
	buf.append((TCHAR)'_').append((TCHAR)'*');
	for (int i = 0; i < 4; i++) {
		buf.setcharAt(-1,(TCHAR)('0'+i));
		ddfile.write(m_notify[i],buf);
	}
	ddfile.write(m_width,GetString(STR_DLGDATA_WIDTH));
	ddfile.write(m_height,GetString(STR_DLGDATA_HEIGHT));
	if (m_cnum > 0 && m_pcp != NULL) {
		StringBuffer keybuf(GetString(STR_DLGDATA_CTRL),-1,32);
		keybuf.append((TCHAR)'_');
		int	len, keylen = keybuf.length();
		for (int i = 0; i < m_cnum; i++) {
			keybuf.setlength(keylen);
			keybuf.append(i).append((TCHAR)':');
			len = keybuf.length();
			keybuf.append(GetString(STR_DLGDATA_ID));
			ddfile.write(m_pcp[i].m_id,keybuf);
			keybuf.setlength(len);
			keybuf.append(GetString(STR_DLGDATA_TEXT));
			ddfile.write(m_pcp[i].m_text != NULL ? m_pcp[i].m_text : nullStr,
						keybuf);
		}
	}
	return TRUE;
}

BOOL
CtrlListItem::loadData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	ddfile.read(&m_cnum,GetString(STR_DLGDATA_INTERNALCTRLNUM));
	ddfile.read(&m_bEnable,GetString(STR_DLGDATA_ENABLE));
	StringBuffer buf(GetString(STR_DLGDATA_NOTIFY),-1,2);
	buf.append((TCHAR)'_').append((TCHAR)'*');
	for (int i = 0; i < 4; i++) {
		buf.setcharAt(-1,(TCHAR)('0'+i));
		ddfile.read(&m_notify[i],buf);
	}
	ddfile.read(&m_width,GetString(STR_DLGDATA_WIDTH));
	ddfile.read(&m_height,GetString(STR_DLGDATA_HEIGHT));
	if (m_cnum > 0 && m_pcp != NULL) {
		StringBuffer keybuf(GetString(STR_DLGDATA_CTRL),-1,32);
		keybuf.append((TCHAR)'_');
		int	len, keylen = keybuf.length();
		for (int i = 0; i < m_cnum; i++) {
			keybuf.setlength(keylen);
			keybuf.append(i).append((TCHAR)':');
			len = keybuf.length();
			keybuf.append(GetString(STR_DLGDATA_ID));
			ddfile.read(&m_pcp[i].m_id,keybuf);
			keybuf.setlength(len);
			keybuf.append(GetString(STR_DLGDATA_TEXT));
			ddfile.read(m_pcp[i].m_text,keybuf);
		}
	}
	return TRUE;
}

//	ctrl with one control
SimpleCtrl::SimpleCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	:	CtrlListItem(type,name,text)
{
	m_cnum = 1;
	m_cy = 1;
	m_pcp = new CtrlListItem::CtrlProperty(this);
	m_pcp->m_text = text;
}

SimpleCtrl::~SimpleCtrl()
{
	delete m_pcp;
}

BOOL
SimpleCtrl::setFont(
	const StringBuffer& fface,
	const StringBuffer& color,
	const StringBuffer& fname)
{
	BYTE bfface = 0;
	if (fface.length() > 0) {
		LPCSTR pstr = fface;
		while (*pstr != '\0') {
			switch (*pstr++) {
			case 'b':
				bfface |= 1;
				break;
			case 'i':
				bfface |= 2;
				break;
			case 'u':
				bfface |= 4;
				break;
			case 's':
				bfface |= 8;
				break;
			}
		}
	}
	COLORREF cref = 0;
	SessionInstance* pSI = m_pDlgPage->getDlgFrame().getSessionInstance();
	if (pSI != NULL) {
		cref = pSI->getColorTable().getColorRef(color);
	}
	if (m_pcp->m_fontprop.m_fface != bfface	||
		m_pcp->m_fontprop.m_color != cref ||
		m_pcp->m_fontprop.m_fname.compareTo(fname) != 0) {
		m_pcp->m_fontprop.m_fface = bfface;
		m_pcp->m_fontprop.m_color = cref;
		m_pcp->m_fontprop.m_fname = fname;
		m_pcp->m_fontprop.m_bchanged = TRUE;
	}
	return TRUE;
}

int
SimpleCtrl::getDataSize()
{
	return	sizeof(DLGITEMTEMPLATE) + 
//			sizeof(WORD) * (m_pcp->m_bufsize + m_pcp->m_text.length() + 6);
			((m_pcp->m_bufsize + m_pcp->m_text.length() + 6) << 1);
}

BOOL
SimpleCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y = UHEIGHT * m_y + UHEIGHT / 4;
	cta.m_cx = UWIDTH * m_cx - OWIDTH;
	cta.m_cy = UHEIGHT * m_cy - UHEIGHT / 2;
	m_pcp->setCtrlTemplate(cta);
	return TRUE;
}

BOOL
SimpleCtrl::initCtrl(HWND hDlg)
{
	return CtrlListItem::initCtrl(hDlg) && m_pcp->init(hDlg);
}

BOOL
SimpleCtrl::uninitCtrl()
{
	m_pcp->m_hwndCtrl = NULL;
	return CtrlListItem::uninitCtrl();
}

BOOL
SimpleCtrl::enableCtrl(BOOL bEnable, BOOL bChange)
{
	if (bChange) m_bEnable = bEnable;
	if (m_pcp->m_hwndCtrl != NULL) {
		::EnableWindow(m_pcp->m_hwndCtrl, m_bEnable && bEnable);
	}
	return TRUE;
}

BOOL
SimpleCtrl::showCtrl(BOOL bVisible)
{
	if (m_pcp->m_hwndCtrl != NULL) {
		::ShowWindow(m_pcp->m_hwndCtrl,bVisible ? SW_SHOW : SW_HIDE);
	}
	return TRUE;
}

BOOL
SimpleCtrl::sendData()
{
	//	on default, set text of first control
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	::SetWindowText(m_pcp->m_hwndCtrl, m_pcp->m_text);
	if (m_pcp->m_fontprop.m_bchanged) m_pcp->changeFont();
	return TRUE;
}

BOOL
SimpleCtrl::receiveData()
{
	//	on default, get text of first control
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	int	len = ::SendMessage(m_pcp->m_hwndCtrl, WM_GETTEXTLENGTH, 0, 0);
	::GetWindowText(m_pcp->m_hwndCtrl, m_pcp->m_text.getBufPtr(), len + 1);
	m_pcp->m_text.resize(len);
	return (m_pcp->m_text.length() == len);
}

BOOL
SimpleCtrl::onSetString(const StringBuffer& newstr)
{
	m_pcp->m_text = newstr;
	this->sendData();
	return TRUE;
}

BOOL
SimpleCtrl::onSetCtrlFont(CmdLineParser& argv)
{
	if (argv.itemNum() < 1) return FALSE;
	argv.initSequentialGet();
	const StringBuffer& fface = argv.getNextArgvStr();
	const StringBuffer& color = argv.getNextArgvStr();
	const StringBuffer& fname = argv.getNextArgvStr();
	this->setFont(fface, color, fname);
	this->sendData();
	return TRUE;
}

StringBuffer
SimpleCtrl::onGetString()
{
	this->receiveData();
	return m_pcp->m_text;
}

StringBuffer
SimpleCtrl::onGetCtrlFont()
{
	StringBuffer ret(60);
	BYTE fface = m_pcp->m_fontprop.m_fface;
	if ((fface&1) != 0) ret.append((TCHAR)'b');
	if ((fface&2) != 0) ret.append((TCHAR)'i');
	if ((fface&4) != 0) ret.append((TCHAR)'u');
	if ((fface&8) != 0) ret.append((TCHAR)'s');
	ret.append((TCHAR)',');
	ret.append(ColorTable::colorRefToColorStr(m_pcp->m_fontprop.m_color));
	if (m_pcp->m_fontprop.m_fname.length() > 0) {
		ret.append((TCHAR)',').append(m_pcp->m_fontprop.m_fname);
	}
	return ret;
}

HBRUSH
SimpleCtrl::onCtlColor(HDC hDc)
{
	::SetTextColor(hDc,m_pcp->m_fontprop.m_color);
	::SetBkMode(hDc,TRANSPARENT);
	return ::GetSysColorBrush(COLOR_MENU);
}

BOOL
SimpleCtrl::isCommand(WORD id)
{
	return (m_pcp->m_id == id);
}

//	following are the actual control objects


//	class BtnCtrl, extends SimpleCtrl
BtnCtrl::BtnCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: SimpleCtrl(name,text,type)
{
	m_pcp->m_style		= BS_PUSHBUTTON|
							WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= 0x0;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x80;

	m_pcp->m_style |= (m_type == (DWORD)CTRLID_DEFBUTTON) ?
						BS_DEFPUSHBUTTON : 0;
}

WORD
BtnCtrl::getHeight()
{
	if (m_height > 0) return NORMALHEIGHT * m_height;
	return NORMALHEIGHT;
}

WORD
BtnCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) != BN_CLICKED) return 0xFFFF;
	return m_notify[0];
}

//	class TextCtrl, extends NormalCtrl
TextCtrl::TextCtrl(
	const StringBuffer& name,
	const StringBuffer& text)
	: SimpleCtrl(name,text,CTRLID_TEXT)
{
	m_pcp->m_style		= SS_LEFTNOWORDWRAP|SS_NOTIFY|
							WS_CHILD/*|WS_TABSTOP*/|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= 0x0;
	m_pcp->m_id			= 0;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x82;
}

BOOL
TextCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x	= UWIDTH * m_x + OWIDTH / 2;
	cta.m_y	= UHEIGHT * m_y + UHEIGHT / 2 + 1;
	cta.m_cx = UWIDTH * m_cx - OWIDTH;
	cta.m_cy = UHEIGHT * m_cy - UHEIGHT;
	m_pcp->setCtrlTemplate(cta);
	return TRUE;
}

EditCtrl::EditCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: SimpleCtrl(name,text,type),
	  m_imestate(0L)
{
	m_pcp->m_style		= ES_AUTOHSCROLL|
							WS_BORDER|WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	switch (type) {
	case CTRLID_MLEDIT:
		m_pcp->m_style |= ES_AUTOVSCROLL|ES_MULTILINE|ES_WANTRETURN;
		break;
	case CTRLID_PWDEDIT:
		m_pcp->m_style |= ES_PASSWORD;
		break;
	}
	m_pcp->m_exstyle	= 0x0;
	m_pcp->m_id			= 0;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x81;
	m_pcp->m_text.replaceStr("\n","\r\n",-1);
}

WORD
EditCtrl::getHeight()
{
	if (m_height > 0) return STRICTHEIGHT * m_height + 2;
	return NORMALHEIGHT;
}

BOOL
EditCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y = UHEIGHT * m_y + UHEIGHT / 2;
	cta.m_cx = UWIDTH * m_cx - OWIDTH;
	cta.m_cy = UHEIGHT * m_cy - UHEIGHT;
	m_pcp->setCtrlTemplate(cta);
	return TRUE;
}

BOOL
EditCtrl::sendData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	::SetWindowText(m_pcp->m_hwndCtrl,
					StringBuffer(m_pcp->m_text).replaceStr("\n", "\r\n",-1));
	if (m_pcp->m_fontprop.m_bchanged) m_pcp->changeFont();
	return TRUE;
}

BOOL
EditCtrl::receiveData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	int	len = ::SendMessage(m_pcp->m_hwndCtrl, WM_GETTEXTLENGTH, 0, 0);
	m_pcp->m_text.resize(len + 1);
	::GetWindowText(m_pcp->m_hwndCtrl,m_pcp->m_text.getBufPtr(), len + 1);
	m_pcp->m_text.replaceStr("\r\n","\n",-1);
	return TRUE;
}

BOOL
EditCtrl::onSetState(CmdLineParser& state)
{
	return this->onSetImeState(ival(state.getArgv(0)));
}

StringBuffer
EditCtrl::onGetState()
{
	this->receiveData();
	LPCSTR av = m_pcp->m_text;
	while (IsCharSpace(*av)) av++;
	if (*av == '-' || IsCharDigit(*av))
		return StringBuffer(8).append(ival(av));
	else
		return StringBuffer(8).append((TCHAR)'0');
}

BOOL
EditCtrl::onSetImeState(int state)
{
	m_imestate = state;
	return TRUE;
}

int
EditCtrl::onGetImeState()
{
	return m_imestate;
}

WORD
EditCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam)) {
	case EN_CHANGE:
		return m_notify[0];
	case EN_SETFOCUS:
		if (m_imestate > 0) {
			HIMC hImc = ::ImmGetContext((HWND)lParam);
			switch (m_imestate) {
			case 1:
				if (!::ImmGetOpenStatus(hImc)) ::ImmSetOpenStatus(hImc,TRUE);
				break;
			default:
				if (::ImmGetOpenStatus(hImc)) ::ImmSetOpenStatus(hImc,FALSE);
				break;
			}
			::ImmReleaseContext((HWND)lParam,hImc);
		}
		::SendMessage((HWND)lParam,EM_SETSEL,0,-1);
		break;
	}
	return 0xFFFF;
}

WORD
EditCtrl::onImeNotify(WPARAM wParam, LPARAM lParam)
{
	return 0xFFFF;
}

BOOL
EditCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_imestate,GetString(STR_DLGDATA_IMESTATE));
	return TRUE;
}

BOOL
EditCtrl::loadData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(&m_imestate,GetString(STR_DLGDATA_IMESTATE));
	return TRUE;
}

//	class CheckCtrl, extends SimpleCtrl
CheckCtrl::CheckCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: BtnCtrl(name,text,type)
{
	m_pcp->m_style = (type == CTRLID_CHECK ? BS_AUTOCHECKBOX : BS_AUTORADIOBUTTON) |
						WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_GROUP;

	m_state = FALSE;
}

BOOL
CheckCtrl::sendData()
{
	if (!BtnCtrl::sendData()) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL)
		::SendMessage(m_pcp->m_hwndCtrl, BM_SETCHECK,
					m_state ? BST_CHECKED : BST_UNCHECKED, 0L);
	return TRUE;
}

BOOL
CheckCtrl::receiveData()
{
	if (!BtnCtrl::receiveData()) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL)
		m_state = (::SendMessage(m_pcp->m_hwndCtrl, BM_GETCHECK, 0, 0L)
					== BST_CHECKED);
	return TRUE;
}

BOOL
CheckCtrl::onSetState(CmdLineParser& state)
{
	m_state = (ival(state.getArgv(0)) != 0);
	this->sendData();
	return TRUE;
}

static const StringBuffer oneStr("1"), zeroStr("0");

StringBuffer
CheckCtrl::onGetState()
{
	this->receiveData();
	return m_state ? oneStr : zeroStr;
}

BOOL
CheckCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!BtnCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_state,GetString(STR_DLGDATA_STATE));
	return TRUE;
}

BOOL
CheckCtrl::loadData(DlgDataFile& ddfile)
{
	if (!BtnCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(&m_state,GetString(STR_DLGDATA_STATE));
	return TRUE;
}

//	class TrackCtrl, extends NormalCtrl
TrackCtrl::TrackCtrl(
	const StringBuffer& name,
	const StringBuffer& text)
	: SimpleCtrl(name,text,CTRLID_TRACK),
	  m_wPrevNotify(0xFFFF)
{
	m_min = m_val = (WORD)ival(text);
	LPCSTR ptr = text;
	if ((ptr = lstrchr(ptr,':')) != NULL) {
		m_max = (WORD)ival(++ptr);
		if ((ptr = lstrchr(ptr,':')) != NULL) m_tic = (WORD)ival(++ptr);
	}
	if (m_max < m_min) {
		WORD tmp = m_min;
		m_min = m_max;
		m_max = tmp;
	} else if (m_max == m_min) m_max = m_min + 1;	//	minimun
	if (m_tic <= 0 || m_tic >= (m_max-m_min)) m_tic = 1;

	m_pcp->m_style		= TBS_HORZ|TBS_AUTOTICKS|TBS_BOTTOM|
							WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_classname	= TRACKBAR_CLASS;
	m_pcp->m_bufsize	= lstrlen(m_pcp->m_classname) + 1;
	m_pcp->m_text.reset();
	m_pcp->m_text.append((DWORD)m_val);
}

WORD
TrackCtrl::getHeight()
{
	return NORMALHEIGHT;
}

BOOL
TrackCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x  = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y  = UHEIGHT * m_y;
	cta.m_cx = UWIDTH * m_cx - OWIDTH;
	cta.m_cy = UHEIGHT * NARROWHEIGHT;
	m_pcp->setCtrlTemplate(cta);
	return TRUE;
}

BOOL
TrackCtrl::initCtrl(HWND hDlg)
{
	if (!SimpleCtrl::initCtrl(hDlg)) return FALSE;
	::SendMessage(m_pcp->m_hwndCtrl, TBM_SETRANGE,
					(WPARAM)FALSE, MAKELPARAM(m_min,m_max));
	::SendMessage(m_pcp->m_hwndCtrl, TBM_SETTICFREQ, (WPARAM)m_tic, 0L);
	::SendMessage(m_pcp->m_hwndCtrl, TBM_SETPOS, TRUE, (LPARAM)m_val);
	return TRUE;
}

BOOL
TrackCtrl::sendData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	::SendMessage(m_pcp->m_hwndCtrl, TBM_SETPOS, TRUE, (LPARAM)m_val);
//	if (m_pcp->m_fontprop.m_bchanged) m_pcp->changeFont();
	return TRUE;
}

BOOL
TrackCtrl::receiveData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	m_val = (WORD)::SendMessage(m_pcp->m_hwndCtrl, TBM_GETPOS, 0, 0);
	return TRUE;
}

BOOL
TrackCtrl::onSetState(CmdLineParser& state)
{
	WORD val = (WORD)ival(state.getArgv(0));
	if (m_val >= m_min && m_val <= m_max) {
		m_val = val;
		this->sendData();
		return TRUE;
	}
	return FALSE;
}

StringBuffer
TrackCtrl::onGetState()
{
	this->receiveData();
	m_pcp->m_text.reset();
	m_pcp->m_text.append((DWORD)m_val);
	return m_pcp->m_text;
}

WORD
TrackCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_pcp->m_hwndCtrl != NULL	&&
		m_pcp->m_hwndCtrl == (HWND)lParam) {
		if (LOWORD(wParam) != TB_ENDTRACK) {
			m_wPrevNotify = LOWORD(wParam);
		} else {
			switch (m_wPrevNotify) {
			case 0xFFFF:
				break;
			case TB_THUMBPOSITION:
				m_wPrevNotify = 0xFFFF;
				return (m_notify[1] != 0xFFFF) ? m_notify[1] : m_notify[0];
			default:
				m_wPrevNotify = 0xFFFF;
				return m_notify[0];
			}
		}
	}
	return 0xFFFF;
}

BOOL
TrackCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_min, GetString(STR_DLGDATA_MIN));
	ddfile.write(m_max, GetString(STR_DLGDATA_MAX));
	ddfile.write(m_val, GetString(STR_DLGDATA_VAL));
	ddfile.write(m_tic, GetString(STR_DLGDATA_TIC));
	return TRUE;
}

BOOL
TrackCtrl::loadData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(&m_min, GetString(STR_DLGDATA_MIN));
	ddfile.read(&m_max, GetString(STR_DLGDATA_MAX));
	ddfile.read(&m_val, GetString(STR_DLGDATA_VAL));
	ddfile.read(&m_tic, GetString(STR_DLGDATA_TIC));
	return TRUE;
}

LineCtrl::LineCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: SimpleCtrl(name,text,type)
{
	m_pcp->m_style		= SS_SUNKEN|SS_GRAYRECT|
							WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x82;
}

WORD
LineCtrl::getHeight()
{
	return (m_type == CTRLID_HLINE) ? STRICTHEIGHT : (NARROWHEIGHT*m_height);
}

WORD
LineCtrl::getWidth()
{
	return (m_type == CTRLID_VLINE) ? 1 : m_width;
}

BOOL
LineCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x  = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y  = UHEIGHT * m_y + UHEIGHT / 2;
	if (m_type == CTRLID_HLINE) {
		cta.m_cx = UWIDTH * m_cx - OWIDTH;
		cta.m_cy = 1;
	} else {
		cta.m_cx = 1;
		cta.m_cy = UHEIGHT * m_cy - UHEIGHT;
	}
	m_pcp->setCtrlTemplate(cta);
	return TRUE;
}

HasListCtrl::HasListCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: SimpleCtrl(name,text,type)
{
	m_cy = 0;
	m_item = new LinkList<ItemData>;
	m_state = 0;
}

HasListCtrl::~HasListCtrl()
{
//	delete m_item;
}

WORD
HasListCtrl::getHeight()
{
	WORD h = (WORD)m_item->itemNum();
	if (m_height > 0) h = m_height;
	else if (h == 0) h = 5;
	return STRICTHEIGHT * h + 2;
}

BOOL
HasListCtrl::onSetState(CmdLineParser& state)
{
	m_state = ival(state.getArgv(0));
	if (m_state < 0 || m_state > m_item->itemNum()) m_state = 0;
	this->sendData();
	return TRUE;
}

int
HasListCtrl::getItemData(const StringBuffer& sind)
{
	int	ind = m_state - 1,
		num = m_item->itemNum();
	if (sind.length() > 0) {
		int	tmp = ival(sind) - 1;
		switch (tmp) {
		case -2:
			ind = num - 1;
			break;
		case -1:
			ind = m_state - 1;
			break;
		default:
			if (tmp >= 0 && tmp < num) ind = tmp;
		}
	}
	if (ind < 0 || ind >= num) return -1;
	return ind;
}

BOOL
HasListCtrl::onSetItem(CmdLineParser& text, const StringBuffer& pos)
{
	this->receiveData();
	int	ind = this->getItemData(pos);
	if (ind < 0) return FALSE;
	ItemData* id = static_cast<ItemData*>(m_item->getItemByIndex(ind));
	if (id == NULL) return FALSE;
	id->getText() = text.getRawData();
	return ind + 1;
}

BOOL
HasListCtrl::onInsertItem(CmdLineParser& text, const StringBuffer& pos)
{
	this->receiveData();
	int	ind = -1,
		num = m_item->itemNum();
	if (pos.length() > 0) {
		int	tmp = ival(pos) - 1;
		if (tmp >= 0 && tmp < num) ind = tmp;
	}
	m_item->addItem(new ItemData(text.getRawData()), ind);
	if (ind < 0) ind = num;
	if (ind < m_state) m_state++;
	return ind + 1;
}

BOOL
HasListCtrl::onDeleteItem(const StringBuffer& pos)
{
	this->receiveData();
	int	ind = this->getItemData(pos);
	if (ind < 0) return FALSE;
	m_item->delItemByIndex(ind);
	if (ind < m_state - 1) m_state--;
	else if (ind == m_state - 1) m_state = 0;
	return ind + 1;
}

BOOL
HasListCtrl::onResetList()
{
	m_item = new LinkList<ItemData>;
	m_state = 0;
	return TRUE;
}

StringBuffer
HasListCtrl::onGetState()
{
	this->receiveData();
	return StringBuffer(8).append(m_state);
}

StringBuffer
HasListCtrl::onGetItem(const StringBuffer& pos)
{
	this->receiveData();
	int	ind = this->getItemData(pos);
	if (ind < 0) return nullStr;
	ItemData* id = static_cast<ItemData*>(m_item->getItemByIndex(ind));
	if (id == NULL) return nullStr;
	else return id->getText();
}

BOOL
HasListCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_state,GetString(STR_DLGDATA_STATE));
	ItemData* id;
	StringBuffer buf(GetString(STR_DLGDATA_ITEM),-1,32);
	buf.append((TCHAR)'_');
	int	len = buf.length(), num = m_item->initSequentialGet();
	for (int i = 0; i < num; i++) {
		if ((id = static_cast<ItemData*>(m_item->getNextItem())) == NULL)
			continue;
		buf.setlength(len);
		buf.append(i);
		ddfile.write(id->getText(),buf);
	}
	ddfile.write(num,GetString(STR_DLGDATA_ITEMNUM));
	return TRUE;
}

BOOL
HasListCtrl::loadData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(&m_state,GetString(STR_DLGDATA_STATE));
	int	num;
	ddfile.read(&num,GetString(STR_DLGDATA_ITEMNUM));
	ItemData* id;
	StringBuffer buf(GetString(STR_DLGDATA_ITEM),-1,32);
	buf.append((TCHAR)'_');
	int	len = buf.length();
	for (int i = 0; i < num; i++) {
		try {
			id = new ItemData(nullStr);
		} catch (exception&) {
			return FALSE;
		}
		buf.setlength(len);
		buf.append(i);
		ddfile.read(id->getText(),buf);
		m_item->addItem(id);
	}
	return TRUE;
}

static LRESULT CALLBACK
ChildCtrlProc(HWND hCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ChildCtrlSubClassInfo*
		pCCSci = (ChildCtrlSubClassInfo*)::GetWindowLong(hCtrl, GWL_USERDATA);
	switch (uMsg) {
	case WM_GET_CTRL_PTR:
		return (LRESULT)pCCSci->m_pCtrl;
	case WM_DESTROY:
		delete pCCSci;
		// fall down
	default:
		return ::CallWindowProc(pCCSci->m_pfnDefCallback, hCtrl, uMsg, wParam, lParam);
	}
	return 0;
}

//	class RadioCtrl, extends SimpleCtrl, implements WithItem
RadioCtrl::RadioCtrl(
	const StringBuffer& name,
	const StringBuffer& text)
	: HasListCtrl(name,text,CTRLID_RADIO)
{
	m_pcp->m_style		= BS_GROUPBOX|BS_NOTIFY|WS_CHILD|WS_VISIBLE|WS_TABSTOP;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x80;
	m_pcp->m_text = text;
}

RadioCtrl::~RadioCtrl()
{
}

WORD
RadioCtrl::getHeight()
{
	WORD h = m_item->itemNum() + 1;
	if (m_height >= h) h = m_height;
	return h * NARROWHEIGHT + 2;
}

int
RadioCtrl::getCtrlNum() const
{
	return m_item->itemNum() + 1;
}

int
RadioCtrl::getDataSize()
{
	ItemData* id;
	m_item->initSequentialGet();
	int	size = this->SimpleCtrl::getDataSize();
	while ((id = m_item->getNextItem()) != NULL) {
		size += sizeof(DLGITEMTEMPLATE) + 
				sizeof(WORD) * (id->getText().length() + 8);
	}
	return size;
}

BOOL
RadioCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x  = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y  = UHEIGHT * m_y + UHEIGHT / 2;
	cta.m_cx = UWIDTH * m_cx - OWIDTH;
	cta.m_cy = UHEIGHT * m_cy - UHEIGHT;
	m_pcp->setCtrlTemplate(cta);
	CtrlProperty cp;
	cp.m_style		=	BS_AUTORADIOBUTTON|
						WS_GROUP|WS_CHILD|WS_TABSTOP|WS_VISIBLE;
	cp.m_bufsize	=	1;
	cp.m_classname	=	(LPSTR)0x80;
	cta.m_x	=	UWIDTH * (m_x + 1);
	cta.m_y	=	UHEIGHT * m_y;
	cta.m_cx	=	UWIDTH * (m_cx - 2);
	cta.m_cy	=	UHEIGHT * NARROWHEIGHT;
	m_item->initSequentialGet();
	ItemData* id;
	while ((id = m_item->getNextItem()) != NULL) {
		cp.m_text = id->getText();
		cta.m_y	+= UHEIGHT * NARROWHEIGHT;
		cp.setCtrlTemplate(cta);
		cp.m_style = BS_AUTORADIOBUTTON|WS_CHILD|WS_TABSTOP|WS_VISIBLE;
	}
	return TRUE;
}

BOOL
RadioCtrl::initCtrl(HWND hDlg)
{
	if (!HasListCtrl::initCtrl(hDlg)) return FALSE;
	int num = m_item->itemNum();
	for (int i = 1; i <= num; i++) {
		HWND hCtrl = ::GetDlgItem(hDlg, m_pcp->m_id + i);
		ChildCtrlSubClassInfo* pCCSci = new ChildCtrlSubClassInfo();
		pCCSci->m_pCtrl = this;
		pCCSci->m_pfnDefCallback
			= (WNDPROC)::SetWindowLong(hCtrl,
									   GWL_WNDPROC,
									   (LONG)ChildCtrlProc);
		::SetWindowLong(hCtrl, GWL_USERDATA, (LONG)pCCSci);
	}
	return TRUE;
}

BOOL
RadioCtrl::enableCtrl(BOOL bEnable, BOOL bChange)
{
	if (HasListCtrl::enableCtrl(bEnable,bChange)) {
		HWND hDlg = m_pDlgPage->gethwndPage();
		if (hDlg != NULL) {
			int num = m_item->itemNum();
			for (int i = 1; i <= num; i++)
				::EnableWindow(::GetDlgItem(hDlg, m_pcp->m_id + i),
								m_bEnable && bEnable);
		}
	}
	return TRUE;
}

BOOL
RadioCtrl::showCtrl(BOOL bVisible)
{
	if (HasListCtrl::showCtrl(bVisible)) {
		HWND hDlg = m_pDlgPage->gethwndPage();
		if (hDlg != NULL) {
			ItemData* id;
			m_item->initSequentialGet();
			for (int i = 1; (id = m_item->getNextItem()) != NULL; i++)
				::ShowWindow(::GetDlgItem(hDlg,m_pcp->m_id + i),
								bVisible ? SW_SHOW : SW_HIDE);
		}
	}
	return TRUE;
}

BOOL
RadioCtrl::sendData()
{
	BOOL bFontChanged = m_pcp->m_fontprop.m_bchanged;
	if (!HasListCtrl::sendData()) return FALSE;
	HWND hDlg = m_pDlgPage->gethwndPage(), hwndBtn;
	int num = m_item->initSequentialGet();
	ItemData* id;
	for (int i = 1; i <= num; i++) {
		id = m_item->getNextItem();
		hwndBtn = ::GetDlgItem(hDlg, m_pcp->m_id + i);
		if (bFontChanged)
			::SendMessage(hwndBtn, WM_SETFONT, (WPARAM)m_pcp->m_fontprop.m_hfont, NULL);
		::SetWindowText(hwndBtn,id->getText());
		::SendMessage(hwndBtn,BM_SETCHECK,
					(i == m_state) ? BST_CHECKED : BST_UNCHECKED, 0L);
	}
	return TRUE;
}

BOOL
RadioCtrl::receiveData()
{
	if (!HasListCtrl::receiveData()) return FALSE;
	m_state = 0;
	HWND hDlg = m_pDlgPage->gethwndPage(), hwndBtn;
	int num = m_item->initSequentialGet(), len;
	ItemData* id;
	for (int i = 1; i <= num; i++) {
		id = m_item->getNextItem();
		StringBuffer& buf = id->getText();
		hwndBtn = ::GetDlgItem(hDlg, m_pcp->m_id + i);
		len = ::SendMessage(hwndBtn, WM_GETTEXTLENGTH, 0, 0);
		buf.resize(len);
		::GetWindowText(hwndBtn, buf.getBufPtr(), len + 1);
		if (buf.length() != len) return FALSE;
		if (::SendMessage(hwndBtn, BM_GETCHECK, 0, 0L)) m_state = i;
	}
	return TRUE;
}

BOOL
RadioCtrl::onSetString(const StringBuffer& newstr)
{
	m_pcp->m_text = newstr;
	this->sendData();
	return TRUE;
}

BOOL
RadioCtrl::onSetItem(CmdLineParser& text, const StringBuffer& pos)
{
	int	ind = HasListCtrl::onSetItem(text,pos) - 1;
	if (ind < 0) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		if (id == NULL) return FALSE;
		::SetWindowText(::GetDlgItem(m_pDlgPage->gethwndPage(),
									m_pcp->m_id + ind + 1),
						id->getText());
	}
	return TRUE;
}

BOOL
RadioCtrl::onInsertItem(CmdLineParser& text, const StringBuffer& pos)
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onInsertItem(text,pos) ? TRUE : FALSE;
}

BOOL
RadioCtrl::onDeleteItem(const StringBuffer& pos)
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onDeleteItem(pos) ? TRUE : FALSE;
}

BOOL
RadioCtrl::onResetList()
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onResetList() ? TRUE : FALSE;
}

StringBuffer
RadioCtrl::onGetString()
{
	this->receiveData();
	return m_pcp->m_text;
}

BOOL
RadioCtrl::isCommand(WORD id)
{
	return (id >= m_pcp->m_id && id <= m_pcp->m_id + m_item->itemNum());
}

WORD
RadioCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == m_pcp->m_id) {
		if (HIWORD(wParam) == BN_SETFOCUS) {
			WORD id = m_pcp->m_id + ((m_state > 0) ? m_state : 1);
			::SetFocus(::GetDlgItem(m_pDlgPage->gethwndPage(), id));
		}
	} else if (HIWORD(wParam) == BN_CLICKED) {
		WORD old_state = m_state;
		this->receiveData();
		if (old_state != m_state || m_state != 0) return m_notify[0];
	}
	return 0xFFFF;
}

//	class ListCtrl, extends SimpleCtrl
ListCtrl::ListCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: HasListCtrl(name,text,type)
{
	m_pcp->m_style		= LBS_NOINTEGRALHEIGHT|LBS_NOTIFY|
		WS_BORDER|WS_VSCROLL|WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= 0x0;
	m_pcp->m_id			= 0;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x83;

	m_msg_setstr	= LB_INSERTSTRING;
	m_msg_getstr	= LB_GETTEXT;
	m_msg_delstr	= LB_DELETESTRING;
	m_msg_delall	= LB_RESETCONTENT;
	m_msg_setsel	= LB_SETCURSEL;
	m_msg_getsel	= LB_GETCURSEL;
	m_msg_err		= (UINT)LB_ERR;
}

BOOL
ListCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x  = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y  = UHEIGHT * m_y + UHEIGHT / 2;
	cta.m_cx = UWIDTH * m_cx - OWIDTH;
	cta.m_cy = UHEIGHT * m_cy - 2 * UHEIGHT + UHEIGHT / 2;
	m_pcp->setCtrlTemplate(cta);
	return TRUE;
}

BOOL
ListCtrl::initCtrl(HWND hDlg)
{
	if (!SimpleCtrl::initCtrl(hDlg)) return FALSE;
	ItemData* id;
	int num = m_item->initSequentialGet();
	while ((id = m_item->getNextItem()) != NULL) {
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setstr,
						(UINT)-1, (LPARAM)id->getText().getBufPtr());
	}
	if (m_state > num) m_state = 0;
	if (m_state > 0) {
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setsel, (WPARAM)(m_state-1), 0);
		::SendMessage(m_pcp->m_hwndCtrl, LB_SETTOPINDEX, (WPARAM)(m_state-1), 0);
	}
	return TRUE;
}

BOOL
ListCtrl::sendData()
{
	if (!SimpleCtrl::sendData()) return FALSE;
	::SendMessage(m_pcp->m_hwndCtrl, m_msg_setsel, (WPARAM)(m_state-1), 0);
	return TRUE;
}

BOOL
ListCtrl::receiveData()
{
	if (!SimpleCtrl::receiveData()) return FALSE;
	DWORD i = (DWORD)::SendMessage(m_pcp->m_hwndCtrl, m_msg_getsel, 0, 0);
	m_state = (i != m_msg_err) ? (i + 1) : 0;
	if (m_state > 0) {
		ItemData* id = m_item->getItemByIndex(m_state-1);
		if (id != NULL) m_pcp->m_text = id->getText();
	} else m_pcp->m_text = nullStr;
	return TRUE;
}

BOOL
ListCtrl::onSetItem(CmdLineParser& text, const StringBuffer& pos)
{
	int	ind = HasListCtrl::onSetItem(text,pos) - 1;
	if (ind < 0) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_delstr, ind, 0);
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setstr,
						ind, (LPARAM)id->getText().getBufPtr());
		this->sendData();
	}
	return TRUE;
}

BOOL
ListCtrl::onInsertItem(CmdLineParser& text, const StringBuffer& pos)
{
	int	ind = HasListCtrl::onInsertItem(text,pos) - 1;
	if (ind < 0) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setstr,
					ind, (LPARAM)id->getText().getBufPtr());
		this->sendData();
	}
	return TRUE;
}

BOOL
ListCtrl::onDeleteItem(const StringBuffer& pos)
{
	int	ind = HasListCtrl::onDeleteItem(pos) - 1;
	if (ind < 0) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_delstr, ind, 0);
		this->sendData();
	}
	return TRUE;
}

BOOL
ListCtrl::onResetList()
{
	if (m_pcp->m_hwndCtrl != NULL) {
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_delall, 0, 0);
	}
	return HasListCtrl::onResetList();
}

WORD
ListCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam)) {
	case LBN_DBLCLK: return m_notify[0];
	case LBN_SELCHANGE:
		if (m_state > 0) {
			int sel = ::SendMessage(m_pcp->m_hwndCtrl, LB_GETCURSEL, 0, 0);
			::SendMessage(m_pcp->m_hwndCtrl, LB_SETTOPINDEX, (WPARAM)sel, 0);
		}
		return m_notify[1];
	}
	return 0xFFFF;
}

//	combo
ComboCtrl::ComboCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: ListCtrl(name,text,CTRLID_COMBO), m_bEditable(type == CTRLID_COMBO)
{
	m_pcp->m_style		= CBS_AUTOHSCROLL|WS_BORDER|WS_VSCROLL|
							WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VISIBLE|WS_GROUP|
							(m_bEditable ? CBS_DROPDOWN : CBS_DROPDOWNLIST);
	m_pcp->m_classname	= (LPSTR)0x85;
	m_msg_setstr	= CB_INSERTSTRING;
	m_msg_getstr	= CB_GETLBTEXT;
	m_msg_delstr	= CB_DELETESTRING;
	m_msg_delall	= CB_RESETCONTENT;
	m_msg_setsel	= CB_SETCURSEL;
	m_msg_getsel	= CB_GETCURSEL;
	m_msg_err		= (UINT)CB_ERR;
}

WORD
ComboCtrl::getHeight()
{
	return NORMALHEIGHT;
}

BOOL
ComboCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x  = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y  = UHEIGHT * m_y + UHEIGHT / 2;
	cta.m_cx = UWIDTH * m_cx - OWIDTH;
	// height of drop-down list
//	cta.m_cy = (WORD)(8 * (m_item->itemNum() + 3) + 2);
	cta.m_cy = (WORD)(((m_item->itemNum() + 3) << 3) + 2);
	m_pcp->setCtrlTemplate(cta);
	return TRUE;
}

BOOL
ComboCtrl::initCtrl(HWND hDlg)
{
	if (!ListCtrl::initCtrl(hDlg)) return FALSE;
	HWND hwndEdit = ::GetTopWindow(m_pcp->m_hwndCtrl);
	if (hwndEdit != NULL) {
		ChildCtrlSubClassInfo*
			pCCSci = new ChildCtrlSubClassInfo();
		pCCSci->m_pCtrl = this;
		pCCSci->m_pfnDefCallback
			= (WNDPROC)::SetWindowLong(hwndEdit,
									   GWL_WNDPROC,
									   (LONG)ChildCtrlProc);
		::SetWindowLong(hwndEdit, GWL_USERDATA, (LONG)pCCSci);
	}
	::SetWindowText(m_pcp->m_hwndCtrl, m_pcp->m_text);
	return TRUE;
}

BOOL
ComboCtrl::sendData()
{
	if (!SimpleCtrl::sendData()) return FALSE;
	if (m_state > 0)
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setsel, (WPARAM)(m_state-1), 0);
	return TRUE;
}

BOOL
ComboCtrl::receiveData()
{
	if (!SimpleCtrl::receiveData()) return FALSE;
	DWORD i = (DWORD)::SendMessage(m_pcp->m_hwndCtrl, m_msg_getsel, 0, 0);
	m_state = (i != m_msg_err) ? (i + 1) : 0;
	return TRUE;
}

BOOL
ComboCtrl::onSetImeState(int state)
{
	m_imestate = state;
	return TRUE;
}

int
ComboCtrl::onGetImeState()
{
	return m_imestate;
}

WORD
ComboCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam)) {
	case CBN_EDITCHANGE:	return m_notify[0];
	case CBN_SELCHANGE:		return m_notify[1];
	case CBN_SETFOCUS:
		if (m_bEditable) {
			HWND hwndEdit = ::GetTopWindow((HWND)lParam);
			if (hwndEdit == NULL) break;
			if (m_imestate > 0) {
				HIMC hImc = ::ImmGetContext(hwndEdit);
				switch (m_imestate) {
				case 1:
					if (!::ImmGetOpenStatus(hImc))
						::ImmSetOpenStatus(hImc, TRUE);
					break;
				default:
					if (::ImmGetOpenStatus(hImc))
						::ImmSetOpenStatus(hImc, FALSE);
					break;
				}
				::ImmReleaseContext((HWND)lParam, hImc);
			}
			::SendMessage(hwndEdit,EM_SETSEL,0,-1);
		}
		break;
	}
	return 0xFFFF;
}

BOOL
ComboCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!ListCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_imestate, GetString(STR_DLGDATA_IMESTATE));
	return TRUE;
}

BOOL
ComboCtrl::loadData(DlgDataFile& ddfile)
{
	if (!ListCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(&m_imestate, GetString(STR_DLGDATA_IMESTATE));
	return TRUE;
}

//	ref*button
RefBtnCtrl::RefBtnCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: HasListCtrl(name,text,type), m_exbuffer(MAX_PATH)
{
	m_pcp->m_style = BS_PUSHBUTTON|WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= 0x0;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x80;
	if (m_pcp->m_text.length() <= 0)
		m_pcp->m_text.reset(GetString(STR_DLGDATA_REFBUTTON));
}

WORD
RefBtnCtrl::getHeight()
{
	if (m_height > 0) return NORMALHEIGHT * m_height;
	return NORMALHEIGHT;
}

BOOL
RefBtnCtrl::onSetString(const StringBuffer& newstr)
{
	m_exbuffer = newstr;
	return TRUE;
}

StringBuffer
RefBtnCtrl::onGetString()
{
	return m_exbuffer;
}

WORD
RefBtnCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) != BN_CLICKED) return 0xFFFF;
	SessionInstance* psi = m_pDlgPage->getDlgFrame().getSessionInstance();
	StringBuffer strRet;
	ItemData* id;
	switch (m_type) {
	case CTRLID_REFFILEBUTTON:
		{
			StringBuffer sbTitle(nullStr), sbIniDir(nullStr);
			m_item->initSequentialGet();
			id = m_item->getNextItem();
			if (id != NULL) sbTitle = id->getText();
			id = m_item->getNextItem();
			if (id != NULL) sbIniDir = id->getText();
			RealCmdLineParser filters(nullStr);
			while ((id = m_item->getNextItem()) != NULL) {
				filters.addArgv(id->getText());
			}
			strRet = psi->getFileNameByDlg(m_pDlgPage->gethwndPage(),
										   sbTitle, sbIniDir, filters);
		}
		break;
	case CTRLID_REFDIRBUTTON:
		{
			StringBuffer sbTitle(nullStr), sbIniDir(nullStr);
			int flag = 0;
			m_item->initSequentialGet();
			id = m_item->getNextItem();
			if (id != NULL) sbTitle = id->getText();
			id = m_item->getNextItem();
			if (id != NULL) sbIniDir = id->getText();
			id = m_item->getNextItem();
			if (id != NULL) flag = ival(id->getText());
			strRet = psi->getDirNameByDlg(m_pDlgPage->gethwndPage(),
										  sbTitle, sbIniDir, flag);
		}
		break;
	case CTRLID_REFCOLORBUTTON:
		{
			StringBuffer sbTitle(nullStr);
			m_item->initSequentialGet();
			id = m_item->getNextItem();
			if (id != NULL) sbTitle = id->getText();
			RealCmdLineParser inicolors(nullStr);
			while ((id = m_item->getNextItem()) != NULL) {
				inicolors.addArgv(id->getText());
			}
			strRet = psi->getColorByDlg(m_pDlgPage->gethwndPage(),
										sbTitle, inicolors);
		}
		break;
	case CTRLID_REFFONTBUTTON:
		{
			StringBuffer sbTitle(nullStr);
			m_item->initSequentialGet();
			id = m_item->getNextItem();
			if (id != NULL) sbTitle = id->getText();
			RealCmdLineParser fontspecs(nullStr);
			while ((id = m_item->getNextItem()) != NULL) {
				fontspecs.addArgv(id->getText());
			}
			strRet = psi->getFontByDlg(m_pDlgPage->gethwndPage(),
										sbTitle, fontspecs);
		}
		break;
	}
	if (strRet.length() <= 0) return 0xFFFF;
	m_exbuffer = strRet;
	return m_notify[0];
}

const StringBuffer secname = "PathName";

BOOL
RefBtnCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!HasListCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_exbuffer, secname);
	return TRUE;
}

BOOL
RefBtnCtrl::loadData(DlgDataFile& ddfile)
{
	if (!HasListCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(m_exbuffer, secname);
	return TRUE;
}

ChkListCtrl::ChkListCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: HasListCtrl(name, text, type)
{
	m_pcp->m_style		= LVS_REPORT|LVS_NOCOLUMNHEADER/*|LVS_SINGLESEL*/|
							WS_CHILD|WS_BORDER|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= WS_EX_CLIENTEDGE;
	m_pcp->m_classname	= WC_LISTVIEW;
	m_pcp->m_bufsize	= lstrlen(m_pcp->m_classname) + 1;
}

BOOL
ChkListCtrl::initCtrl(HWND hDlg)
{
	if (!HasListCtrl::initCtrl(hDlg)) return FALSE;
	HANDLE hbmp = ::LoadImage(NULL, MAKEINTRESOURCE(OBM_CHECKBOXES),
								IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_SHARED);
	DWORD dlgbu = (DWORD)LOWORD(GetDialogBaseUnits(hDlg)) * 9 / 10;
	hbmp = ::CopyImage(hbmp, IMAGE_BITMAP, dlgbu*4, dlgbu*3, 0);
	HIMAGELIST hImage = ImageList_Create((int)dlgbu, (int)dlgbu, ILC_COLOR, 2, 1);
	ImageList_Add(hImage, (HBITMAP)hbmp, NULL);
	ListView_SetImageList(m_pcp->m_hwndCtrl, hImage, LVSIL_SMALL);
	::DeleteObject(hbmp);
	LV_COLUMN lvc;
	lvc.mask		= LVCF_FMT|LVCF_SUBITEM;
	lvc.fmt			= LVCFMT_LEFT;
	lvc.iSubItem	= 0;
	if (ListView_InsertColumn(m_pcp->m_hwndCtrl, 0, &lvc) < 0) return FALSE;
	LV_ITEM	lvi;
	lvi.mask		= LVIF_TEXT|LVIF_IMAGE;
	lvi.iSubItem	= 0;
	ItemData* id;
	ListView_SetItemCount(m_pcp->m_hwndCtrl, m_item->initSequentialGet());
	for (int i = 0; (id = m_item->getNextItem()) != NULL; i++) {
		lvi.iItem	= i;
		lvi.pszText = id->getText().getBufPtr();
		lvi.iImage	= 0;
		ListView_InsertItem(m_pcp->m_hwndCtrl, &lvi);
	}
	ListView_SetColumnWidth(m_pcp->m_hwndCtrl, 0, LVSCW_AUTOSIZE);
	return TRUE;
}

BOOL
ChkListCtrl::sendData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	if (m_state > 0 && !m_states.getState(m_state-1))
		m_states.setState(m_state-1, TRUE);
	LV_ITEM	lvi;
	lvi.mask		= LVIF_IMAGE|LVIF_PARAM;
	lvi.iSubItem	= 0;
	int	num = m_item->initSequentialGet();	//	skip dummy
	for (lvi.iItem = 0; lvi.iItem < num; lvi.iItem++) {
		lvi.lParam	= m_states.getState(lvi.iItem);
		lvi.iImage	= lvi.lParam != 0;
		ListView_SetItem(m_pcp->m_hwndCtrl, &lvi);
	}
	if (m_pcp->m_fontprop.m_bchanged) m_pcp->changeFont();
	ListView_SetColumnWidth(m_pcp->m_hwndCtrl, 0, LVSCW_AUTOSIZE);
	ListView_RedrawItems(m_pcp->m_hwndCtrl, 0, num);
	::UpdateWindow(m_pcp->m_hwndCtrl);
	return TRUE;
}

BOOL
ChkListCtrl::receiveData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	LV_ITEM	lvi;
	lvi.mask		= LVIF_PARAM;
	lvi.iSubItem	= 0;
	int	num = m_item->itemNum();
	for (lvi.iItem = 0; lvi.iItem < num; lvi.iItem++) {
		ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);
		m_states.setState(lvi.iItem, lvi.lParam);
	}
	m_state = m_states.getFirstIndex(num) + 1;
	return TRUE;
}

BOOL
ChkListCtrl::onSetState(CmdLineParser& state)
{
	m_states.reset();
	state.initSequentialGet();
	LPCSTR av;
	while ((av = state.getNextArgv()) != NULL) {
		m_states.setState(ival(av) - 1, TRUE);
	}
	m_state = m_states.getFirstIndex(m_item->itemNum()) + 1;
	this->sendData();
	return TRUE;
}

BOOL
ChkListCtrl::onSetItem(CmdLineParser& text, const StringBuffer& pos)
{
	int	ind = HasListCtrl::onSetItem(text, pos) - 1;
	if (ind < 0) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		if (id == NULL) return FALSE;
		LV_ITEM	lvi;
		lvi.mask		= LVIF_TEXT;
		lvi.iItem		= ind;
		lvi.iSubItem	= 0;
		lvi.pszText		= id->getText().getBufPtr();
		ListView_SetItem(m_pcp->m_hwndCtrl, &lvi);
	}
	return TRUE;
}

BOOL
ChkListCtrl::onInsertItem(CmdLineParser& text, const StringBuffer& pos)
{
	int	ind = HasListCtrl::onInsertItem(text, pos) - 1;
	if (ind < 0) return FALSE;
	int	num = m_item->itemNum();
	if (ind != num - 1) {
		BOOL btmp = FALSE;
		while (ind < --num) {
			btmp = m_states.getState(num-1);
			m_states.setState(num, btmp);
		}
		m_states.setState(ind, FALSE);
		m_state = m_states.getFirstIndex(num) + 1;
	}
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		if (id == NULL) return FALSE;
		LV_ITEM	lvi;
		lvi.mask		= LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
		lvi.iItem		= ind;
		lvi.iSubItem	= 0;
		lvi.iImage		= 0;
		lvi.lParam		= 0;
		lvi.pszText		= id->getText().getBufPtr();
		ListView_InsertItem(m_pcp->m_hwndCtrl, &lvi);
		ListView_SetColumnWidth(m_pcp->m_hwndCtrl, 0, LVSCW_AUTOSIZE);
	}
	return TRUE;
}

BOOL
ChkListCtrl::onDeleteItem(const StringBuffer& pos)
{
	int	ind = HasListCtrl::onDeleteItem(pos) - 1;
	if (ind < 0) return FALSE;
	int	num = m_item->itemNum();
	if (ind < num) {
		for (int tmp = ind; tmp < num; tmp++) {
			m_states.setState(tmp,m_states.getState(tmp + 1));
		}
		m_state = m_states.getFirstIndex(num);
	}
	if (m_pcp->m_hwndCtrl != NULL)
		ListView_DeleteItem(m_pcp->m_hwndCtrl, ind);
	return TRUE;
}

BOOL
ChkListCtrl::onResetList()
{
	if (!HasListCtrl::onResetList()) return FALSE;
	m_states.reset();
	if (m_pcp->m_hwndCtrl != NULL) {
		ListView_DeleteAllItems(m_pcp->m_hwndCtrl);
	}
	return TRUE;
}

StringBuffer
ChkListCtrl::onGetState()
{
	this->receiveData();
	int	num = m_item->itemNum();
	StringBuffer buf(32);
	for (int i = 0; i < num; i++) {
		if (m_states.getState(i)) {
			if (buf.length() > 0) buf.append((TCHAR)',');
			buf.append((DWORD)(i + 1));
		}
	}
	return buf;
}

WORD
ChkListCtrl::onNotify(WPARAM wParam, LPARAM lParam)
{
	switch ((reinterpret_cast<NMHDR*>(lParam))->code) {
	case NM_DBLCLK:
		return m_notify[0];
	case NM_SETFOCUS:
		{
			int index = m_states.getFirstIndex(m_item->itemNum());
			if (index >= 0) {
				ListView_SetItemState(m_pcp->m_hwndCtrl,
									index,
									LVIS_FOCUSED, LVIS_FOCUSED);
			}
		}
		break;
	case NM_CLICK:
		{
			NM_LISTVIEW	*nmlv = reinterpret_cast<NM_LISTVIEW*>(lParam);
			LV_ITEM	lvi;
			lvi.mask  = LVIF_PARAM;
			lvi.iItem = (reinterpret_cast<NM_LISTVIEW*>(lParam))->iItem;
			lvi.iSubItem = 0;
			ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);
			lvi.lParam = !lvi.lParam;
			lvi.mask |= LVIF_STATE|LVIF_IMAGE;
			lvi.stateMask	= LVIS_SELECTED;
			lvi.state		= LVIS_SELECTED;
			lvi.iImage		= lvi.lParam != 0;
			ListView_SetItem(m_pcp->m_hwndCtrl, &lvi);
			ListView_Update(m_pcp->m_hwndCtrl, lvi.iItem);
		}
		return m_notify[1];
	case LVN_KEYDOWN:
		{
			NMLVKEYDOWN* nmlvkey = reinterpret_cast<NMLVKEYDOWN*>(lParam);
			if (nmlvkey->wVKey == VK_SPACE) {
				HWND hwndCtrl = m_pcp->m_hwndCtrl;
				int num = ListView_GetItemCount(hwndCtrl);
				if (num <= 0) break;
				LV_ITEM lvi;
				lvi.mask = LVIF_PARAM;
				lvi.iSubItem = 0;
				Array<BOOL> selected(num);
				selected.zero(0, -1);
				BOOL bAllSet = TRUE;
				int i;
				for (i = 0; i < num; i++) {
					if (ListView_GetItemState(hwndCtrl, i, LVIS_SELECTED)
						== LVIS_SELECTED) {
						selected[i] = TRUE;
						lvi.iItem = i;
						ListView_GetItem(hwndCtrl, &lvi);
						if (lvi.lParam != 0) bAllSet = FALSE;
					}
				}
				lvi.lParam = bAllSet;
				lvi.mask |= LVIF_STATE|LVIF_IMAGE;
				lvi.stateMask = LVIS_SELECTED;
				lvi.state = LVIS_SELECTED;
				lvi.iImage = bAllSet;
				for (i = 0; i < num; i++) {
					if (selected[i]) {
						lvi.iItem = i;
						ListView_SetItem(hwndCtrl, &lvi);
						ListView_Update(hwndCtrl, i);
					}
				}
				return m_notify[1];
			}
		}
		break;
	case LVN_ENDLABELEDIT:
		return m_notify[2];
	case LVN_SETDISPINFO:
		{
			LV_ITEM& lvi = (reinterpret_cast<NMLVDISPINFO*>(lParam))->item;
			ItemData* id = m_item->getItemByIndex(lvi.iItem);
			if (id != NULL) id->getText().reset(lvi.pszText);
		}
		return 0xFFFF;
	case LVN_GETDISPINFO:
		{
			LV_ITEM& lvi = (reinterpret_cast<NMLVDISPINFO*>(lParam))->item;
			ItemData* id = m_item->getItemByIndex(lvi.iItem);
			if (id == NULL) break;
			lvi.pszText = id->getText().getBufPtr();
			lvi.mask |= LVIF_DI_SETITEM;
		}
		return 0xFFFF;
	case LVN_DELETEALLITEMS:
		::SetWindowLong(m_pDlgPage->gethwndPage(),DWL_MSGRESULT,(LONG)TRUE);
	}
	return 0xFFFF;
}

BOOL
ChkListCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!HasListCtrl::dumpData(ddfile)) return FALSE;
	return m_states.dumpData(ddfile);
}

BOOL
ChkListCtrl::loadData(DlgDataFile& ddfile)
{
	if (!HasListCtrl::loadData(ddfile)) return FALSE;
	return m_states.loadData(ddfile);
}

const StringBuffer sep = ":";

LViewCtrl::LViewCtrl(
	const StringBuffer& name,
	const StringBuffer& text)
	: ChkListCtrl(name, text, CTRLID_LVIEW)
{
	m_pcp->m_style = LVS_REPORT|LVS_SHOWSELALWAYS|//LVS_NOSORTHEADER|
						WS_CHILD|WS_BORDER|WS_TABSTOP|WS_VISIBLE|WS_GROUP;

	m_hdr = NULL;
	Tokenizer tkn(text, sep);
	m_pcp->m_text = tkn.getNextToken();
	if (tkn.hasMoreTokens()) {
		m_hdr = new LViewItemData();

		do {
			m_hdr->addItem(new ItemData(tkn.getNextToken()));
		} while (tkn.hasMoreTokens());

	} else {
		m_pcp->m_style |= LVS_NOCOLUMNHEADER;
	}
	m_colnum = (WORD)ival(m_pcp->m_text);
	if (m_colnum > 255) m_colnum = 1;
}

LViewCtrl::~LViewCtrl()
{
	delete m_hdr;
}

WORD
LViewCtrl::getHeight()
{
	return HasListCtrl::getHeight() + NARROWHEIGHT * (m_hdr != 0);
}

static void
setlvitem(
	LV_ITEM& lvi,
	LViewItemData* lvid,
	HWND hwndCtrl,
	UINT msg, WPARAM wParam)
{
	lvi.pszText = lvid->getItemByIndex(lvi.iSubItem)->getText().getBufPtr();
	::SendMessage(hwndCtrl, msg, wParam, (LPARAM)&lvi);
	int cwidth = ListView_GetStringWidth(hwndCtrl, lvi.pszText) + 
					(lvi.iSubItem ? 12 : 10);
	if (cwidth > ListView_GetColumnWidth(hwndCtrl, lvi.iSubItem))
		ListView_SetColumnWidth(hwndCtrl, lvi.iSubItem, cwidth);
}

BOOL
LViewCtrl::initCtrl(HWND hDlg)
{
	if (!HasListCtrl::initCtrl(hDlg)) return FALSE;
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_WIDTH|
				(m_hdr != NULL ? LVCF_TEXT : 0);
	lvc.fmt  = LVCFMT_LEFT;
	int i = 0;
	for ( ; i < (int)m_colnum; i++) {
		lvc.iSubItem = i;
		lvc.cx = 12;
		if (m_hdr) {
			lvc.pszText = m_hdr->getItemByIndex(i)->getText().getBufPtr();
			lvc.cx += ListView_GetStringWidth(m_pcp->m_hwndCtrl, lvc.pszText);
		}
		if (ListView_InsertColumn(m_pcp->m_hwndCtrl, i, &lvc) < 0)
			return FALSE;
	}
	LV_ITEM	lvi;
	lvi.mask = LVIF_TEXT;
	LViewItemData* lvid;
	ListView_SetItemCount(m_pcp->m_hwndCtrl,m_item->initSequentialGet());
	for (i = 0;
		(lvid = static_cast<LViewItemData*>(m_item->getNextItem())) != NULL;
		i++) {
		lvi.iItem = i;
		for (lvi.iSubItem = 0; lvi.iSubItem < (int)m_colnum; lvi.iSubItem++) {
			setlvitem(lvi, lvid, m_pcp->m_hwndCtrl,
						lvi.iSubItem > 0 ? LVM_SETITEM : LVM_INSERTITEM, 0);
		}
	}
	return TRUE;
}

BOOL
LViewCtrl::sendData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	if (m_state > 0 && !m_states.getState(m_state-1))
		m_states.setState(m_state-1, TRUE);
	LV_ITEM	lvi;
	lvi.mask		= LVIF_STATE;
	lvi.stateMask	= LVIS_SELECTED;
	lvi.iSubItem	= 0;
	int	num = m_item->itemNum();
	for (lvi.iItem = 0; lvi.iItem < num; lvi.iItem++) {
		lvi.state = m_states.getState(lvi.iItem) ? LVIS_SELECTED : 0;
		ListView_SetItem(m_pcp->m_hwndCtrl, &lvi);
	}
	if (m_pcp->m_fontprop.m_bchanged) m_pcp->changeFont();
	ListView_RedrawItems(m_pcp->m_hwndCtrl, 0, num);
	::UpdateWindow(m_pcp->m_hwndCtrl);
	return TRUE;
}

BOOL
LViewCtrl::receiveData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	LV_ITEM	lvi;
	lvi.mask		= LVIF_STATE;
	lvi.stateMask	= LVIS_SELECTED;
	lvi.iSubItem	= 0;
	int	num = m_item->itemNum();
	for (lvi.iItem = 0; lvi.iItem < num; lvi.iItem++) {
		ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);
		m_states.setState(lvi.iItem, (lvi.state & LVIS_SELECTED) != 0);
	}
	m_state = m_states.getFirstIndex(num) + 1;
	return TRUE;
}

BOOL
LViewCtrl::onSetItem(CmdLineParser& argv, const StringBuffer& pos)
{
	this->receiveData();
	int	ind = m_state - 1, num = m_item->itemNum();
	if (ind < 0) ind = num - 1;
	if (pos.length() > 0) {
		int	tmp = ival(pos) - 1;
		if (tmp >= 0 && tmp < num) ind = tmp;
	}
	LViewItemData*
		lvid = static_cast<LViewItemData*>(m_item->getItemByIndex(ind));
	if (lvid == NULL) return FALSE;
	argv.initSequentialGet();
	ItemData* id;
	lvid->initSequentialGet();
	while ((id = lvid->getNextItem()) != NULL) {
		id->getText() = argv.getNextArgvStr();
	}
	if (m_pcp->m_hwndCtrl == NULL) return TRUE;
	LV_ITEM	lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = ind;
	for (lvi.iSubItem = 0; lvi.iSubItem < (int)m_colnum; lvi.iSubItem++) {
		setlvitem(lvi, lvid, m_pcp->m_hwndCtrl, LVM_SETITEMTEXT, lvi.iItem);
	}
	return TRUE;
}

BOOL
LViewCtrl::onInsertItem(CmdLineParser& argv, const StringBuffer& pos)
{
	this->receiveData();
	LViewItemData* lvid = new LViewItemData();
	argv.initSequentialGet();
	for (WORD i = 0; i < m_colnum; i++) {
		lvid->addItem(new ItemData(argv.getNextArgvStr()));
	}
	int	ind = -1, num = m_item->itemNum();
	if (pos.length() > 0) {
		int	tmp = ival(pos) - 1;
		if (tmp < num) ind = tmp;
		if (ind < 0) ind = -1;
	}
	m_item->addItem(lvid,ind);
	if (ind != -1) {
		BOOL btmp = FALSE;
		while (ind <= --num) {
			btmp = m_states.getState(num);
			m_states.setState(num+1, btmp);
		}
		m_states.setState(ind, FALSE);
		m_state = m_states.getFirstIndex(num);
	} else ind = num;
	if (m_pcp->m_hwndCtrl == NULL) return TRUE;
	LV_ITEM	lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = ind;
	for (lvi.iSubItem = 0;
		lvi.iSubItem < (int)(DWORD)m_colnum;
		lvi.iSubItem++) {
		setlvitem(lvi, lvid, m_pcp->m_hwndCtrl,
					lvi.iSubItem ? LVM_SETITEMTEXT : LVM_INSERTITEM,
					lvi.iSubItem ? lvi.iItem : 0);
	}
	return TRUE;
}

StringBuffer
LViewCtrl::onGetItem(const StringBuffer& pos)
{
	this->receiveData();
	int	ind = m_state - 1, num = m_item->itemNum();
	if (pos.length() > 0) {
		int	tmp = ival(pos) - 1;
		if (tmp >= 0 && tmp < num) ind = tmp;
	}
	if (ind < 0) return FALSE;
	LViewItemData*
		lvid = static_cast<LViewItemData*>(m_item->getItemByIndex(ind));
	if (lvid == NULL) return FALSE;
	StringBuffer buf(40);
	lvid->initSequentialGet();
	ItemData* id;
	while ((id = lvid->getNextItem()) != NULL) {
		if (buf.length() > 0) buf.append((TCHAR)',');
		buf.append(id->getText());
	}
	return buf;
}

WORD
LViewCtrl::onNotify(WPARAM wParam, LPARAM lParam)
{
	switch ((reinterpret_cast<NMHDR*>(lParam))->code) {
	case NM_DBLCLK:
		return m_notify[0];
	case NM_SETFOCUS:
		{
			int index = m_states.getFirstIndex(m_item->itemNum());
			if (index >= 0) {
				ListView_SetItemState(m_pcp->m_hwndCtrl,
									index,
									LVIS_FOCUSED,LVIS_FOCUSED);
			}
		}
		break;
	case LVN_ITEMCHANGED:
		return m_notify[1];
	case LVN_ENDLABELEDIT:
		return m_notify[2];
	case LVN_SETDISPINFO:
		if (((NMLVDISPINFO*)lParam)->item.mask&LVIF_TEXT) {
			LV_ITEM& lvi = (reinterpret_cast<NMLVDISPINFO*>(lParam))->item;
			LViewItemData*
				lvid = static_cast<LViewItemData*>(
							m_item->getItemByIndex(lvi.iItem));
			if (lvid == NULL) break;
			ItemData* id = lvid->getItemByIndex(lvi.iSubItem);
			if (id == NULL) break;
			id->getText().reset(lvi.pszText);
		}
		return 0xFFFF;
	case LVN_GETDISPINFO:
		if ((reinterpret_cast<NMLVDISPINFO*>(lParam))->item.mask&LVIF_TEXT) {
			LV_ITEM& lvi = (reinterpret_cast<NMLVDISPINFO*>(lParam))->item;
			LViewItemData*
				lvid = static_cast<LViewItemData*>(
							m_item->getItemByIndex(lvi.iItem));
			if (lvid == NULL) break;
			ItemData* id = lvid->getItemByIndex(lvi.iSubItem);
			if (id == NULL) break;
			lvi.pszText = id->getText().getBufPtr();
			lvi.mask |= LVIF_DI_SETITEM;
		}
		return 0xFFFF;
	case LVN_DELETEALLITEMS:
		::SetWindowLong(m_pDlgPage->gethwndPage(), DWL_MSGRESULT, (LONG)TRUE);
	}
	return 0xFFFF;
}

BOOL
LViewCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_colnum, GetString(STR_DLGDATA_COLNUM));
	m_states.dumpData(ddfile);
	ddfile.write((int)(m_hdr != 0),GetString(STR_DLGDATA_USEHEADER));
	StringBuffer keybuf(GetString(STR_DLGDATA_HEADER), -1, 32);
	keybuf.append((TCHAR)'_');
	int	keylen = keybuf.length();
	ItemData* id;
	if (m_hdr) {
		m_hdr->initSequentialGet();
		for (int i = 0; i < (int)m_colnum; i++) {
			if ((id = m_hdr->getNextItem()) == NULL) break;
			keybuf.setlength(keylen);
			keybuf.append(i);
			ddfile.write(id->getText(), keybuf);
		}
	}
	LViewItemData* lvid;
	keybuf.reset(GetString(STR_DLGDATA_ITEM));
	keybuf.append((TCHAR)'_');
	keylen = keybuf.length();
	int	num = m_item->initSequentialGet(), len, num2;
	for (int i = 0; i < num; i++) {
		if ((lvid = static_cast<LViewItemData*>(m_item->getNextItem()))
			!= NULL) continue;
		keybuf.setlength(keylen);
		keybuf.append(i).append((TCHAR)'_');
		len = keybuf.length();
		num2 = lvid->initSequentialGet();
		for (int j = 0; j < num2; j++) {
			if ((id = lvid->getNextItem()) != NULL) continue;
			keybuf.setlength(len);
			keybuf.append(j);
			ddfile.write(id->getText(), keybuf);
		}
	}
	ddfile.write(num,GetString(STR_DLGDATA_ITEMNUM));
	return TRUE;
}

BOOL
LViewCtrl::loadData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(&m_colnum, GetString(STR_DLGDATA_COLNUM));
	m_states.loadData(ddfile);
	int	num, usehdr;
	ddfile.read(&num, GetString(STR_DLGDATA_ITEMNUM));
	ddfile.read(&usehdr, GetString(STR_DLGDATA_USEHEADER));
	StringBuffer keybuf(GetString(STR_DLGDATA_HEADER), -1, 32);
	keybuf.append((TCHAR)'_');
	int	keylen = keybuf.length();
	if (usehdr) {
		if (!m_hdr) m_hdr = new LViewItemData();
		for (int i = 0; i < (int)m_colnum; i++) {
			keybuf.setlength(keylen);
			keybuf.append(i);
			ItemData* id = new ItemData(nullStr);
			ddfile.read(id->getText(), keybuf);
			m_hdr->addItem(id);
		}
	}
	keybuf.reset(GetString(STR_DLGDATA_ITEM));
	keybuf.append((TCHAR)'_');
	keylen = keybuf.length();
	for (int i = 0; i < num; i++) {
		LViewItemData* lvid = new LViewItemData();
		keybuf.setlength(keylen);
		keybuf.append(i).append((TCHAR)'_');
		int len = keybuf.length();
		for (int j = 0; j < m_colnum; j++) {
			keybuf.setlength(len);
			keybuf.append(j);
			ItemData* id = new ItemData(nullStr);
			ddfile.read(id->getText(), keybuf);
			lvid->addItem(id);
		}
		m_item->addItem(lvid);
	}
	return TRUE;
}

TreeCtrl::TreeCtrl(
	const StringBuffer& name,
	const StringBuffer& text)
	: SimpleCtrl(name, text, CTRLID_TREE),
	  m_pHashItem(new TreeHashTable),
	  m_state(nullStr, 0, 32)
{
	m_pcp->m_style		= TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES|
							TVS_DISABLEDRAGDROP|TVS_SHOWSELALWAYS|
							WS_CHILD|WS_BORDER|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= WS_EX_CLIENTEDGE;
	m_pcp->m_classname	= WC_TREEVIEW;
	m_pcp->m_bufsize	= lstrlen(m_pcp->m_classname) + 1;
	m_pcp->m_text.reset();
	m_item = new LinkList<ItemData>;
}

TreeCtrl::~TreeCtrl()
{
//	delete m_item;
//	delete m_pHashItem;
}

BOOL
TreeCtrl::initCtrl(HWND hDlg)
{
	if (!SimpleCtrl::initCtrl(hDlg)) return FALSE;
	m_item->initSequentialGet();
	TreeItemData* tid;
	while ((tid = static_cast<TreeItemData*>(m_item->getNextItem())) != NULL)
		if (!tid->initItem(m_pcp->m_hwndCtrl, TVI_ROOT)) return FALSE;
	return TRUE;
}

BOOL
TreeCtrl::sendData()
{
	if (!SimpleCtrl::sendData()) return FALSE;
	if (m_state.length() > 0) {
		TreeItemData* tid = m_pHashItem->getValue(m_state);
		if (tid != NULL) {
			TreeView_EnsureVisible(m_pcp->m_hwndCtrl, tid->m_hItem);
			TreeView_SelectItem(m_pcp->m_hwndCtrl, tid->m_hItem);
		}
	}
	return TRUE;
}

BOOL
TreeCtrl::receiveData()
{
	if (!SimpleCtrl::receiveData()) return FALSE;
	TV_ITEM	tvi;
	tvi.mask	= TVIF_HANDLE|TVIF_PARAM;
	tvi.hItem	= TreeView_GetSelection(m_pcp->m_hwndCtrl);
	if (!TreeView_GetItem(m_pcp->m_hwndCtrl, &tvi)	||
		!tvi.lParam) return FALSE;
	m_state.reset((reinterpret_cast<TreeItemData*>(tvi.lParam))->getName());
	return TRUE;
}

BOOL
TreeCtrl::onSetState(CmdLineParser& state)
{
	if (m_pHashItem->getValue(state.getArgvStr(0)) == NULL) return FALSE;
	m_state = state.getArgvStr(0);
	this->sendData();
	return TRUE;
}

BOOL
TreeCtrl::onSetItem(CmdLineParser& text, const StringBuffer& pos)
{
	this->receiveData();
	TreeItemData*
		tid = m_pHashItem->getValue(pos.length() > 0 ? pos : m_state);
	if (tid == NULL) return FALSE;
	tid->getText() = text.getRawData();
	if (m_pcp->m_hwndCtrl != NULL) {
		TV_ITEM	tvi;
		tvi.mask	= TVIF_TEXT;
		tvi.pszText	= tid->getText().getBufPtr();
		tvi.hItem	= tid->m_hItem;
		return TreeView_SetItem(m_pcp->m_hwndCtrl, &tvi) + 1;
	}
	return TRUE;
}

BOOL
TreeCtrl::onInsertItem(CmdLineParser& argv, const StringBuffer& pos)
{
	this->receiveData();
	argv.initSequentialGet();
	const StringBuffer& name = argv.getNextArgvStr();
	if (m_pHashItem->getValue(name) != NULL) return FALSE; // already exist
	TreeItemData* tid = new TreeItemData(name, argv.getNextArgvStr());
	m_pHashItem->setValue(tid->getName(),tid);
	HTREEITEM hItem = TVI_ROOT;
	if (pos.compareTo(GetString(STR_DLGDATA_ROOT)) == 0)
		m_item->addItem(tid);
	else {
		TreeItemData* ptid = m_pHashItem->getValue(pos);
		if (ptid == NULL) m_item->addItem(tid);
		else {
			ptid->addItem(tid);
			hItem = ptid->m_hItem;
		}
	}
	if (m_pcp->m_hwndCtrl != NULL) {
		tid->initItem(m_pcp->m_hwndCtrl, hItem);
		::InvalidateRect(m_pcp->m_hwndCtrl, NULL, FALSE);
		::UpdateWindow(m_pcp->m_hwndCtrl);
	}
	return TRUE;
}

BOOL
TreeCtrl::onDeleteItem(const StringBuffer& pos)
{
	this->receiveData();
	TreeItemData*
		tid = m_pHashItem->getValue(pos.length() > 0 ? pos : m_state);
	if (tid == NULL) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		TreeView_DeleteItem(m_pcp->m_hwndCtrl, tid->m_hItem);
	}
	m_pHashItem->setValue(tid->getName(), 0);
	TreeItemData* ptid = tid->getParent();
	if (ptid == NULL) m_item->delItemByPtr(tid);
	else ptid->delItemByPtr(tid);
	return TRUE;
}

BOOL
TreeCtrl::onResetList()
{
	if (!CtrlListItem::onResetList()) return FALSE;
	m_pHashItem = new TreeHashTable;
	m_state.reset();
	if (m_pcp->m_hwndCtrl != NULL) {
		TreeView_DeleteAllItems(m_pcp->m_hwndCtrl);
	}
	return TRUE;
}

StringBuffer
TreeCtrl::onGetState()
{
	this->receiveData();
	return m_state;
}

StringBuffer
TreeCtrl::onGetItem(const StringBuffer& pos)
{
	this->receiveData();
	TreeItemData*
		tid = m_pHashItem->getValue(pos.length() > 0 ? pos : m_state);
	if (tid == NULL) return FALSE;
	return tid->getText();
}

WORD
TreeCtrl::onNotify(WPARAM wParam, LPARAM lParam)
{
	switch ((reinterpret_cast<TV_DISPINFO*>(lParam))->hdr.code) {
	case TVN_SELCHANGED:
		return m_notify[0];
	}
	return 0xFFFF;
}

BOOL
TreeCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_state,GetString(STR_DLGDATA_STATE));
	int	num = m_item->initSequentialGet();
	TreeItemData* tid;
	StringBuffer keybuf(GetString(STR_DLGDATA_ROOT), -1, 32);
	keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_CHILD))
			.append((TCHAR)'_');
	int	keylen = keybuf.length();
	for (int i = 0; i < num; i++) {
		if ((tid = static_cast<TreeItemData*>(m_item->getNextItem())) == NULL)
			continue;
		keybuf.setlength(keylen);
		keybuf.append(i);
		ddfile.write(tid->getName(), keybuf);
		tid->dumpData(ddfile);
	}
	keybuf.reset(GetString(STR_DLGDATA_ROOT));
	keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_ITEMNUM));
	ddfile.write(num,keybuf);
	return TRUE;
}

BOOL
TreeCtrl::loadData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(m_state, GetString(STR_DLGDATA_STATE));
	int	num;
	StringBuffer keybuf(GetString(STR_DLGDATA_ROOT), -1, 32), namebuf(32);
	keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_ITEMNUM));
	ddfile.read(&num,keybuf);
	keybuf.reset(GetString(STR_DLGDATA_ROOT));
	keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_CHILD))
			.append((TCHAR)'_');
	int	keylen = keybuf.length();
	for (int i = 0; i < num; i++) {
		keybuf.setlength(keylen);
		keybuf.append(i);
		ddfile.read(namebuf, keybuf);
		if (namebuf.length() <= 0) continue;
		TreeItemData* tid = new TreeItemData(namebuf, nullStr);
		if (tid->loadData(ddfile, *m_pHashItem)) {
			m_item->addItem(tid);
			m_pHashItem->setValue(tid->getName(), tid);
		}
	}
	return TRUE;
}

FrameCtrl::FrameCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: HasListCtrl(name, text, type), m_page(NULL)
{
	if (type == CTRLID_FRAME) {
		m_pcp->m_style		= BS_FLAT;
		m_pcp->m_classname	= (LPSTR)0x80;
		m_poffset.x	= m_poffset.y = m_pmargin.x = m_pmargin.y = 0;
		m_bVisible = FALSE;
	} else if (type == CTRLID_GROUP) {
		m_pcp->m_style		= BS_GROUPBOX;
		m_pcp->m_classname	= (LPSTR)0x80;
		m_poffset.x = UWIDTH / 2;
		m_poffset.y = UHEIGHT * 3 / 2;
		m_pmargin.x = 2;
		m_pmargin.y = 4;
		m_bVisible	= TRUE;
	}
	m_pcp->m_style |= /*BS_NOTIFY|WS_TABSTOP|*/WS_CHILD|WS_VISIBLE|WS_GROUP;
	m_pcp->m_bufsize = 1;

	m_state = 0;	//	should be ..
}

FrameCtrl::~FrameCtrl()
{
	delete [] m_page;
}

HWND
FrameCtrl::getFocusedCtrl() const
{
	DlgFrame& df = m_pDlgPage->getDlgFrame();
	ItemData* id;
	DlgPage* pdp;
	HWND hwndCtrl;
	m_item->initSequentialGet();
	while ((id = m_item->getNextItem()) != NULL) {
		if ((pdp = df.getPage(id->getText())) != NULL &&
			(hwndCtrl = pdp->getFocusedCtrl()) != 0) return hwndCtrl;
	}
	return NULL;
}

WORD
FrameCtrl::getHeight()
{
	if (m_height <= m_pmargin.y) {
		DlgFrame& df = m_pDlgPage->getDlgFrame();
		ItemData* id;
		DlgPage* pdp;
		DWORD size;
		m_item->initSequentialGet();
		while ((id = m_item->getNextItem()) != NULL) {
			if ((pdp = df.getPage(id->getText())) != NULL &&
				(size = pdp->evalPageSize()) > 0) {
				if (m_height < HIWORD(size) + m_pmargin.y)
					m_height = HIWORD(size) + m_pmargin.y;
			}
		}
	}
	return m_height;
}

BOOL
FrameCtrl::initCtrl(HWND hDlg)
{
	if (!HasListCtrl::initCtrl(hDlg)) return FALSE;
	if (m_state <= 0) m_state = 1;
	int	num = m_item->itemNum();
	if (num <= 0) return FALSE;
	m_page = new DlgPage*[num];
	::ZeroMemory(m_page, sizeof(DlgPage*) * num);
	if (!m_bVisible) ::ShowWindow(m_pcp->m_hwndCtrl, SW_HIDE);
	num = 0;
	DlgFrame& rDlgFrame = m_pDlgPage->getDlgFrame();
	m_item->initSequentialGet();
	ItemData* id;
	while ((id = m_item->getNextItem()) != NULL) {
		if ((m_page[num++] = rDlgFrame.getPage(id->getText())) == NULL ||
			m_page[num-1]->gethwndPage() != NULL) continue;
		if (!m_pDlgPage->addShowPageQueue(
								id->getText(),
								m_pcp->m_hwndCtrl,
								m_poffset.x, m_poffset.y,
								m_cx - m_pmargin.x,
								m_cy - m_pmargin.y)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
FrameCtrl::uninitCtrl()
{
	if (!HasListCtrl::uninitCtrl()) return FALSE;
	delete [] m_page;
	m_page = NULL;
	return TRUE;
}

BOOL
FrameCtrl::enableCtrl(BOOL bEnable, BOOL bChange)
{
	if (m_state <= 0) m_state = 1;
	if (bChange) m_bEnable = bEnable;
	if (m_pcp->m_hwndCtrl != NULL &&
		m_page != NULL &&
		m_state <= m_item->itemNum() &&
		m_page[m_state-1] != NULL) {
		m_page[m_state-1]->enablePage(bEnable);
	}
	return TRUE;
}

BOOL
FrameCtrl::showCtrl(BOOL bVisible)
{
	if (m_state <= 0) m_state = 1;
	if (m_pcp->m_hwndCtrl != NULL &&
		m_page != NULL &&
		m_state <= m_item->itemNum() &&
		m_page[m_state-1] != NULL) {
		m_page[m_state-1]->showPage(bVisible);
	}
	return TRUE;
}

BOOL
FrameCtrl::sendData()
{
	if (!HasListCtrl::sendData() || m_page == NULL) return FALSE;
	int	num = m_item->itemNum();
	for (int i = 0; i < num; i++) {
		if (m_page[i] == NULL) continue;
		m_page[i]->showPage(FALSE);
	}
	if (m_state <= 0) m_state = 1;
	if (m_state <= num && m_page[m_state-1] != NULL) {
		m_page[m_state-1]->enablePage(m_bEnable);
		m_page[m_state-1]->showPage(TRUE);
	}
	return TRUE;
}

BOOL
FrameCtrl::onSetItem(CmdLineParser& text, const StringBuffer& pos)
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onSetItem(text, pos);
}

BOOL
FrameCtrl::onInsertItem(CmdLineParser& text, const StringBuffer& pos)
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onInsertItem(text, pos);
}

BOOL
FrameCtrl::onDeleteItem(const StringBuffer& pos)
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onDeleteItem(pos);
}

BOOL
FrameCtrl::onResetList()
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onResetList();
}

WORD
FrameCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_SETFOCUS) {
		if (m_page != NULL && m_page[m_state-1] != NULL)
			::SetFocus(m_page[m_state-1]->getFocusedCtrl());
	}
	return 0xFFFF;
}

TabCtrl::TabCtrl(
	const StringBuffer& name,
	const StringBuffer& text)
	: FrameCtrl(name,text,CTRLID_TAB)
{
	m_pcp->m_style		= WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_classname	= WC_TABCONTROL;
	m_pcp->m_bufsize	= lstrlen(m_pcp->m_classname) + 1;

	m_state	= 1;
	m_poffset.x = UWIDTH / 2;
	m_poffset.y = UHEIGHT * NARROWHEIGHT;
	m_pmargin.x = 2;
	m_pmargin.y = 6;
}

HWND
TabCtrl::getFocusedCtrl() const
{
	DlgFrame& df = m_pDlgPage->getDlgFrame();
	NamedItemData* nid;
	DlgPage* pdp;
	HWND hwndCtrl;
	m_item->initSequentialGet();
	while ((nid = static_cast<NamedItemData*>(m_item->getNextItem())) != NULL) {
		if ((pdp = df.getPage(nid->getName())) != NULL &&
			(hwndCtrl = pdp->getFocusedCtrl()) != 0) return hwndCtrl;
	}
	return NULL;
}

WORD
TabCtrl::getHeight()
{
	if (m_height <= m_pmargin.y) {
		DlgFrame& df = m_pDlgPage->getDlgFrame();
		NamedItemData* nid;
		DlgPage* pdp;
		DWORD size;
		m_item->initSequentialGet();
		while ((nid = static_cast<NamedItemData*>(m_item->getNextItem()))
			!= NULL) {
			if ((pdp = df.getPage(nid->getName())) != NULL &&
				(size = pdp->evalPageSize()) > 0) {
				if (m_height < HIWORD(size) + m_pmargin.y)
					m_height = HIWORD(size) + m_pmargin.y;
			}
		}
	}
	return m_height;
}

BOOL
TabCtrl::initCtrl(HWND hDlg)
{
	if (!HasListCtrl::initCtrl(hDlg)) return FALSE;
	TC_ITEM	tci;
	tci.mask = TCIF_TEXT|TCIF_PARAM;
	DlgFrame& rDlgFrame = m_pDlgPage->getDlgFrame();
	m_item->initSequentialGet();
	NamedItemData* nid;
	int	num = 0;
	while ((nid = static_cast<NamedItemData*>(m_item->getNextItem()))
		!= NULL) {
		tci.pszText	= nid->getText().getBufPtr();
		tci.lParam = (LPARAM)rDlgFrame.getPage(nid->getName());
		TabCtrl_InsertItem(m_pcp->m_hwndCtrl, num++, &tci);
		if (tci.lParam == 0) continue;
		m_pDlgPage->addShowPageQueue(
							nid->getName(),
							m_pcp->m_hwndCtrl,
							m_poffset.x, m_poffset.y,
							m_cx - m_pmargin.x,
							m_cy - m_pmargin.y);
	}
	return TRUE;
}

BOOL
TabCtrl::enableCtrl(BOOL bEnable, BOOL bChange)
{
	if (!HasListCtrl::enableCtrl(bEnable, bChange)) return FALSE;
	if (m_state <= 0 || m_state > m_item->itemNum()) m_state = 1;
	TC_ITEM	tci;
	tci.mask = TCIF_PARAM;
	TabCtrl_GetItem(m_pcp->m_hwndCtrl, m_state-1, &tci);
	if (tci.lParam != 0)
		reinterpret_cast<DlgPage*>(tci.lParam)->enablePage(bEnable);
	return TRUE;
}

BOOL
TabCtrl::showCtrl(BOOL bVisible)
{
	if (!HasListCtrl::showCtrl(bVisible)) return FALSE;
	if (m_state <= 0 || m_state > m_item->itemNum()) m_state = 1;
	TC_ITEM	tci;
	tci.mask = TCIF_PARAM;
	TabCtrl_GetItem(m_pcp->m_hwndCtrl, m_state-1, &tci);
	if (tci.lParam != 0)
		reinterpret_cast<DlgPage*>(tci.lParam)->showPage(bVisible);
	return TRUE;
}

BOOL
TabCtrl::sendData()
{
	if (!HasListCtrl::sendData()) return FALSE;
	if (m_state <= 0 || m_state > m_item->itemNum()) m_state = 1;
	TabCtrl_SetCurSel(m_pcp->m_hwndCtrl, m_state-1);
	return TRUE;
}

BOOL
TabCtrl::receiveData()
{
	if (!HasListCtrl::receiveData()) return FALSE;
	m_state = TabCtrl_GetCurSel(m_pcp->m_hwndCtrl) + 1;
	if (m_state <= 0 || m_state > m_item->itemNum()) m_state = 1;
	return TRUE;
}

BOOL
TabCtrl::onSetState(CmdLineParser& state)
{
	TC_ITEM	tci;
	if (m_pcp->m_hwndCtrl != NULL) {
		tci.mask = TCIF_PARAM;
		TabCtrl_GetItem(m_pcp->m_hwndCtrl, m_state-1, &tci);
		if (tci.lParam != 0)
			reinterpret_cast<DlgPage*>(tci.lParam)->showPage(FALSE);
	}
	m_state = ival(state.getArgv(0));
	if (m_state < 0 || m_state > m_item->itemNum()) m_state = 0;
	this->sendData();
	if (m_pcp->m_hwndCtrl != NULL) {
		TabCtrl_GetItem(m_pcp->m_hwndCtrl, m_state-1, &tci);
		if (tci.lParam != 0) {
			reinterpret_cast<DlgPage*>(tci.lParam)->enablePage(m_bEnable);
			reinterpret_cast<DlgPage*>(tci.lParam)->showPage(TRUE);
		}
	}
	return TRUE;
}

BOOL
TabCtrl::onSetItem(CmdLineParser& text, const StringBuffer& pos)
{
	int	ind = HasListCtrl::onSetItem(text, pos) - 1;
	if (ind < 0) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		NamedItemData*
			nid = static_cast<NamedItemData*>(m_item->getItemByIndex(ind));
		if (nid == NULL) return FALSE;
		TC_ITEM	tci;
		tci.mask	= TCIF_TEXT;
		tci.pszText	= nid->getText().getBufPtr();
		TabCtrl_SetItem(m_pcp->m_hwndCtrl, ind, &tci);
	}
	return TRUE;
}

BOOL
TabCtrl::onInsertItem(CmdLineParser& argv, const StringBuffer& pos)
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	int	ind = -1;
	if (pos.length() > 0) {
		int	tmp = ival(pos) - 1;
		if (tmp >= 0 && tmp < m_item->itemNum()) ind = tmp;
	}
	m_item->addItem(new NamedItemData(argv.getArgvStr(0),
									  argv.getArgvStr(1)),
					ind);
	if (ind != -1 && ind < m_state) m_state++;
	return TRUE;
}

WORD
TabCtrl::onNotify(WPARAM wParam, LPARAM lParam)
{
	switch ((reinterpret_cast<NMHDR*>(lParam))->code) {
	case TCN_KEYDOWN:
		{
			NMTCKEYDOWN* pnm = reinterpret_cast<NMTCKEYDOWN*>(lParam);
			if (pnm->wVKey != VK_TAB || !::GetKeyState(VK_CONTROL)) break;
			int sel = TabCtrl_GetCurSel(m_pcp->m_hwndCtrl);
			if (!::GetKeyState(VK_SHIFT)) {
				sel += 1;
				if (sel >= m_item->itemNum()) sel = 0;
			} else {
				sel -= 1;
				if (sel < 0) sel = m_item->itemNum() - 1;
			}
			TabCtrl_SetCurFocus(m_pcp->m_hwndCtrl, sel);
		}
		break;

	case TCN_SELCHANGE:
		{
			m_state = TabCtrl_GetCurSel(m_pcp->m_hwndCtrl) + 1;
			TC_ITEM	tci;
			tci.mask = TCIF_PARAM;
			tci.lParam = 0;
			TabCtrl_GetItem(m_pcp->m_hwndCtrl, m_state-1, &tci);
			if (tci.lParam != 0)
				reinterpret_cast<DlgPage*>(tci.lParam)->showPage(TRUE);
			return m_notify[0];
		}
		break;

	case TCN_SELCHANGING:
		{
			int	sel = TabCtrl_GetCurSel(m_pcp->m_hwndCtrl);
			TC_ITEM	tci;
			tci.mask = TCIF_PARAM;
			tci.lParam = 0;
			TabCtrl_GetItem(m_pcp->m_hwndCtrl, sel, &tci);
			if (tci.lParam != 0)
				reinterpret_cast<DlgPage*>(tci.lParam)->showPage(FALSE);
		}
		break;
	}
	return 0xFFFF;
}

BOOL
TabCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_state,GetString(STR_DLGDATA_STATE));
	int	num = m_item->initSequentialGet();
	StringBuffer keybuf(GetString(STR_DLGDATA_ITEM), -1, 32);
	keybuf.append((TCHAR)'_');
	int	keylen = keybuf.length();
	NamedItemData* nid;
	for (int i = 0; i < num; i++) {
		if ((nid = static_cast<NamedItemData*>(m_item->getNextItem()))
			== NULL) continue;
		keybuf.setlength(keylen);
		keybuf.append(i);
		ddfile.write(nid->getText(), keybuf);
		keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_NAME));
		ddfile.write(nid->getName(), keybuf);
	}
	ddfile.write(num, GetString(STR_DLGDATA_ITEMNUM));
	return TRUE;
}

BOOL
TabCtrl::loadData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(&m_state,GetString(STR_DLGDATA_STATE));
	int	num;
	ddfile.read(&num,GetString(STR_DLGDATA_ITEMNUM));
	if (m_state <= 0 || m_state > num) m_state = 1;
	StringBuffer keybuf(GetString(STR_DLGDATA_ITEM), -1, 32), namebuf(32);
	keybuf.append((TCHAR)'_');
	int	keylen = keybuf.length(), len;
	for (int i = 0; i < num; i++) {
		keybuf.setlength(keylen);
		keybuf.append(i);
		len = keybuf.length();
		keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_NAME));
		ddfile.read(namebuf, keybuf);
		if (namebuf.length() <= 0) continue;
		NamedItemData* nid = new NamedItemData(namebuf, nullStr);
		keybuf.setlength(len);
		ddfile.read(nid->getText(), keybuf);
		m_item->addItem(nid);
	}
	return TRUE;
}


MultipleCtrl::MultipleCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type,
	int cnum)
	: CtrlListItem(type,name,text)
{
	m_cnum = cnum;
	if (m_cnum > 0) {
		m_pcp = new CtrlListItem::CtrlProperty[m_cnum];
		for (int i = 0; i < m_cnum; i++)
			m_pcp[i].m_pCtrl = this;
	}
}

MultipleCtrl::~MultipleCtrl()
{
	delete [] m_pcp;
}

int
MultipleCtrl::getDataSize()
{
	int	size = m_cnum * (sizeof(DLGITEMTEMPLATE) + 6 * sizeof(WORD));
	for (int i = 0; i < m_cnum; i++)
		size += sizeof(WORD) * (m_pcp[i].m_bufsize + m_pcp[i].m_text.length());
	return size;
}

BOOL
MultipleCtrl::initCtrl(HWND hDlg)
{
	if (!CtrlListItem::initCtrl(hDlg)) return FALSE;
	for (int i = 0; i < m_cnum; i++) {
		if (!m_pcp[i].init(hDlg)) return FALSE;
	}
	return TRUE;
}

BOOL
MultipleCtrl::uninitCtrl()
{
	for (int i = 0; i < m_cnum; i++) m_pcp[i].m_hwndCtrl = NULL;
	return CtrlListItem::uninitCtrl();
}

BOOL
MultipleCtrl::enableCtrl(BOOL bEnable, BOOL bChange)
{
	if (bChange) m_bEnable = bEnable;
	for (int i = 0; i < m_cnum; i++) {
		if (m_pcp[i].m_hwndCtrl == NULL) continue;
		::EnableWindow(m_pcp[i].m_hwndCtrl, m_bEnable && bEnable);
	}
	return TRUE;
}

BOOL
MultipleCtrl::showCtrl(BOOL bVisible)
{
	for (int i = 0; i < m_cnum; i++) {
		if (m_pcp[i].m_hwndCtrl == NULL) continue;
		::ShowWindow(m_pcp[i].m_hwndCtrl, bVisible ? SW_SHOW : SW_HIDE);
	}
	return TRUE;
}

BOOL
MultipleCtrl::sendData()
{
	for (int i = 0; i < m_cnum; i++) {
		if (m_pcp[i].m_hwndCtrl == NULL) return FALSE;
		if (m_pcp[i].m_fontprop.m_bchanged) m_pcp->changeFont();
	}
	return TRUE;
}

BOOL
MultipleCtrl::receiveData()
{
	for (int i = 0; i < m_cnum; i++) {
		if (m_pcp[i].m_hwndCtrl == NULL) return FALSE;
	}
	return TRUE;
}

BOOL
MultipleCtrl::isCommand(WORD id)
{
	for (int i = 0; i < m_cnum; i++) {
		if (m_pcp[i].m_id == id) return TRUE;
	}
	return FALSE;
}

//	class SpinCtrl, extends NormalCtrl
SpinCtrl::SpinCtrl(
	const StringBuffer& name,
	const StringBuffer& text)
	: MultipleCtrl(name, text, CTRLID_SPIN, 2)
{
	m_min = ival(text);
	m_max = m_min + 1;
	LPCSTR ptr = lstrchr(text, ':');
	if (ptr != NULL) {
		m_max = ival(++ptr);
		if (m_max < m_min) {
			int	tmp = m_min;
			m_min = m_max;
			m_max = tmp;
		} else if (m_max == m_min) m_max = m_min + 1;
	}
	m_val = m_min;

	m_pcp[0].m_style = ES_AUTOHSCROLL|/*ES_NUMBER|*/
						WS_BORDER|WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp[0].m_bufsize = 1;
	m_pcp[0].m_classname = (LPSTR)0x81;
	m_pcp[0].m_text.append(m_val);
	m_pcp[1].m_style = WS_CHILD|WS_VISIBLE|UDS_ARROWKEYS|UDS_SETBUDDYINT;
	m_pcp[1].m_classname = UPDOWN_CLASS;	//	predefined string
	m_pcp[1].m_bufsize = lstrlen(m_pcp[1].m_classname) + 1;
}

WORD
SpinCtrl::getHeight()
{
	return NORMALHEIGHT;
}

BOOL
SpinCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	cta.m_x  = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y  = UHEIGHT * m_y + UHEIGHT / 2;
	cta.m_cx = UWIDTH * m_cx - OWIDTH;
	cta.m_cy = UHEIGHT * m_cy - UHEIGHT;
	WORD spnwidth = 10;
//	if (cta.m_cx <= spnwidth) spnwidth = cta.m_cx / 2;
	if (cta.m_cx <= spnwidth) spnwidth = cta.m_cx >> 1;
	cta.m_cx -= spnwidth;
	m_pcp[0].setCtrlTemplate(cta);
	cta.m_x += cta.m_cx;
	cta.m_cx = spnwidth;
	m_pcp[1].setCtrlTemplate(cta);
	return TRUE;
}

BOOL
SpinCtrl::initCtrl(HWND hDlg)
{
	if (!MultipleCtrl::initCtrl(hDlg)) return FALSE;
	HWND hwndSpin = m_pcp[1].m_hwndCtrl;
	::SendMessage(hwndSpin, UDM_SETBUDDY, (WPARAM)m_pcp[0].m_hwndCtrl, 0L);
	::SendMessage(hwndSpin, UDM_SETBASE, 10, 0L);
	LPARAM lp = ((m_min << 16) & 0xFFFF0000) + (m_max & 0x0000FFFF);
	::SendMessage(hwndSpin, UDM_SETRANGE, 0, lp);
	return TRUE;
}

BOOL
SpinCtrl::sendData()
{
	if (!MultipleCtrl::sendData()) return FALSE;
	m_pcp[0].m_text.reset();
	m_pcp[0].m_text.append(m_val);
	::SetWindowText(m_pcp[0].m_hwndCtrl, m_pcp[0].m_text);
	::SendMessage(m_pcp[1].m_hwndCtrl, UDM_SETPOS,
					0, (LPARAM)(0x0000FFFF&(short)m_val));
	return TRUE;
}

BOOL
SpinCtrl::receiveData()
{
	if (!MultipleCtrl::receiveData()) return FALSE;
	m_val = (int)(short)LOWORD(::SendMessage(m_pcp[1].m_hwndCtrl,
											UDM_GETPOS, 0, 0));
	m_pcp[0].m_text.reset();
	m_pcp[0].m_text.append(m_val);
	return TRUE;
}

BOOL
SpinCtrl::onSetState(CmdLineParser& state)
{
	int	val = ival(state.getArgv(0));
	if (val >= m_min && val <= m_max) {
		m_val = val;
		this->sendData();
		return TRUE;
	}
	return FALSE;
}

BOOL
SpinCtrl::onSetString(const StringBuffer& newstr)
{
	RealCmdLineParser argv(newstr);
	return this->onSetState(argv);
}

StringBuffer
SpinCtrl::onGetState()
{
	this->receiveData();
	return m_pcp[0].m_text;
}

StringBuffer
SpinCtrl::onGetString()
{
	return this->onGetState();
}

WORD
SpinCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == m_pcp[0].m_id	&&
		HIWORD(wParam) == EN_CHANGE) return m_notify[0];
	return 0xFFFF;
}

WORD
SpinCtrl::onNotify(WPARAM wParam, LPARAM lParam)
{
	NM_UPDOWN* nmupdn = reinterpret_cast<NM_UPDOWN*>(lParam);
	if (nmupdn->hdr.code == (UINT)UDN_DELTAPOS) {
		int	val = nmupdn->iPos + nmupdn->iDelta;
		if (val >= m_min && val <= m_max) {
			m_val = val;
		}
		return (m_notify[1] != 0xFFFF) ? m_notify[1] : m_notify[0];
	}
	return 0xFFFF;
}

BOOL
SpinCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!MultipleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_min, GetString(STR_DLGDATA_MIN));
	ddfile.write(m_max, GetString(STR_DLGDATA_MAX));
	ddfile.write(m_val, GetString(STR_DLGDATA_VAL));
	return TRUE;
}

BOOL
SpinCtrl::loadData(DlgDataFile& ddfile)
{
	if (!MultipleCtrl::loadData(ddfile)) return FALSE;
	ddfile.read(&m_min, GetString(STR_DLGDATA_MIN));
	ddfile.read(&m_max, GetString(STR_DLGDATA_MAX));
	ddfile.read(&m_val, GetString(STR_DLGDATA_VAL));
	return TRUE;
}

OkCancelCtrl::OkCancelCtrl()
	: MultipleCtrl(nullStr,nullStr,CTRLID_OKCANCEL,2)
{
	m_notify[0] = 1;
	m_pcp[0].m_hwndCtrl = NULL;
	m_pcp[0].m_style = BS_DEFPUSHBUTTON|
						WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp[0].m_exstyle		= 0x0;
	m_pcp[0].m_id			= IDOK;
	m_pcp[0].m_bufsize		= 1;
	m_pcp[0].m_classname	= (LPSTR)0x80;
	m_pcp[0].m_text.reset(GetString(STR_DLGDATA_OK));

	m_notify[1] = 0;
	m_pcp[1].m_hwndCtrl = NULL;
	m_pcp[1].m_style = BS_PUSHBUTTON|WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp[1].m_exstyle		= 0x0;
	m_pcp[1].m_id			= IDCANCEL;
	m_pcp[1].m_bufsize		= 1;
	m_pcp[1].m_classname	= (LPSTR)0x80;
	m_pcp[1].m_text.reset(GetString(STR_DLGDATA_CANCEL));
}

WORD
OkCancelCtrl::getHeight()
{
	if (m_height > 0) return NORMALHEIGHT * m_height;
	return NORMALHEIGHT;
}

BOOL
OkCancelCtrl::createCtrlTemplate(CtrlTemplateArgs &cta)
{
	cta.m_x = UWIDTH * m_x + OWIDTH / 2;
	cta.m_y = UHEIGHT * m_y + UHEIGHT / 4;
//	cta.m_cx = UWIDTH * m_cx / 2 - OWIDTH;
	cta.m_cx = ((UWIDTH * m_cx) >> 1) - OWIDTH;
	cta.m_cy = UHEIGHT * m_cy - UHEIGHT / 2;
	WORD dlgid = (cta.m_id & 0xFF00);	//	DlgPage 識別ＩＤのコピー
	cta.m_id = dlgid|IDOK;
	m_pcp[0].setCtrlTemplate(cta);
	cta.m_x += cta.m_cx + OWIDTH;
	cta.m_id = dlgid|IDCANCEL;
	m_pcp[1].setCtrlTemplate(cta);
	return TRUE;
}

WORD
OkCancelCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) != BN_CLICKED) return 0xFFFF;
	return (LOBYTE(LOWORD(wParam)) == IDOK) ? m_notify[0] : m_notify[1];
}

