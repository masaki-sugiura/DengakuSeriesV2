//	$Id: ctrldata.cpp,v 1.59 2011/01/07 16:08:38 sugiura Exp $
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
#include <commctrl.h>

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
	ddfile.write(buf, GetString(STR_DLGDATA_STATES), nullStr);
	return TRUE;
}

BOOL
CtrlListItem::States::loadData(DlgDataFile& ddfile)
{
	m_states.clear();
	StringBuffer buf(32);
	ddfile.read(buf, GetString(STR_DLGDATA_STATES), nullStr);
	Tokenizer tknStates(buf, ", ", TRUE); // skip null str
	while (tknStates.hasMoreTokens()) {
		int index = ival(tknStates.getNextToken()) - 1;
		if (index > 0) m_states.setBit(index, TRUE);
	}
	return TRUE;
}

BOOL
CtrlListItem::CtrlFontProperty::dumpData(
	DlgDataFile& ddfile,
	StringBuffer& key)
{
	ddfile.write((DWORD)m_color, key, GetString(STR_DLGDATA_COLOR), nullStr);
//	DEBUG_OUTPUT("Write:Face = %d", m_fface);
	ddfile.write((int)m_fface, key, GetString(STR_DLGDATA_FACE), nullStr);
//	DEBUG_OUTPUT("Write:FontName = %s", (LPCSTR)m_fname);
	ddfile.write(m_fname, key, GetString(STR_DLGDATA_NAME), nullStr);
	return TRUE;
}

BOOL
CtrlListItem::CtrlFontProperty::loadData(
	DlgDataFile& ddfile,
	StringBuffer& key)
{
	ddfile.read((DWORD*)&m_color, key, GetString(STR_DLGDATA_COLOR), nullStr);
	int fface;
	ddfile.read(&fface, key, GetString(STR_DLGDATA_FACE), nullStr);
	m_fface = fface;
//	DEBUG_OUTPUT("Read:Face = %d", m_fface);
	StringBuffer version;
	ddfile.read(version, GetString(STR_DLGDATA_VERSION), GetString(STR_DLGDATA_COMMONPROPERTY));
	if (version[0] >= '2') {
		ddfile.read(m_fname, key, GetString(STR_DLGDATA_NAME), nullStr);
//		DEBUG_OUTPUT("Read:FontName = %s", (LPCSTR)m_fname);
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
		m_text(32),
		m_hbrBackground(NULL),
		m_hDC(NULL)
{
//	::ZeroMemory(&m_fontprop, sizeof(CtrlFontProperty));
}

CtrlListItem::CtrlProperty::~CtrlProperty()
{
	if (m_fontprop.m_hfont) ::DeleteObject(m_fontprop.m_hfont);
	if (m_hbrBackground) ::DeleteObject(m_hbrBackground);
	if (m_hDC) {
		HGDIOBJ hBitmap = ::SelectObject(m_hDC, (HBITMAP)NULL);
		::DeleteObject(hBitmap);
		::DeleteDC(m_hDC);
	}
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

#ifdef _DEBUG
	int size = m_text.toUnicode((LPWSTR)lphdr);

	::OutputDebugString("Str:");
	::OutputDebugStringW((LPWSTR)lphdr);
	::OutputDebugString("\n");
	lphdr += size;
#else
	lphdr += m_text.toUnicode((LPWSTR)lphdr);
#endif
	
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
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfWeight		= 400 + (m_fontprop.m_fface & 1) * 300;
	lf.lfItalic		= (m_fontprop.m_fface & 2) >> 1;
	lf.lfUnderline	= (m_fontprop.m_fface & 4) >> 2;
	lf.lfStrikeOut	= (m_fontprop.m_fface & 8) >> 3;
	if (m_fontprop.m_fname.length() == 0) {
		lstrcpyn(lf.lfFaceName,
				 m_pCtrl->getParentPage().getDlgFrame().getFontName(), 32);
	} else {
		lstrcpyn(lf.lfFaceName, m_fontprop.m_fname, 32);
	}
	if (m_fontprop.m_hfont != NULL) ::DeleteObject(m_fontprop.m_hfont);
	m_fontprop.m_hfont = ::CreateFontIndirect(&lf);
	if (m_fontprop.m_hfont != NULL) {
		DlgFrame& dlgFrame = m_pCtrl->getParentPage().getDlgFrame();
#if 1
		if (dlgFrame.isThemeActive() && m_classname == (LPCSTR)0x82) {
			// XP Theme が有効の場合、タブコントロール内にないスタティックテキストの色が
			// 変わらないことへの対策(何だかなぁ。。。)
			if (m_hDC) {
				HGDIOBJ hBitmap = ::SelectObject(m_hDC, (HBITMAP)NULL);
				::DeleteObject(hBitmap);
				::DeleteDC(m_hDC);
			}
			HDC hDC = ::GetDC(m_hwndCtrl);
			m_hDC = ::CreateCompatibleDC(hDC);
			RECT rcCtrl;
			::GetWindowRect(m_hwndCtrl, &rcCtrl);
			rcCtrl.right -= rcCtrl.left;
			rcCtrl.bottom -= rcCtrl.top;
			rcCtrl.left = rcCtrl.top = 0;
			HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC,
													   rcCtrl.right,
													   rcCtrl.bottom);
			::ReleaseDC(m_hwndCtrl, hDC);
			::SelectObject(m_hDC, hBitmap);
			dlgFrame.drawThemeParentBackground(m_hwndCtrl, m_hDC, &rcCtrl);
			::SetTextColor(m_hDC, m_fontprop.m_color);
			::SetBkMode(m_hDC, TRANSPARENT);
			::SelectObject(m_hDC, m_fontprop.m_hfont);
//			::TextOut(m_hDC, 0, 0, m_text, m_text.length());
			::DrawText(m_hDC, m_text, m_text.length(), &rcCtrl, DT_LEFT | DT_TOP);
		} else {
			::SendMessage(m_hwndCtrl, WM_SETFONT, (WPARAM)m_fontprop.m_hfont, TRUE);
		}
#else
		::SendMessage(m_hwndCtrl, WM_SETFONT, (WPARAM)m_fontprop.m_hfont, TRUE);
#endif
	}
	m_fontprop.m_bchanged = FALSE;
}

LRESULT CALLBACK
CtrlListItemProc(HWND hCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CtrlListItem::CtrlProperty*
		pCProp = (CtrlListItem::CtrlProperty*)::GetWindowLongPtr(hCtrl, GWLP_USERDATA);
	return pCProp->m_pCtrl->dispatchRawMsg(pCProp, hCtrl, uMsg, wParam, lParam);
}

BOOL
CtrlListItem::CtrlProperty::init(HWND hDlg)
{
	m_hwndCtrl = ::GetDlgItem(hDlg, m_id);
	if (m_hwndCtrl == NULL) return FALSE;
//	m_pCtrl->getParentPage().getDlgFrame().setWindowTheme(m_hwndCtrl);
	m_pfnDefCallback = (WNDPROC)::SetWindowLongPtr(m_hwndCtrl,
												   GWLP_WNDPROC,
												   (LONG)CtrlListItemProc);
	::SetWindowLongPtr(m_hwndCtrl, GWLP_USERDATA, (LONG_PTR)this);
	return TRUE;
}

BOOL
CtrlListItem::CtrlProperty::setText()
{
	return MySetWindowText(m_hwndCtrl, m_text);
}

BOOL
CtrlListItem::CtrlProperty::getText()
{
	return MyGetWindowText(m_hwndCtrl, m_text);
}

BOOL
CtrlListItem::CtrlProperty::dumpData(DlgDataFile& ddfile, StringBuffer& key)
{
	ddfile.write(m_id, key, GetString(STR_DLGDATA_ID), nullStr);
	ddfile.write(m_text, key, GetString(STR_DLGDATA_TEXT), nullStr);
	// extended data from Ver.2.00
	ddfile.write(m_style, key, GetString(STR_DLGDATA_STYLE), nullStr);
	ddfile.write(m_exstyle, key, GetString(STR_DLGDATA_EXSTYLE), nullStr);
	key.append(GetString(STR_DLGDATA_FONT)).append((TCHAR)':');
	return m_fontprop.dumpData(ddfile, key);
}

BOOL
CtrlListItem::CtrlProperty::loadData(DlgDataFile& ddfile, StringBuffer& key)
{
	ddfile.read(&m_id, key, GetString(STR_DLGDATA_ID), nullStr);
	ddfile.read(m_text, key, GetString(STR_DLGDATA_TEXT), nullStr);
	// extended data from Ver.2.00
	StringBuffer version;
	ddfile.read(version, GetString(STR_DLGDATA_VERSION), GetString(STR_DLGDATA_COMMONPROPERTY));
	if (version[0] >= '2') {
		ddfile.read(&m_style, key, GetString(STR_DLGDATA_STYLE), nullStr);
		ddfile.read(&m_exstyle, key, GetString(STR_DLGDATA_EXSTYLE), nullStr);
	}
	key.append(GetString(STR_DLGDATA_FONT)).append((TCHAR)':');
	return m_fontprop.loadData(ddfile, key);
}

static int CALLBACK
CompareTreeItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TreeItemData* tid1 = (TreeItemData*)lParam1;
	TreeItemData* tid2 = (TreeItemData*)lParam2;
	return lstrcmp(tid1->getText(), tid2->getText());
}

BOOL
TreeItemData::initItem(HWND hCtrl, HTREEITEM hParent, BOOL bSort)
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
	while ((tid = this->getNextItem()) != NULL) {
		if (!tid->initItem(hCtrl,m_hItem, bSort)) return FALSE;
	}
	TVSORTCB tvs;
	tvs.hParent = hParent;
	tvs.lpfnCompare = (PFNTVCOMPARE)CompareTreeItems;
	if (bSort) TreeView_SortChildrenCB(hCtrl, &tvs, 0);
	return TRUE;
}

BOOL
TreeItemData::dumpData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	StringBuffer keybuf(32);
	keybuf.append(m_name).append((TCHAR)':');
	int	num = this->initSequentialGet();
	ddfile.write(m_text, keybuf, GetString(STR_DLGDATA_TEXT), nullStr);
	ddfile.write(num, keybuf, GetString(STR_DLGDATA_ITEMNUM), nullStr);
	keybuf.append(GetString(STR_DLGDATA_CHILD)).append((TCHAR)'_');
	for (int i = 0; i < num; i++) {
		TreeItemData* tid = this->getNextItem();
		if (tid == NULL) continue;
		char nbuf[16];
		itoa(i, nbuf, 10);
		ddfile.write(tid->getName(), keybuf, nbuf, nullStr);
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
	ddfile.read(m_text, keybuf, GetString(STR_DLGDATA_TEXT), nullStr);
	int num;
	ddfile.read(&num, keybuf, GetString(STR_DLGDATA_ITEMNUM), nullStr);
	keybuf.append(GetString(STR_DLGDATA_CHILD)).append((TCHAR)'_');
	StringBuffer namebuf(32);
	for (int i = 0; i < num; i++) {
		char nbuf[16];
		itoa(i, nbuf, 10);
		ddfile.read(namebuf, keybuf, nbuf, nullStr);
		if (namebuf.length() <= 0) continue;
		TreeItemData* tid = new TreeItemData(namebuf, nullStr);
		if (tid->loadData(ddfile, hash)) {
			this->addItem(tid);
			hash.setValue(tid->getName(), tid);
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
	::FillMemory(m_notify, sizeof(m_notify), 0xFF);
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
		case CTRLID_DEFBUTTON:
			nc = new BtnCtrl(name,text,static_cast<CTRL_ID>(ctrltype));
			break;
		case CTRLID_CHECK:
		case CTRLID_RDBTN:
			nc = new CheckCtrl(name,text,static_cast<CTRL_ID>(ctrltype));
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
CtrlListItem::onWmCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_bInitDone ? this->onCtlColor((HWND)lParam, uMsg, (HDC)wParam) : NULL;
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

WORD
CtrlListItem::getDefID() const
{
	return 0xFFFF;
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
	while ((av = argv.getNextArgv()) != NULL &&
		   num < sizeof(m_notify) / sizeof(m_notify[0]))
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

int
CtrlListItem::onSetFocusedItem(const StringBuffer& item)
{
	return 0;
}

StringBuffer
CtrlListItem::onGetFocusedItem()
{
	return nullStr;
}

int
CtrlListItem::onSetCtrlExProperty(const StringBuffer& key, const StringBuffer& value)
{
	return 0;
}

StringBuffer
CtrlListItem::onGetCtrlExProperty(const StringBuffer& key)
{
	return nullStr;
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
#if 0
	case WM_PAINT:
		{
			LRESULT ret = ::CallWindowProc(pCProp->m_pfnDefCallback,
											hCtrl, uMsg, wParam, lParam);
			PAINTSTRUCT ps;
			::BeginPaint(hCtrl, &ps);
			pCProp->m_pCtrl->getParentPage()
				.getDlgFrame().drawThemeParentBackground(hCtrl, ps.hdc, &ps.rcPaint);
			::EndPaint(hCtrl, &ps);
			return ret;
		}
		break;
#endif
#if 1
	// XP Theme が有効の場合、タブコントロール内にないスタティックテキストの色が
	// 変わらないことへの対策(何だかなぁ。。。)
	case WM_PAINT:
		if (pCProp->m_hDC) {
			PAINTSTRUCT ps;
			::BeginPaint(hCtrl, &ps);
			::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top,
					 ps.rcPaint.right - ps.rcPaint.left,
					 ps.rcPaint.bottom - ps.rcPaint.top,
					 pCProp->m_hDC,
					 ps.rcPaint.left, ps.rcPaint.top,
					 SRCCOPY);
			::EndPaint(hCtrl, &ps);
			return 0;
		}
		return ::CallWindowProc(pCProp->m_pfnDefCallback, hCtrl, uMsg, wParam, lParam);
#endif
	case WM_GET_CTRL_PTR:
		return (LRESULT)pCProp->m_pCtrl;
#if 0
	case WM_CTLCOLORSTATIC:
		::SetTextColor((HDC)wParam, pCProp->m_fontprop.m_color);
		::SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)::GetSysColorBrush(COLOR_BTNFACE);
#endif
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
CtrlListItem::onCtlColor(HWND hwndCtrl, UINT, HDC)
{
	return 0;
}

WORD
CtrlListItem::onImeNotify(WPARAM, LPARAM)
{
	return 0xFFFF;
}

BOOL
CtrlListItem::onSetSort(CmdLineParser&)
{
	return FALSE;
}

StringBuffer
CtrlListItem::onGetSort()
{
	return nullStr;
}

BOOL
CtrlListItem::dumpData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	if (m_pcp && m_pcp->m_hwndCtrl) {
		this->receiveData();
	}
	ddfile.write(m_type, GetString(STR_DLGDATA_TYPE), nullStr);
	ddfile.write(m_name, GetString(STR_DLGDATA_NAME), nullStr);
	ddfile.write(m_text, GetString(STR_DLGDATA_TEXT), nullStr);
	ddfile.write(m_cnum, GetString(STR_DLGDATA_INTERNALCTRLNUM), nullStr);
	ddfile.write(m_bEnable, GetString(STR_DLGDATA_ENABLE), nullStr);
	StringBuffer buf(GetString(STR_DLGDATA_NOTIFY), -1, 2);
	buf.append((TCHAR)'_');
	int len = buf.length();
	for (int i = 0; i < sizeof(m_notify) / sizeof(m_notify[0]); i++) {
		char nbuf[16];
		itoa(i, nbuf, 10);
		ddfile.write(m_notify[i], buf, nbuf, nullStr);
	}
	ddfile.write(m_width, GetString(STR_DLGDATA_WIDTH), nullStr);
	ddfile.write(m_height, GetString(STR_DLGDATA_HEIGHT), nullStr);
	if (m_cnum > 0 && m_pcp != NULL) {
		StringBuffer keybuf(GetString(STR_DLGDATA_CTRL),-1,32);
		keybuf.append((TCHAR)'_');
		int	keylen = keybuf.length();
		for (int i = 0; i < m_cnum; i++) {
			keybuf.append(i).append((TCHAR)':');
			m_pcp[i].dumpData(ddfile, keybuf);
			keybuf.setlength(keylen);
		}
	}
	return TRUE;
}

BOOL
CtrlListItem::loadData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	ddfile.read(&m_cnum, GetString(STR_DLGDATA_INTERNALCTRLNUM), nullStr);
	ddfile.read(&m_bEnable, GetString(STR_DLGDATA_ENABLE), nullStr);
	StringBuffer buf(GetString(STR_DLGDATA_NOTIFY), -1, 2);
	buf.append((TCHAR)'_');
	for (int i = 0; i < sizeof(m_notify) / sizeof(m_notify[0]); i++) {
		char nbuf[16];
		itoa(i, nbuf, 10);
		if (!ddfile.read(&m_notify[i], buf, nbuf, nullStr)) {
			m_notify[i] = 0xFFFF;
		}
	}
	ddfile.read(&m_width, GetString(STR_DLGDATA_WIDTH), nullStr);
	ddfile.read(&m_height, GetString(STR_DLGDATA_HEIGHT), nullStr);
	if (m_cnum > 0 && m_pcp != NULL) {
		StringBuffer keybuf(GetString(STR_DLGDATA_CTRL), -1, 32);
		keybuf.append((TCHAR)'_');
		int	keylen = keybuf.length();
		for (int i = 0; i < m_cnum; i++) {
			keybuf.append(i).append((TCHAR)':');
			m_pcp[i].loadData(ddfile, keybuf);
			keybuf.setlength(keylen);
		}
	}

	// load 時点では HWND == NULL && sendData() は HWND == NULL で FALSE を返す
//	return this->sendData();
	return TRUE;
}

//	ctrl with one control
SimpleCtrl::SimpleCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	:	CtrlListItem(type,name,text)
	,	m_crefBkColor(0)
	,	m_bSetBkColor(FALSE)
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
	BOOL bActualState = bChange ? bEnable : (m_bEnable && bEnable);
	if (m_pcp->m_hwndCtrl != NULL) {
		::EnableWindow(m_pcp->m_hwndCtrl, bActualState);
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
//	::SetWindowText(m_pcp->m_hwndCtrl, m_pcp->m_text);
	if (m_pcp->m_fontprop.m_bchanged)
		m_pcp->changeFont();
	m_pcp->setText();
	return TRUE;
}

BOOL
SimpleCtrl::receiveData()
{
	//	on default, get text of first control
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
//	int	len = ::SendMessage(m_pcp->m_hwndCtrl, WM_GETTEXTLENGTH, 0, 0);
//	m_pcp->m_text.resize(len);
//	::GetWindowText(m_pcp->m_hwndCtrl, m_pcp->m_text.getBufPtr(), len + 1);
//	return (m_pcp->m_text.length() == len);
	return m_pcp->getText();
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
SimpleCtrl::onCtlColor(HWND hwndCtrl, UINT uMsg, HDC hDc)
{
	if (!::IsWindowEnabled(hwndCtrl)) {
		return NULL;
	}

	if (m_pcp->m_hbrBackground) {
		::DeleteObject(m_pcp->m_hbrBackground);
		m_pcp->m_hbrBackground = NULL;
	}

	LOGBRUSH lbr;
	if (m_bSetBkColor)
	{
		lbr.lbColor = m_crefBkColor;
	}
	else
	{
		lbr.lbColor = ::GetBkColor(hDc);
	}
	lbr.lbStyle = BS_SOLID;
	m_pcp->m_hbrBackground = ::CreateBrushIndirect(&lbr);

	DEBUG_OUTPUT(("%s: TextColor = %08x, BkColor = %08x", (LPCSTR)this->m_name, m_pcp->m_fontprop.m_color, lbr.lbColor));

	::SetTextColor(hDc, m_pcp->m_fontprop.m_color);
	::SetBkColor(hDc, lbr.lbColor);

	return m_pcp->m_hbrBackground;
}

int
SimpleCtrl::onSetCtrlExProperty(const StringBuffer& key, const StringBuffer& value)
{
	if (key.compareTo("bgcolor") != 0)
	{
		return 0;
	}

	if (value.length() == 0)
	{
		m_bSetBkColor	= FALSE;
		m_crefBkColor	= 0;
	}
	else
	{
		m_bSetBkColor	= TRUE;
		m_crefBkColor	= ColorTable::colorStrToColorRef(value);
	}

	this->sendData();

	return 1;
}

StringBuffer
SimpleCtrl::onGetCtrlExProperty(const StringBuffer& key)
{
	if (key.compareTo("bgcolor") != 0)
	{
		return nullStr;
	}

	return m_bSetBkColor ? ColorTable::colorRefToColorStr(m_crefBkColor) : nullStr;
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
	m_pcp->m_style		= WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= 0x0;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x80;

	m_pcp->m_style |= (m_type == (DWORD)CTRLID_DEFBUTTON) ?
						BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}

WORD
BtnCtrl::getHeight()
{
	if (m_height > 0) return NORMALHEIGHT * m_height;
	return NORMALHEIGHT;
}

WORD
BtnCtrl::getDefID() const
{
//	DWORD dwStyle = ::GetWindowLong(m_pcp->m_hwndCtrl, GWL_STYLE);
//	if (dwStyle & BS_DEFPUSHBUTTON) return m_pcp->m_id;
#if 0
	TCHAR buf[80];
	wsprintf(buf, "id[%d]::getDefID() is invoked!! (%x, %x)\n",
			 m_notify[0], (m_pcp->m_style & BS_TYPEMASK), m_type);
	::OutputDebugString(buf);
#endif
	if ((m_pcp->m_style & BS_TYPEMASK) == BS_DEFPUSHBUTTON) {
		return m_pcp->m_id;
	}
	return 0xFFFF;
}

HBRUSH
BtnCtrl::onCtlColor(HWND hwndCtrl, UINT uMsg, HDC hDc)
{
	if (!::IsWindowEnabled(hwndCtrl)) {
		return NULL;
	}

	if (m_pcp->m_hbrBackground) {
		::DeleteObject(m_pcp->m_hbrBackground);
		m_pcp->m_hbrBackground = NULL;
	}

	LOGBRUSH lbr;
	if (m_bSetBkColor)
	{
		lbr.lbColor = m_crefBkColor;
	}
	else
	{
		lbr.lbColor = m_pDlgPage->getBkColor();
	}

	lbr.lbStyle = BS_SOLID;
	m_pcp->m_hbrBackground = ::CreateBrushIndirect(&lbr);

	DEBUG_OUTPUT(("%s: TextColor = %08x, BkColor = %08x", (LPCSTR)this->m_name, m_pcp->m_fontprop.m_color, lbr.lbColor));

	::SetTextColor(hDc, m_pcp->m_fontprop.m_color);
	::SetBkColor(hDc, lbr.lbColor);

	return uMsg == WM_CTLCOLORBTN ? m_pcp->m_hbrBackground : NULL;
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
	m_pcp->m_exstyle	= 0;
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

BOOL
TextCtrl::onSetCtrlFont(CmdLineParser& argv)
{
	if (argv.itemNum() < 1) return FALSE;
	argv.initSequentialGet();
	const StringBuffer& fface = argv.getNextArgvStr();
	const StringBuffer& color = argv.getNextArgvStr();
	const StringBuffer& fname = argv.getNextArgvStr();
	this->setFont(fface, color, fname);
	if (m_pDlgPage->getDlgFrame().isThemeActive() && m_pcp->m_hwndCtrl) {
		// XP Theme がアクティブの場合、無理矢理changeFont()を呼び出す
		m_pcp->changeFont();
		::InvalidateRect(m_pcp->m_hwndCtrl, NULL, TRUE);
		::UpdateWindow(m_pcp->m_hwndCtrl);
	} else {
		this->sendData();
	}
	return TRUE;
}

HBRUSH
TextCtrl::onCtlColor(HWND hwndCtrl, UINT uMsg, HDC hDc)
{
	if (m_pDlgPage->getDlgFrame().isThemeActive()) {
#ifdef _DEBUG
		{
			char msgbuf[80];
			wsprintf(msgbuf, "%s: TextColor = %08x, BkColor = %08x\n",
					 (LPCSTR)this->m_name,
					 m_pcp->m_fontprop.m_color,
					 m_crefBkColor);
			::OutputDebugString(msgbuf);
		}
#endif
		::SetTextColor(hDc, m_pcp->m_fontprop.m_color);
		if (m_bSetBkColor)
		{
			::SetBkColor(hDc, m_crefBkColor);
		}
		else
		{
			m_pDlgPage->getDlgFrame().drawThemeParentBackground(m_pcp->m_hwndCtrl, hDc, NULL);
		}
		return NULL;
	} else {
#if 1
		COLORREF	bgColor;
		if (m_bSetBkColor)
		{
			bgColor = m_crefBkColor;
		}
		else
		{
//			bgColor = ::GetBkColor(hDc);
//			bgColor = ::GetSysColor(COLOR_3DFACE);
			bgColor = m_pDlgPage->getBkColor();
		}
		if (!m_pcp->m_hbrBackground) {
			LOGBRUSH lbr;
			lbr.lbColor = bgColor;
			lbr.lbStyle = BS_SOLID;
			m_pcp->m_hbrBackground = ::CreateBrushIndirect(&lbr);
		}
		::SetTextColor(hDc, m_pcp->m_fontprop.m_color);
		::SetBkColor(hDc, bgColor);
		return m_pcp->m_hbrBackground;
#else
		return SimpleCtrl::onCtlColor(hwndCtrl, uMsg, hDc);
#endif
	}
}

EditCtrl::EditCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: SimpleCtrl(name,text,type),
	  m_imestate(0L),
	  m_bAlreadyFocused(FALSE),
	  m_bSelectOnFocused(TRUE)
{
	m_pcp->m_style		= /*ES_AUTOHSCROLL|*/
							WS_BORDER|WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	switch (type) {
	case CTRLID_MLEDIT:
		m_pcp->m_style |= ES_AUTOVSCROLL|ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL;
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
EditCtrl::initCtrl(HWND hDlg)
{
	m_bAlreadyFocused = FALSE;
	return SimpleCtrl::initCtrl(hDlg);
}

BOOL
EditCtrl::sendData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	if (m_pcp->m_fontprop.m_bchanged)
		m_pcp->changeFont();
//	::SetWindowText(m_pcp->m_hwndCtrl,
//					StringBuffer(m_pcp->m_text).replaceStr("\n", "\r\n",-1));
	MySetWindowText(m_pcp->m_hwndCtrl,
					StringBuffer(m_pcp->m_text).replaceStr("\n", "\r\n", -1));
	return TRUE;
}

BOOL
EditCtrl::receiveData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
//	int	len = ::SendMessage(m_pcp->m_hwndCtrl, WM_GETTEXTLENGTH, 0, 0);
//	m_pcp->m_text.resize(len);
//	::GetWindowText(m_pcp->m_hwndCtrl,m_pcp->m_text.getBufPtr(), len + 1);
//	m_pcp->m_text.replaceStr("\r\n","\n",-1);
	BOOL bRet = m_pcp->getText();
	if (bRet) {
		m_pcp->m_text.replaceStr("\r\n", "\n", -1);
	}
	return bRet;
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
	m_bAlreadyFocused = FALSE;
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
		{
			DlgFrame& dlgFrame = m_pDlgPage->getDlgFrame();
			int nState = dlgFrame.getImeState();
			if (nState > 2 && dlgFrame.isImeAlreadyFocused()) {
				// 既にいずれかのコントロールがフォーカスを得ている
				nState = 0;
			}
			if (m_imestate > 0 || nState > 0) {
				// グローバル設定をローカル設定で上書き
				if (m_imestate > 0) nState = m_imestate;
				HIMC hImc = ::ImmGetContext((HWND)lParam);
				switch (nState) {
				case 3:
					if (m_bAlreadyFocused) break;
					// through down
				case 1:
					if (!::ImmGetOpenStatus(hImc)) ::ImmSetOpenStatus(hImc,TRUE);
					break;
				case 4:
					if (m_bAlreadyFocused) break;
					// through down
				case 2:
					if (::ImmGetOpenStatus(hImc)) ::ImmSetOpenStatus(hImc,FALSE);
					break;
				default:
					break;
				}
				::ImmReleaseContext((HWND)lParam,hImc);
			}
			if (m_bSelectOnFocused)
			{
				::SendMessage((HWND)lParam, EM_SETSEL, 0, -1);
			}
			else
			{
				::SendMessage((HWND)lParam, EM_SETSEL, -1, 0);
			}
			m_bAlreadyFocused = TRUE;
			dlgFrame.setImeAlreadyFocused();
		}
		break;
	}
	return 0xFFFF;
}

WORD
EditCtrl::onImeNotify(WPARAM wParam, LPARAM lParam)
{
	return 0xFFFF;
}

int
EditCtrl::onSetCtrlExProperty(const StringBuffer& key, const StringBuffer& value)
{
	if (key.compareTo("select_on_focused") == 0)
	{
		m_bSelectOnFocused = (value.compareTo("true") == 0);

		return 1;
	}
	else if (key.compareTo("auto_h_scroll") == 0)
	{
		if (value.compareTo("true") == 0)
		{
			m_pcp->m_style |= ES_AUTOHSCROLL;
		}
		else
		{
			m_pcp->m_style &= ~ES_AUTOHSCROLL;
		}

		return 1;
	}

	return SimpleCtrl::onSetCtrlExProperty(key, value);
}

StringBuffer
EditCtrl::onGetCtrlExProperty(const StringBuffer& key)
{
	if (key.compareTo("select_on_focused") == 0)
	{
		return m_bSelectOnFocused ? "true" : "false";
	}
	else if (key.compareTo("auto_h_scroll") == 0)
	{
		return (m_pcp->m_style & ES_AUTOHSCROLL) != 0 ? "true" : "false";
	}

	return SimpleCtrl::onGetCtrlExProperty(key);
}

BOOL
EditCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_imestate, GetString(STR_DLGDATA_IMESTATE), nullStr);
	return TRUE;
}

BOOL
EditCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_imestate, GetString(STR_DLGDATA_IMESTATE), nullStr);
	return SimpleCtrl::loadData(ddfile);
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
CheckCtrl::createCtrlTemplate(CtrlListItem::CtrlTemplateArgs& cta)
{
	if (m_type == CTRLID_RDBTN)
	{
		CtrlListItem*	pPrevCtrl = this->getParentPage().prevCtrl(this);
		if (pPrevCtrl != NULL	&&
			pPrevCtrl->getCtrlType() == CTRLID_RDBTN)
		{
			m_pcp->m_style	&= ~WS_GROUP;
		}
	}

	return SimpleCtrl::createCtrlTemplate(cta);
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

HWND
CheckCtrl::getFocusedCtrl() const
{
	if (m_type != CTRLID_RDBTN	||
		m_state != 0)
	{
		return m_pcp->m_hwndCtrl;
	}

	//	ラジオボタンがフォーカスを得る場合、
	//	チェックされているラジオボタンにフォーカスを移動

	const CtrlListItem*	cli = this;
	while ((cli = const_cast<CheckCtrl*>(this)->getParentPage().prevCtrl(cli)) != NULL)
	{
		if (cli->getCtrlType() != CTRLID_RDBTN)
		{
			break;
		}

		if (((CheckCtrl*)cli)->m_state != 0)
		{
			return cli->getCtrlHWND();
		}
	}

	cli = this;
	while ((cli = const_cast<CheckCtrl*>(this)->getParentPage().nextCtrl(cli)) != NULL)
	{
		if (cli->getCtrlType() != CTRLID_RDBTN)
		{
			break;
		}

		if (((CheckCtrl*)cli)->m_state != 0)
		{
			return cli->getCtrlHWND();
		}
	}

	return m_pcp->m_hwndCtrl;
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
	ddfile.write(m_state, GetString(STR_DLGDATA_STATE), nullStr);
	return TRUE;
}

BOOL
CheckCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_state, GetString(STR_DLGDATA_STATE), nullStr);
	return BtnCtrl::loadData(ddfile);
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

HBRUSH
TrackCtrl::onCtlColor(HWND hwndCtrl, UINT uMsg, HDC hDc)
{
	return NULL;
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
				return m_notify[(m_notify[1] != 0xFFFF)];
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
	ddfile.write(m_min, GetString(STR_DLGDATA_MIN), nullStr);
	ddfile.write(m_max, GetString(STR_DLGDATA_MAX), nullStr);
	ddfile.write(m_val, GetString(STR_DLGDATA_VAL), nullStr);
	ddfile.write(m_tic, GetString(STR_DLGDATA_TIC), nullStr);
	return TRUE;
}

BOOL
TrackCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_min, GetString(STR_DLGDATA_MIN), nullStr);
	ddfile.read(&m_max, GetString(STR_DLGDATA_MAX), nullStr);
	ddfile.read(&m_val, GetString(STR_DLGDATA_VAL), nullStr);
	ddfile.read(&m_tic, GetString(STR_DLGDATA_TIC), nullStr);
	return SimpleCtrl::loadData(ddfile);
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
			else ind = -1;
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
	id->getText() = RemoveQuote(text.getRawData());
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
	m_item->addItem(new ItemData(RemoveQuote(text.getRawData())), ind);
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
	ddfile.write(m_state, GetString(STR_DLGDATA_STATE), nullStr);
	StringBuffer buf(GetString(STR_DLGDATA_ITEM), -1, 32);
	buf.append((TCHAR)'_');
	int	num = m_item->initSequentialGet();
	for (int i = 0; i < num; i++) {
		ItemData* id = m_item->getNextItem();
		if (id == NULL) continue;
		char nbuf[16];
		itoa(i, nbuf, 10);
		ddfile.write(id->getText(), buf, nbuf, nullStr);
	}
	ddfile.write(num, GetString(STR_DLGDATA_ITEMNUM), nullStr);
	return TRUE;
}

BOOL
HasListCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_state, GetString(STR_DLGDATA_STATE), nullStr);
	int	num;
	ddfile.read(&num, GetString(STR_DLGDATA_ITEMNUM), nullStr);
	StringBuffer buf(GetString(STR_DLGDATA_ITEM), -1, 32);
	buf.append((TCHAR)'_');
	for (int i = 0; i < num; i++) {
		ItemData* id = new ItemData(nullStr);
		char nbuf[16];
		itoa(i, nbuf, 10);
		ddfile.read(id->getText(), buf, nbuf, nullStr);
		m_item->addItem(id);
	}

	return SimpleCtrl::loadData(ddfile);
}

void
HasListCtrl::setViewIndex()
{
	// nothing to do.
}

void
HasListCtrl::getStateFromView()
{
	// nothing to do.
}

static LRESULT CALLBACK
ChildCtrlProc(HWND hCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ChildCtrlSubClassInfo*
		pCCSci = (ChildCtrlSubClassInfo*)::GetWindowLongPtr(hCtrl, GWLP_USERDATA);
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
	m_pcp->m_exstyle	= WS_EX_TRANSPARENT;
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
			= (WNDPROC)::SetWindowLongPtr(hCtrl,
										  GWLP_WNDPROC,
										  (LONG_PTR)ChildCtrlProc);
		::SetWindowLongPtr(hCtrl, GWLP_USERDATA, (LONG_PTR)pCCSci);
	}
	return TRUE;
}

BOOL
RadioCtrl::enableCtrl(BOOL bEnable, BOOL bChange)
{
	if (HasListCtrl::enableCtrl(bEnable,bChange)) {
		HWND hDlg = m_pDlgPage->gethwndPage();
		if (hDlg != NULL) {
			BOOL bActualState = bChange ? bEnable : (m_bEnable && bEnable);
			int num = m_item->itemNum();
			for (int i = 1; i <= num; i++) {
				::EnableWindow(::GetDlgItem(hDlg, m_pcp->m_id + i),
								bActualState);
			}
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
//		::SetWindowText(hwndBtn,id->getText());
		::MySetWindowText(hwndBtn, id->getText());
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
	int num = m_item->initSequentialGet();
	ItemData* id;
	for (int i = 1; i <= num; i++) {
		id = m_item->getNextItem();
		StringBuffer& buf = id->getText();
		hwndBtn = ::GetDlgItem(hDlg, m_pcp->m_id + i);
//		int len = ::SendMessage(hwndBtn, WM_GETTEXTLENGTH, 0, 0);
//		buf.resize(len);
//		::GetWindowText(hwndBtn, buf.getBufPtr(), len + 1);
//		if (buf.length() != len) return FALSE;
		if (!::MyGetWindowText(hwndBtn, buf)) {
			return FALSE;
		}
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
//		::SetWindowText(::GetDlgItem(m_pDlgPage->gethwndPage(),
//									m_pcp->m_id + ind + 1),
		MySetWindowText(::GetDlgItem(m_pDlgPage->gethwndPage(),
									 m_pcp->m_id + ind + 1),
						id->getText());
	}
	return TRUE;
}

BOOL
RadioCtrl::onInsertItem(CmdLineParser& text, const StringBuffer& pos)
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onInsertItem(text,pos) != 0;
}

BOOL
RadioCtrl::onDeleteItem(const StringBuffer& pos)
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onDeleteItem(pos) != 0;
}

BOOL
RadioCtrl::onResetList()
{
	if (m_pcp->m_hwndCtrl != NULL) return FALSE;
	return HasListCtrl::onResetList() != 0;
}

HBRUSH
RadioCtrl::onCtlColor(HWND hwndCtrl, UINT uMsg, HDC hDC)
{
	SimpleCtrl::onCtlColor(hwndCtrl, uMsg, hDC);
	return NULL;
}

StringBuffer
RadioCtrl::onGetString()
{
	this->receiveData();
	return m_pcp->m_text;
}

int
RadioCtrl::onSetFocusedItem(const StringBuffer& pos)
{
	return onSetState(RealCmdLineParser(pos));
}

StringBuffer
RadioCtrl::onGetFocusedItem()
{
	return onGetState();
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
	m_pcp->m_style		= LBS_NOINTEGRALHEIGHT|LBS_NOTIFY|LBS_SORT|
		WS_BORDER|WS_VSCROLL|WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= 0x0;
	m_pcp->m_id			= 0;
	m_pcp->m_bufsize	= 1;
	m_pcp->m_classname	= (LPSTR)0x83;

	m_bSorted = FALSE;
	m_style_sort = LBS_SORT;

	m_msg_addstr	= LB_ADDSTRING;
	m_msg_setstr	= LB_INSERTSTRING;
	m_msg_getstr	= LB_GETTEXT;
	m_msg_delstr	= LB_DELETESTRING;
	m_msg_delall	= LB_RESETCONTENT;
	m_msg_setsel	= LB_SETCURSEL;
	m_msg_getsel	= LB_GETCURSEL;
	m_msg_setdata	= LB_SETITEMDATA;
	m_msg_getdata	= LB_GETITEMDATA;
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
	int num = m_item->initSequentialGet(), ind = 0;
	while ((id = m_item->getNextItem()) != NULL) {
		int index;
		if (m_bSorted) {
			index = ::SendMessage(m_pcp->m_hwndCtrl, m_msg_addstr,
								  0, (LPARAM)id->getText().getBufPtr());
		} else {
			index = ::SendMessage(m_pcp->m_hwndCtrl, m_msg_setstr,
								  ind++, (LPARAM)id->getText().getBufPtr());
		}
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setdata,
						index, (LPARAM)id);
	}
	this->setViewIndex();
	if (m_state > num) m_state = 0;
	if (m_state > 0) {
		id = m_item->getItemByIndex(m_state - 1);
		if (!id) return FALSE;
		int index = id->getViewIndex();
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setsel, index, 0);
		::SendMessage(m_pcp->m_hwndCtrl, LB_SETTOPINDEX, index, 0);
	}
	return TRUE;
}

void
ListCtrl::setViewIndex()
{
	int num = m_item->itemNum();
	for (int i = 0; i < num; i++) {
		ItemData* id = (ItemData*)::SendMessage(m_pcp->m_hwndCtrl, m_msg_getdata,
												i, 0);
		id->setViewIndex(i);
	}
}

BOOL
ListCtrl::sendData()
{
	if (!SimpleCtrl::sendData()) return FALSE;
	if (m_state > 0) {
		ItemData* id = m_item->getItemByIndex(m_state - 1);
		if (!id) return FALSE;
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setsel, id->getViewIndex(), 0);
	} else {
		// 未選択状態にする
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setsel, -1, 0);
	}
	return TRUE;
}

BOOL
ListCtrl::receiveData()
{
	if (!SimpleCtrl::receiveData()) return FALSE;
	this->setViewIndex();
	this->getStateFromView();
	return TRUE;
}

BOOL
ListCtrl::onSetSort(CmdLineParser& sstate)
{
	if (m_pcp->m_hwndCtrl) return FALSE;
	if (sstate.itemNum() > 0 &&
		sstate.getArgvStr(0).compareTo("yes") == 0) {
		m_bSorted = TRUE;
	} else {
		m_bSorted = FALSE;
	}
	return TRUE;
}

StringBuffer
ListCtrl::onGetSort()
{
	return m_bSorted ? "yes" : "no";
}

BOOL
ListCtrl::onSetItem(CmdLineParser& text, const StringBuffer& pos)
{
	int	ind = HasListCtrl::onSetItem(text,pos) - 1;
	if (ind < 0) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		int vindex = id->getViewIndex();
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_delstr, vindex, 0);
		if (m_bSorted) {
			vindex = ::SendMessage(m_pcp->m_hwndCtrl, m_msg_addstr,
								   0, (LPARAM)id->getText().getBufPtr());
		} else {
			vindex = ::SendMessage(m_pcp->m_hwndCtrl, m_msg_setstr,
								   vindex, (LPARAM)id->getText().getBufPtr());
		}
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setdata,
					  vindex, (LPARAM)id);
		this->setViewIndex();
		this->sendData();
	}
	return TRUE;
}

BOOL
ListCtrl::onInsertItem(CmdLineParser& text, const StringBuffer& pos)
{
	this->receiveData();
	int	ind = -1,
		num = m_item->itemNum();
	if (pos.length() > 0) {
		int	tmp = ival(pos) - 1;
		if (tmp >= 0 && tmp < num) ind = tmp;
	}
	m_item->addItem(new ItemData(RemoveQuote(text.getRawData())), ind);
	if (ind < 0) ind = num;
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		int index;
		if (m_bSorted) {
			index = ::SendMessage(m_pcp->m_hwndCtrl, m_msg_addstr,
								  0, (LPARAM)id->getText().getBufPtr());
		} else {
			index = ::SendMessage(m_pcp->m_hwndCtrl, m_msg_setstr,
								  ind, (LPARAM)id->getText().getBufPtr());
		}
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setdata, index, (LPARAM)id);
		this->setViewIndex();
		this->getStateFromView();
	}
	return TRUE;
}

BOOL
ListCtrl::onDeleteItem(const StringBuffer& pos)
{
	this->receiveData();
	int	ind = this->getItemData(pos);
	if (ind < 0) return FALSE;
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		int vindex = id->getViewIndex();
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_delstr, vindex, 0);
	}
	m_item->delItemByIndex(ind);
	if (m_pcp->m_hwndCtrl != NULL) {
		this->setViewIndex();
		this->getStateFromView();
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

int
ListCtrl::onSetFocusedItem(const StringBuffer& pos)
{
	return onSetState(RealCmdLineParser(pos));
}

StringBuffer
ListCtrl::onGetFocusedItem()
{
	return onGetState();
}

WORD
ListCtrl::onCommand(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam)) {
	case LBN_DBLCLK: return m_notify[0];
	case LBN_SELCHANGE:
		if (m_state > 0) {
			int sel = ::SendMessage(m_pcp->m_hwndCtrl, LB_GETCURSEL, 0, 0);
//			::SendMessage(m_pcp->m_hwndCtrl, LB_SETTOPINDEX, (WPARAM)sel, 0);
		}
		return m_notify[1];
	}
	return 0xFFFF;
}

void
ListCtrl::getStateFromView()
{
	if (!m_pcp->m_hwndCtrl) return;
	DWORD i = (DWORD)::SendMessage(m_pcp->m_hwndCtrl, m_msg_getsel, 0, 0);
	if (i == m_msg_err) {
		m_state = 0;
		m_pcp->m_text = nullStr;
	} else {
		ItemData* id = (ItemData*)::SendMessage(m_pcp->m_hwndCtrl, m_msg_getdata,
												i, 0);
		m_state = m_item->getItemIndexByPtr(id) + 1;
		m_pcp->m_text = id->getText();
	}
}

StringBuffer
ListCtrl::onGetCtrlExProperty(const StringBuffer& key)
{
	if (key.compareTo("view_order_of_", TRUE, 14) == 0)
	{
		int	index = strtol((LPCSTR)key + 14, NULL, 10),
			itemNum = m_item->itemNum();
		if (index < 0 || index >= itemNum)
		{
			return nullStr;
		}

		ItemData* id = m_item->getItemByIndex(index);
		if (id == NULL)
		{
			return nullStr;
		}

		int	viewIndex = id->getViewIndex();

		StringBuffer	ret;

		ret.append(viewIndex);

		return ret;
	}
	else if (key.compareTo("index_of_", TRUE, 9) == 0)
	{
		int	viewIndex = strtol((LPCSTR)key + 9, NULL, 10),
			itemNum = m_item->itemNum();
		if (viewIndex < 0 || viewIndex >= itemNum)
		{
			return nullStr;
		}

		ItemData* id = NULL;

		if (m_pcp->m_hwndCtrl != NULL)
		{
			id = (ItemData*)::SendMessage(m_pcp->m_hwndCtrl, m_msg_getdata,
										  viewIndex, 0);
		}
		else
		{
			int	num = m_item->initSequentialGet();	//	skip dummy
			for (int i = 0; i < num; i++)
			{
				ItemData*	idTemp = m_item->getItemByIndex(i);
				if (viewIndex == idTemp->getViewIndex())
				{
					id = idTemp;
					break;
				}
			}
		}

		if (id == NULL)
		{
			return nullStr;
		}

		int	index = m_item->getItemIndexByPtr(id);
		if (index == -1)
		{
			return nullStr;
		}

		StringBuffer	ret;

		ret.append(index);

		return ret;
	}
	else
	{
		return SimpleCtrl::onGetCtrlExProperty(key);
	}
}

BOOL
ListCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!HasListCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_bSorted, GetString(STR_DLGDATA_SORT), nullStr);
	return TRUE;
}

BOOL
ListCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_bSorted, GetString(STR_DLGDATA_SORT), nullStr);
	return HasListCtrl::loadData(ddfile);
}


static LRESULT CALLBACK
ComboChildCtrlProc(HWND hCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ChildCtrlSubClassInfo*
		pCCSci = (ChildCtrlSubClassInfo*)::GetWindowLongPtr(hCtrl, GWLP_USERDATA);
	HWND hwndCombo = pCCSci->m_pCtrl->getCtrlHWND();
	switch (uMsg) {
	case WM_GET_CTRL_PTR:
		return (LRESULT)pCCSci->m_pCtrl;
	case WM_KEYDOWN:
		if (wParam == VK_DOWN &&
			!::SendMessage(hwndCombo, CB_GETDROPPEDSTATE, 0, 0)) {
			::SendMessage(hwndCombo, CB_SHOWDROPDOWN, TRUE, 0);
			int cursel = ::SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
			if (cursel == CB_ERR) cursel = 0;
			::SendMessage(hwndCombo, CB_SETCURSEL, cursel, 0);
			return 0;
		}
		break;
	case WM_CHAR:
		if (wParam == '\r' &&
			::SendMessage(hwndCombo, CB_GETDROPPEDSTATE, 0, 0)) {
			// ドロップダウン表示中にエンターキーを押されたとき、
			// エディットボックスの内容がクリアされないように自前で処理を行う
			::SendMessage(hwndCombo, CB_SHOWDROPDOWN, FALSE, 0);
			return 0;
		}
		break;
	case WM_DESTROY:
		delete pCCSci;
		// fall down
	default:
		break;
	}

	if (uMsg == WM_SETTEXT)
	{
		DEBUG_OUTPUT(("combo settext: %s", (LPCSTR)lParam));
	}

	return ::CallWindowProc(pCCSci->m_pfnDefCallback, hCtrl, uMsg, wParam, lParam);
}

//	combo
ComboCtrl::ComboCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: ListCtrl(name,text,type),
	  m_imestate(0L),
	  m_bEditable(type == CTRLID_COMBO),
	  m_bAlreadyFocused(FALSE)
{
	m_pcp->m_style		= CBS_AUTOHSCROLL|CBS_SORT|WS_BORDER|WS_VSCROLL|
							WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VISIBLE|WS_GROUP|
							(m_bEditable ? CBS_DROPDOWN : CBS_DROPDOWNLIST);
	m_pcp->m_classname	= (LPSTR)0x85;

	m_style_sort = CBS_SORT;

	m_msg_addstr	= CB_ADDSTRING;
	m_msg_setstr	= CB_INSERTSTRING;
	m_msg_getstr	= CB_GETLBTEXT;
	m_msg_delstr	= CB_DELETESTRING;
	m_msg_delall	= CB_RESETCONTENT;
	m_msg_setsel	= CB_SETCURSEL;
	m_msg_getsel	= CB_GETCURSEL;
	m_msg_setdata	= CB_SETITEMDATA;
	m_msg_getdata	= CB_GETITEMDATA;
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
	if (m_height > 0) {
		cta.m_cy = (WORD)(8 * m_height + 2);
	} else {
		cta.m_cy = (WORD)(8 * (m_item->itemNum() + 3) + 2);
	}
	m_pcp->setCtrlTemplate(cta);
	return TRUE;
}

BOOL
ComboCtrl::initCtrl(HWND hDlg)
{
	m_bAlreadyFocused = FALSE;
	if (!ListCtrl::initCtrl(hDlg)) return FALSE;
	ChildCtrlSubClassInfo*
		pCCSci = new ChildCtrlSubClassInfo();
	pCCSci->m_pCtrl = this;
	if (m_bEditable) {
		// コンボボックス
		HWND hwndEdit = ::GetTopWindow(m_pcp->m_hwndCtrl);
		pCCSci->m_pfnDefCallback = (WNDPROC)::GetWindowLongPtr(hwndEdit, GWLP_WNDPROC);
		::SetWindowLongPtr(hwndEdit, GWLP_USERDATA, (LONG_PTR)pCCSci);
		::SetWindowLongPtr(hwndEdit, GWLP_WNDPROC, (LONG_PTR)ComboChildCtrlProc);
	} else {
		// ドロップダウンリスト
#if 0
		// dispatchRawMsg() で対処
		::SetWindowLongPtr(m_pcp->m_hwndCtrl, GWLP_USERDATA, (LONG_PTR)pCCSci);
		pCCSci->m_pfnDefCallback
			= (WNDPROC)::SetWindowLongPtr(m_pcp->m_hwndCtrl,
										  GWLP_WNDPROC,
										  (LONG_PTR)ComboChildCtrlProc);
#endif
	}
//	::SetWindowText(m_pcp->m_hwndCtrl, m_pcp->m_text);
	m_pcp->setText();
	if (m_height > 0) {
		::SendMessage(m_pcp->m_hwndCtrl, CB_SETMINVISIBLE, m_height, 0);
	}
	return TRUE;
}

BOOL
ComboCtrl::sendData()
{
	if (!SimpleCtrl::sendData()) return FALSE;
	if (m_state > 0) {
		ItemData* id = m_item->getItemByIndex(m_state - 1);
		if (!id) return FALSE;
		::SendMessage(m_pcp->m_hwndCtrl, m_msg_setsel, id->getViewIndex(), 0);
	}
	return TRUE;
}

BOOL
ComboCtrl::receiveData()
{
	if (!SimpleCtrl::receiveData()) return FALSE;
	this->setViewIndex();
	this->getStateFromView();
	return TRUE;
}

BOOL
ComboCtrl::onSetString(const StringBuffer& str)
{
	SimpleCtrl::receiveData();
//	if (m_state > 0) return FALSE;
	return SimpleCtrl::onSetString(str);
}

BOOL
ComboCtrl::onSetImeState(int state)
{
	m_imestate = state;
	m_bAlreadyFocused = FALSE;
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
			DlgFrame& dlgFrame = m_pDlgPage->getDlgFrame();
			int nState = dlgFrame.getImeState();
			if (nState > 2 && dlgFrame.isImeAlreadyFocused()) {
				// 既にいずれかのコントロールがフォーカスを得ている
				nState = 0;
			}
			if (m_imestate > 0 || nState > 0) {
				// グローバル設定をローカル設定で上書き
				if (m_imestate > 0) nState = m_imestate;
				HIMC hImc = ::ImmGetContext(hwndEdit);
				switch (nState) {
				case 3:
					if (m_bAlreadyFocused) break;
					// through down
				case 1:
					if (!::ImmGetOpenStatus(hImc))
						::ImmSetOpenStatus(hImc, TRUE);
					break;
				case 4:
					if (m_bAlreadyFocused) break;
					// through down
				case 2:
					if (::ImmGetOpenStatus(hImc))
						::ImmSetOpenStatus(hImc, FALSE);
					break;
				default:
					break;
				}
				::ImmReleaseContext(hwndEdit, hImc);
			}
			::SendMessage(hwndEdit, EM_SETSEL, 0, -1);
			m_bAlreadyFocused = TRUE;
			dlgFrame.setImeAlreadyFocused();
		}
		break;
	}
	return 0xFFFF;
}

BOOL
ComboCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!ListCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_imestate, GetString(STR_DLGDATA_IMESTATE), nullStr);
	ddfile.write(m_bEditable, GetString(STR_DLGDATA_EDITABLE), nullStr);
	return TRUE;
}

BOOL
ComboCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_imestate, GetString(STR_DLGDATA_IMESTATE), nullStr);
	ddfile.read(&m_bEditable, GetString(STR_DLGDATA_EDITABLE), nullStr);
	m_pcp->m_style = (m_pcp->m_style & ~(CBS_DROPDOWN | CBS_DROPDOWNLIST)) |
					 (m_bEditable ? CBS_DROPDOWN : CBS_DROPDOWNLIST);
	return ListCtrl::loadData(ddfile);
}

void
ComboCtrl::getStateFromView()
{
	DWORD i = (DWORD)::SendMessage(m_pcp->m_hwndCtrl, m_msg_getsel, 0, 0);
	if (i == m_msg_err) {
		m_state = 0;
	} else {
		ItemData* id = (ItemData*)::SendMessage(m_pcp->m_hwndCtrl, m_msg_getdata,
												i, 0);
		m_state = m_item->getItemIndexByPtr(id) + 1;
	}
}

LRESULT
ComboCtrl::dispatchRawMsg(CtrlProperty* pCProp, HWND hCtrl, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ComboCtrl* pCCtrl = dynamic_cast<ComboCtrl*>(pCProp->m_pCtrl);
	if (pCCtrl && !pCCtrl->m_bEditable) {
		HWND hwndCombo = pCCtrl->getCtrlHWND();
		if (uMsg == WM_KEYDOWN && wParam == VK_DOWN &&
			!::SendMessage(hwndCombo, CB_GETDROPPEDSTATE, 0, 0)) {
			::SendMessage(hwndCombo, CB_SHOWDROPDOWN, TRUE, 0);
			int cursel = ::SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
			if (cursel == CB_ERR) cursel = 0;
			::SendMessage(hwndCombo, CB_SETCURSEL, cursel, 0);
			return 0;
		}
	}
	return ListCtrl::dispatchRawMsg(pCProp, hCtrl, uMsg, wParam, lParam);
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

WORD
RefBtnCtrl::getDefID() const
{
//	DWORD dwStyle = ::GetWindowLong(m_pcp->m_hwndCtrl, GWL_STYLE);
//	if (dwStyle & BS_DEFPUSHBUTTON) return m_pcp->m_id;
	if (m_pcp->m_style & BS_DEFPUSHBUTTON) return m_pcp->m_id;
	return 0xFFFF;
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
										  sbTitle, sbIniDir,
										  (flag!=0) ? 0 : 1);
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
	ddfile.write(m_exbuffer, secname, nullStr);
	return TRUE;
}

BOOL
RefBtnCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(m_exbuffer, secname, nullStr);
	return HasListCtrl::loadData(ddfile);
}

ChkListCtrl::ChkListCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: HasListCtrl(name, text, type)
	, m_bSorted(FALSE)
{
	m_pcp->m_style		= LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_SINGLESEL|
							WS_CHILD|WS_BORDER|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle	= WS_EX_CLIENTEDGE | LVS_EX_CHECKBOXES;
	m_pcp->m_classname	= WC_LISTVIEW;
	m_pcp->m_bufsize	= lstrlen(m_pcp->m_classname) + 1;
}

BOOL
ChkListCtrl::createCtrlTemplate(CtrlTemplateArgs& cta)
{
	if (m_bSorted) m_pcp->m_style |= LVS_SORTASCENDING;
	else           m_pcp->m_style &= ~LVS_SORTASCENDING;
	return HasListCtrl::createCtrlTemplate(cta);
}

BOOL
ChkListCtrl::initCtrl(HWND hDlg)
{
	if (!HasListCtrl::initCtrl(hDlg)) return FALSE;
#if 0
	HANDLE hbmp = ::LoadImage(NULL, MAKEINTRESOURCE(OBM_CHECKBOXES),
								IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_SHARED);
	DWORD dlgbu = (DWORD)LOWORD(GetDialogBaseUnits(hDlg)) * 9 / 10;
	hbmp = ::CopyImage(hbmp, IMAGE_BITMAP, dlgbu*4, dlgbu*3, 0);
	HIMAGELIST hImage = ImageList_Create((int)dlgbu, (int)dlgbu, ILC_COLOR, 2, 1);
	ImageList_Add(hImage, (HBITMAP)hbmp, NULL);
	ListView_SetImageList(m_pcp->m_hwndCtrl, hImage, LVSIL_SMALL);
	::DeleteObject(hbmp);
#endif
	ListView_SetExtendedListViewStyleEx(m_pcp->m_hwndCtrl,
										LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES);
	LV_COLUMN lvc;
	lvc.mask		= LVCF_FMT|LVCF_SUBITEM;
	lvc.fmt			= LVCFMT_LEFT;
	lvc.iSubItem	= 0;
	if (ListView_InsertColumn(m_pcp->m_hwndCtrl, 0, &lvc) < 0) return FALSE;
	LVITEM	lvi;
	lvi.mask		= LVIF_TEXT | LVIF_PARAM /*|LVIF_IMAGE*/;
	lvi.iSubItem	= 0;
	ItemData* id;
	ListView_SetItemCount(m_pcp->m_hwndCtrl, m_item->initSequentialGet());
	for (int i = 0; (id = m_item->getNextItem()) != NULL; i++) {
		lvi.iItem	= i;
		lvi.lParam  = (LPARAM)id;
		lvi.pszText = id->getText().getBufPtr();
//		lvi.iImage	= 0;
		ListView_InsertItem(m_pcp->m_hwndCtrl, &lvi);
	}
	this->setViewIndex();
	ListView_SetColumnWidth(m_pcp->m_hwndCtrl, 0, LVSCW_AUTOSIZE);

	ListView_SetTextColor(m_pcp->m_hwndCtrl, m_pcp->m_fontprop.m_color);

	return TRUE;
}

BOOL
ChkListCtrl::sendData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	if (m_pcp->m_fontprop.m_bchanged)
		m_pcp->changeFont();
	if (m_state > 0 && !m_states.getState(m_state-1))
		m_states.setState(m_state-1, TRUE);
	int nFocused = m_strFocusedItem.length() > 0 ? ival(m_strFocusedItem) - 1 : -1;
	LVITEM	lvi;
	lvi.mask	  = LVIF_STATE;
	lvi.iSubItem  = 0;
	lvi.stateMask = 0xFFFF;
	int	num = m_item->initSequentialGet();	//	skip dummy
	for (int i = 0; i < num; i++) {
		ItemData* id = m_item->getItemByIndex(i);
		lvi.iItem = id->getViewIndex();

		if (m_states.getState(i)) {
			lvi.state = 0x2000;
		} else {
			lvi.state = 0x1000;
		}

		if (i == nFocused) {
			lvi.state |= LVIS_FOCUSED;
		}

		ListView_SetItem(m_pcp->m_hwndCtrl, &lvi);
	}
	ListView_SetColumnWidth(m_pcp->m_hwndCtrl, 0, LVSCW_AUTOSIZE);
	ListView_RedrawItems(m_pcp->m_hwndCtrl, 0, num);
	::UpdateWindow(m_pcp->m_hwndCtrl);
	return TRUE;
}

BOOL
ChkListCtrl::receiveData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	m_strFocusedItem = nullStr;
	LVITEM	lvi;
	lvi.mask	  = LVIF_STATE | LVIF_PARAM;
	lvi.iSubItem  = 0;
	lvi.stateMask = 0xFFFF;
	int	num = m_item->itemNum();
	for (int i = 0; i < num; i++) {
		lvi.iItem = i;
		ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);
		ItemData* id = (ItemData*)lvi.lParam;
		id->setViewIndex(i);
		int pos = m_item->getItemIndexByPtr(id);
		m_states.setState(pos, (lvi.state & 0xF000) == 0x2000);
		if (lvi.state & LVIS_FOCUSED) {
			m_strFocusedItem.reset();
			m_strFocusedItem.append(pos + 1);
		}
	}
	m_state = m_states.getFirstIndex(num) + 1;
	return TRUE;
}

void
ChkListCtrl::setViewIndex()
{
	if (!m_pcp->m_hwndCtrl) return;
	int num = m_item->itemNum();
	LVITEM lvi;
	lvi.mask = LVIF_PARAM;
	lvi.iSubItem = 0;
	for (int i = 0; i < num; i++) {
		lvi.iItem = i;
		ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);
		((ItemData*)lvi.lParam)->setViewIndex(i);
	}
}

void
ChkListCtrl::getStateFromView()
{
	if (!m_pcp->m_hwndCtrl) return;
	LVITEM lvi;
	lvi.mask = LVIF_STATE | LVIF_PARAM;
	lvi.stateMask = 0xF000;
	lvi.iSubItem = 0;
	int num = m_item->itemNum();
	for (int i = 0; i < num; i++) {
		lvi.iItem = i;
		ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);
		int pos = m_item->getItemIndexByPtr((ItemData*)lvi.lParam);
		m_states.setState(pos, (lvi.state & 0xF000) == 0x2000);
	}
	m_state = m_states.getFirstIndex(num) + 1;
}

BOOL
ChkListCtrl::onSetSort(CmdLineParser& sstate)
{
	if (m_pcp->m_hwndCtrl) return FALSE;
	if (sstate.itemNum() > 0 &&
		sstate.getArgvStr(0).compareTo("yes") == 0) {
		m_bSorted = TRUE;
	} else {
		m_bSorted = FALSE;
	}
	return TRUE;
}

StringBuffer
ChkListCtrl::onGetSort()
{
	return m_bSorted ? "yes" : "no";
}

BOOL
ChkListCtrl::onSetState(CmdLineParser& state)
{
	m_states.reset();
	state.initSequentialGet();
	int num = m_item->itemNum();
	LPCSTR av;
	while ((av = state.getNextArgv()) != NULL) {
		int idx = ival(av) - 1;
		if (idx < 0) {
			m_states.reset();
		} else if (idx < num) {
			m_states.setState(idx, TRUE);
		}
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
		LVITEM	lvi;
		lvi.mask		= LVIF_TEXT;
		lvi.iItem		= id->getViewIndex();
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
		LVITEM	lvi;
		lvi.mask		= LVIF_TEXT | LVIF_PARAM;
		lvi.iItem		= ind;
		lvi.iSubItem	= 0;
		lvi.lParam		= (LPARAM)id;
		lvi.pszText		= id->getText().getBufPtr();
		ListView_InsertItem(m_pcp->m_hwndCtrl, &lvi);
		this->setViewIndex();
		this->getStateFromView();
		ListView_SetColumnWidth(m_pcp->m_hwndCtrl, 0, LVSCW_AUTOSIZE);
	}
	return TRUE;
}

BOOL
ChkListCtrl::onDeleteItem(const StringBuffer& pos)
{
	this->receiveData();
	int	ind = this->getItemData(pos);
	if (ind < 0) return FALSE;
	int	num = m_item->itemNum();
	if (ind < num) {
		for (int tmp = ind; tmp < num; tmp++) {
			m_states.setState(tmp, m_states.getState(tmp + 1));
		}
		m_state = m_states.getFirstIndex(num) + 1;
	}
	if (m_pcp->m_hwndCtrl != NULL) {
		ItemData* id = m_item->getItemByIndex(ind);
		ListView_DeleteItem(m_pcp->m_hwndCtrl, id->getViewIndex());
	}
	m_item->delItemByIndex(ind);
	this->setViewIndex();
	this->getStateFromView();
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

BOOL
ChkListCtrl::onSetCtrlFont(CmdLineParser& argv)
{
	if (argv.itemNum() < 1) return FALSE;
	argv.initSequentialGet();
	const StringBuffer& fface = argv.getNextArgvStr();
	const StringBuffer& color = argv.getNextArgvStr();
	const StringBuffer& fname = argv.getNextArgvStr();
	this->setFont(fface, color, fname);
	ListView_SetTextColor(m_pcp->m_hwndCtrl, m_pcp->m_fontprop.m_color);
	this->sendData();
	return TRUE;
}

int
ChkListCtrl::onSetFocusedItem(const StringBuffer& item)
{
	m_strFocusedItem = item;
	return this->sendData();
}

StringBuffer
ChkListCtrl::onGetFocusedItem()
{
	this->receiveData();
	return m_strFocusedItem;
}

StringBuffer
ChkListCtrl::onGetCtrlExProperty(const StringBuffer& key)
{
	if (key.compareTo("view_order_of_", TRUE, 14) == 0)
	{
		int	index = strtol((LPCSTR)key + 14, NULL, 10),
			itemNum = m_item->itemNum();
		if (index < 0 || index >= itemNum)
		{
			return nullStr;
		}

		ItemData* id = m_item->getItemByIndex(index);
		if (id == NULL)
		{
			return nullStr;
		}

		int	viewIndex = id->getViewIndex();

		StringBuffer	ret;

		ret.append(viewIndex);

		return ret;
	}
	else if (key.compareTo("index_of_", TRUE, 9) == 0)
	{
		int	viewIndex = strtol((LPCSTR)key + 9, NULL, 10),
			itemNum = m_item->itemNum();
		if (viewIndex < 0 || viewIndex >= itemNum)
		{
			return nullStr;
		}

		ItemData* id = NULL;

		if (m_pcp->m_hwndCtrl != NULL)
		{
			LVITEM	lvi;
			memset(&lvi, 0, sizeof(lvi));

			lvi.iItem = viewIndex;
			lvi.mask = LVIF_PARAM;
			ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);

			id = (ItemData*)lvi.lParam;
		}
		else
		{
			int	num = m_item->initSequentialGet();	//	skip dummy
			for (int i = 0; i < num; i++)
			{
				ItemData*	idTemp = m_item->getItemByIndex(i);
				if (viewIndex == idTemp->getViewIndex())
				{
					id = idTemp;
					break;
				}
			}
		}

		if (id == NULL)
		{
			return nullStr;
		}

		int	index = m_item->getItemIndexByPtr(id);
		if (index == -1)
		{
			return nullStr;
		}

		StringBuffer	ret;

		ret.append(index);

		return ret;
	}
	else
	{
		return SimpleCtrl::onGetCtrlExProperty(key);
	}
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
#if 0
			NM_LISTVIEW	*nmlv = reinterpret_cast<NM_LISTVIEW*>(lParam);
			LVITEM	lvi;
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
#endif
		}
		return m_notify[1];
#if 0
	case LVN_KEYDOWN:
		{
			NMLVKEYDOWN* nmlvkey = reinterpret_cast<NMLVKEYDOWN*>(lParam);
			if (nmlvkey->wVKey == VK_SPACE) {
				HWND hwndCtrl = m_pcp->m_hwndCtrl;
				int num = ListView_GetItemCount(hwndCtrl);
				if (num <= 0) break;
				LVITEM lvi;
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
#endif
	case LVN_ENDLABELEDIT:
		return m_notify[2];
	case LVN_SETDISPINFO:
		{
			LVITEM& lvi = (reinterpret_cast<NMLVDISPINFO*>(lParam))->item;
			ItemData* id = m_item->getItemByIndex(lvi.iItem);
			if (id != NULL) id->getText().reset(lvi.pszText);
		}
		return 0xFFFF;
	case LVN_GETDISPINFO:
		{
			LVITEM& lvi = (reinterpret_cast<NMLVDISPINFO*>(lParam))->item;
			ItemData* id = m_item->getItemByIndex(lvi.iItem);
			if (id == NULL) break;
			lvi.pszText = id->getText().getBufPtr();
			lvi.mask |= LVIF_DI_SETITEM;
		}
		return 0xFFFF;
	case LVN_DELETEALLITEMS:
		::SetWindowLongPtr(m_pDlgPage->gethwndPage(),DWLP_MSGRESULT,(LONG_PTR)TRUE);
	}
	return 0xFFFF;
}

BOOL
ChkListCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!HasListCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_bSorted, GetString(STR_DLGDATA_SORT), nullStr);
	return m_states.dumpData(ddfile);
}

BOOL
ChkListCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_bSorted, GetString(STR_DLGDATA_SORT), nullStr);
	if (!m_states.loadData(ddfile)) return FALSE;
	return HasListCtrl::loadData(ddfile);
}

const StringBuffer sep = ":";

LViewCtrl::LViewCtrl(
	const StringBuffer& name,
	const StringBuffer& text)
	: ChkListCtrl(name, text, CTRLID_LVIEW)
{
	m_pcp->m_style = LVS_REPORT|LVS_SHOWSELALWAYS|//LVS_NOSORTHEADER|
						WS_CHILD|WS_BORDER|WS_TABSTOP|WS_VISIBLE|WS_GROUP;
	m_pcp->m_exstyle = LVS_EX_FULLROWSELECT;

	m_bSorted = FALSE;

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
	LVITEM& lvi,
	LViewItemData* lvid,
	HWND hwndCtrl,
	UINT msg, WPARAM wParam)
{
	lvi.pszText = lvid->getItemByIndex(lvi.iSubItem)->getText().getBufPtr();
	if (lvi.iSubItem == 0) {
		lvi.lParam = (LPARAM)lvid;
		lvi.mask |= LVIF_PARAM;
	}
	::SendMessage(hwndCtrl, msg, wParam, (LPARAM)&lvi);
	int cwidth = ListView_GetStringWidth(hwndCtrl, lvi.pszText) + 
					(lvi.iSubItem ? 12 : 10);
	if (cwidth > ListView_GetColumnWidth(hwndCtrl, lvi.iSubItem))
		ListView_SetColumnWidth(hwndCtrl, lvi.iSubItem, cwidth);
}

struct SortParam {
	int m_nColumn;
	BOOL m_bAscending;
};

static int CALLBACK
CompareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SortParam* pSp = (SortParam*)lParamSort;
	LViewItemData* lvid1 = (LViewItemData*)lParam1;
	LViewItemData* lvid2 = (LViewItemData*)lParam2;
	ItemData* id1 = lvid1->getItemByIndex(pSp->m_nColumn);
	ItemData* id2 = lvid2->getItemByIndex(pSp->m_nColumn);
	int sgn = pSp->m_bAscending ? 1 : -1;
	return sgn * lstrcmp(id1->getText(), id2->getText());
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
		StringBuffer text(nullStr);
		if (m_hdr) {
			ItemData* id = m_hdr->getItemByIndex(i);
			text = id->getText();
			if (i == m_lastSortKey) {
				StringBuffer tmp(m_bAscending ? "▲" : "▼");
				tmp.append(text);
				text = tmp;
			}
			lvc.pszText = text.getBufPtr();
			lvc.cx += ListView_GetStringWidth(m_pcp->m_hwndCtrl, lvc.pszText);
		}
		if (ListView_InsertColumn(m_pcp->m_hwndCtrl, i, &lvc) < 0)
			return FALSE;
	}
	LVITEM	lvi;
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

	if (m_bSorted && m_lastSortKey >= 0 && m_lastSortKey < m_colnum) {
		SortParam sp;
		sp.m_nColumn = m_lastSortKey;
		sp.m_bAscending = m_bAscending;
		::SendMessage(m_pcp->m_hwndCtrl, LVM_SORTITEMS,
					  (WPARAM)&sp, (LPARAM)CompareItem);
	}

	this->setViewIndex();

//	m_lastSortKey = -1;
//	m_bAscending = TRUE;
	ListView_SetExtendedListViewStyleEx(m_pcp->m_hwndCtrl,
										LVS_EX_FULLROWSELECT,
										LVS_EX_FULLROWSELECT);
	ListView_SetTextColor(m_pcp->m_hwndCtrl, m_pcp->m_fontprop.m_color);

	return TRUE;
}

BOOL
LViewCtrl::sendData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	if (m_pcp->m_fontprop.m_bchanged)
		m_pcp->changeFont();
	if (m_state > 0 && !m_states.getState(m_state-1))
		m_states.setState(m_state-1, TRUE);
	int nFocused = m_strFocusedItem.length() > 0 ? ival(m_strFocusedItem) - 1 : -1;
	LVITEM	lvi;
	lvi.mask		= LVIF_STATE;
	lvi.stateMask	= LVIS_SELECTED | LVIS_FOCUSED;
	lvi.iSubItem	= 0;
	int	num = m_item->itemNum();
	for (int i = 0; i < num; i++) {
		LViewItemData*
			lvid = static_cast<LViewItemData*>(m_item->getItemByIndex(i));
		lvi.iItem = lvid->getViewIndex();
		lvi.state = m_states.getState(i) ? LVIS_SELECTED : 0;
		if (i == nFocused) {
			lvi.state |= LVIS_FOCUSED;
		}
		ListView_SetItem(m_pcp->m_hwndCtrl, &lvi);
	}
	ListView_RedrawItems(m_pcp->m_hwndCtrl, 0, num);
	::UpdateWindow(m_pcp->m_hwndCtrl);
	return TRUE;
}

BOOL
LViewCtrl::receiveData()
{
	if (m_pcp->m_hwndCtrl == NULL) return FALSE;
	m_strFocusedItem = nullStr;
	LVITEM	lvi;
	lvi.mask		= LVIF_STATE | LVIF_PARAM;
	lvi.stateMask	= LVIS_SELECTED | LVIS_FOCUSED;
	lvi.iSubItem	= 0;
	int	num = m_item->itemNum();
	for (int i = 0; i < num; i++) {
		lvi.iItem = i;
		ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);
		LViewItemData* lvid = (LViewItemData*)lvi.lParam;
		lvid->setViewIndex(i);
		int pos = m_item->getItemIndexByPtr(lvid);
		m_states.setState(pos, (lvi.state & LVIS_SELECTED) != 0);
		if (lvi.state & LVIS_FOCUSED) {
			m_strFocusedItem.reset();
			m_strFocusedItem.append(pos + 1);
		}
	}
	m_state = m_states.getFirstIndex(num) + 1;
	return TRUE;
}

void
LViewCtrl::getStateFromView()
{
	if (!m_pcp->m_hwndCtrl) return;
	LVITEM lvi;
	lvi.mask = LVIF_STATE | LVIF_PARAM;
	lvi.stateMask = LVIS_SELECTED;
	lvi.iSubItem = 0;
	int num = m_item->itemNum();
	for (int i = 0; i < num; i++) {
		lvi.iItem = i;
		ListView_GetItem(m_pcp->m_hwndCtrl, &lvi);
		int pos = m_item->getItemIndexByPtr((LViewItemData*)lvi.lParam);
		m_states.setState(pos, (lvi.state & LVIS_SELECTED) != 0);
	}
	m_state = m_states.getFirstIndex(num) + 1;
}

BOOL
LViewCtrl::onSetSort(CmdLineParser& sstate)
{
	if (m_pcp->m_hwndCtrl) return FALSE;
	if (sstate.itemNum() > 0 &&
		sstate.getArgvStr(0).compareTo("yes") == 0) {
		m_bSorted = TRUE;
		if (sstate.itemNum() > 1) {
			m_lastSortKey = ival(sstate.getArgv(1)) - 1;
			if (sstate.itemNum() > 2) {
				m_bAscending = (sstate.getArgvStr(2).compareTo("a") == 0);
			} else {
				m_bAscending = TRUE;
			}
		} else {
			m_lastSortKey = 0;
			m_bAscending = TRUE;
		}
	} else {
		m_bSorted = FALSE;
		m_lastSortKey = -1;
		m_bAscending = TRUE;
	}
	return TRUE;
}

StringBuffer
LViewCtrl::onGetSort()
{
	if (m_bSorted) {
		StringBuffer ret("yes,");
		ret.append(m_lastSortKey + 1);
		ret.append(m_bAscending ? ",a" : ",d");
		return ret;
	} else {
		return "no";
	}
}

BOOL
LViewCtrl::onSetItem(CmdLineParser& argv, const StringBuffer& pos)
{
	this->receiveData();
	int	ind = m_state - 1, num = m_item->itemNum();
	if (ind < 0) ind = num - 1;
	if (pos.length() > 0) {
		int	tmp = ival(pos) - 1;
		switch (tmp) {
		case -3:
			return setHeader(argv);
			break;
		case -2:
			ind = num - 1;
			break;
		case -1:
			ind = m_state - 1;
			break;
		default:
			if (tmp >= 0 && tmp < num) ind = tmp;
			else ind = -1;
		}
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
	LVITEM	lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = lvid->getViewIndex();
	for (lvi.iSubItem = 0; lvi.iSubItem < (int)m_colnum; lvi.iSubItem++) {
		setlvitem(lvi, lvid, m_pcp->m_hwndCtrl, LVM_SETITEMTEXT, lvi.iItem);
	}

	if (m_bSorted && m_lastSortKey >= 0 && m_lastSortKey < m_colnum) {
		SortParam sp;
		sp.m_nColumn = m_lastSortKey;
		sp.m_bAscending = m_bAscending;
		::SendMessage(m_pcp->m_hwndCtrl, LVM_SORTITEMS,
					  (WPARAM)&sp, (LPARAM)CompareItem);
		this->setViewIndex();
		this->getStateFromView();
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
		m_state = m_states.getFirstIndex(num) + 1;
	} else ind = num;
	if (m_pcp->m_hwndCtrl == NULL) return TRUE;
	LVITEM	lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = ind;
	lvi.iSubItem = 0;
	setlvitem(lvi, lvid, m_pcp->m_hwndCtrl, LVM_INSERTITEM, 0);
	this->setViewIndex();
	lvi.iItem = lvid->getViewIndex();
	for (lvi.iSubItem = 1;
		lvi.iSubItem < (int)(DWORD)m_colnum;
		lvi.iSubItem++) {
		setlvitem(lvi, lvid, m_pcp->m_hwndCtrl,
					LVM_SETITEMTEXT,
					lvi.iItem);
	}

	if (m_bSorted && m_lastSortKey >= 0 && m_lastSortKey < m_colnum) {
		SortParam sp;
		sp.m_nColumn = m_lastSortKey;
		sp.m_bAscending = m_bAscending;
		::SendMessage(m_pcp->m_hwndCtrl, LVM_SORTITEMS,
					  (WPARAM)&sp, (LPARAM)CompareItem);
		this->setViewIndex();
	}
	this->getStateFromView();

	return TRUE;
}

StringBuffer
LViewCtrl::onGetItem(const StringBuffer& pos)
{
	this->receiveData();
	int	ind = m_state - 1, num = m_item->itemNum();
	if (pos.length() > 0) {
		int	tmp = ival(pos) - 1;
		switch (tmp) {
		case -3:
			{
				if (!m_hdr) return nullStr;
				StringBuffer ret(nullStr);
				int cnum = m_hdr->itemNum();
				for (int i = 0; i < cnum; i++) {
					if (ret.length() > 0) ret.append((TCHAR)',');
					ItemData* id = m_hdr->getItemByIndex(i);
					ret.append(id->getText());
				}
				return ret;
			}
			break;
		case -2:
			ind = num - 1;
			break;
		case -1:
			ind = m_state - 1;
			break;
		default:
			if (tmp >= 0 && tmp < num) ind = tmp;
			else ind = -1;
		}
	}
	if (ind < 0) return nullStr;
	LViewItemData*
		lvid = static_cast<LViewItemData*>(m_item->getItemByIndex(ind));
	if (lvid == NULL) return nullStr;
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
				LViewItemData*
					lvid = static_cast<LViewItemData*>(m_item->getItemByIndex(index));
				ListView_SetItemState(m_pcp->m_hwndCtrl,
									lvid->getViewIndex(),
									LVIS_FOCUSED,LVIS_FOCUSED);
			}
		}
		break;
	case LVN_ITEMCHANGED:
		return m_notify[1];
	case LVN_ENDLABELEDIT:
		return m_notify[2];
	case LVN_COLUMNCLICK:
		{
			int pos = reinterpret_cast<NMLISTVIEW*>(lParam)->iSubItem;
			if (m_bSorted) {
				SortParam sp;
				if (pos == m_lastSortKey) {
					m_bAscending = !m_bAscending;
				}
				m_lastSortKey = pos;
				sp.m_nColumn = pos;
				sp.m_bAscending = m_bAscending;
				::SendMessage(m_pcp->m_hwndCtrl, LVM_SORTITEMS,
							  (WPARAM)&sp, (LPARAM)CompareItem);
				this->setHeaderView();
			}
			pos += 2;
			if (pos >= sizeof(m_notify) / sizeof(m_notify[0])) break;
			return m_notify[pos];
		}
	case LVN_SETDISPINFO:
		if (((NMLVDISPINFO*)lParam)->item.mask&LVIF_TEXT) {
			LVITEM& lvi = (reinterpret_cast<NMLVDISPINFO*>(lParam))->item;
			LViewItemData* lvid = (LViewItemData*)lvi.lParam;
//				lvid = static_cast<LViewItemData*>(
//							m_item->getItemByIndex(lvi.iItem));
			if (lvid == NULL) break;
			ItemData* id = lvid->getItemByIndex(lvi.iSubItem);
			if (id == NULL) break;
			id->getText().reset(lvi.pszText);
		}
		break;
	case LVN_GETDISPINFO:
		if ((reinterpret_cast<NMLVDISPINFO*>(lParam))->item.mask&LVIF_TEXT) {
			LVITEM& lvi = (reinterpret_cast<NMLVDISPINFO*>(lParam))->item;
			LViewItemData* lvid = (LViewItemData*)lvi.lParam;
//				lvid = static_cast<LViewItemData*>(
//							m_item->getItemByIndex(lvi.iItem));
			if (lvid == NULL) break;
			ItemData* id = lvid->getItemByIndex(lvi.iSubItem);
			if (id == NULL) break;
			lvi.pszText = id->getText().getBufPtr();
			lvi.mask |= LVIF_DI_SETITEM;
		}
		break;
	case LVN_DELETEALLITEMS:
		::SetWindowLongPtr(m_pDlgPage->gethwndPage(), DWLP_MSGRESULT, (LONG_PTR)TRUE);
		break;
	}
	return 0xFFFF;
}

BOOL
LViewCtrl::setHeader(CmdLineParser& argv)
{
	if (!m_hdr) return FALSE;
	int num = m_hdr->itemNum();
	for (int i = 0; i < num; i++) {
		ItemData* id = m_hdr->getItemByIndex(i);
		const char* av = argv.getArgv(i);
		if (av) id->getText() = argv.getArgvStr(i);
	}
	this->setHeaderView();
	return TRUE;
}

void
LViewCtrl::setHeaderView()
{
	if (!m_hdr || !m_pcp->m_hwndCtrl) return;
	int num = m_hdr->itemNum();
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	for (int i = 0; i < num; i++) {
		lvc.iSubItem = i;
		ItemData* id = m_hdr->getItemByIndex(i);
		StringBuffer text(id->getText());
		if (i == m_lastSortKey) {
			StringBuffer tmp(m_bAscending ? "▲" : "▼");
			tmp.append(text);
			text = tmp;
		}
		lvc.pszText = text.getBufPtr();
		ListView_SetColumn(m_pcp->m_hwndCtrl, i, &lvc);
	}
}

BOOL
LViewCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!SimpleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_colnum, GetString(STR_DLGDATA_COLNUM), nullStr);
	m_states.dumpData(ddfile);
	ddfile.write((int)(m_hdr != 0), GetString(STR_DLGDATA_USEHEADER), nullStr);
	StringBuffer keybuf(GetString(STR_DLGDATA_HEADER), -1, 32);
	keybuf.append((TCHAR)'_');
	if (m_hdr) {
		m_hdr->initSequentialGet();
		for (int i = 0; i < (int)m_colnum; i++) {
			ItemData* id = m_hdr->getNextItem();
			if (id == NULL) break;
			char nbuf[16];
			itoa(i, nbuf, 10);
			ddfile.write(id->getText(), keybuf, nbuf, nullStr);
		}
	}
	keybuf.reset(GetString(STR_DLGDATA_ITEM));
	keybuf.append((TCHAR)'_');
	int	keylen = keybuf.length(), num = m_item->initSequentialGet();
	for (int i = 0; i < num; i++) {
		LViewItemData*
			lvid = static_cast<LViewItemData*>(m_item->getNextItem());
		if (lvid != NULL) continue;
		keybuf.setlength(keylen);
		keybuf.append(i).append((TCHAR)'_');
		int num2 = lvid->initSequentialGet();
		for (int j = 0; j < num2; j++) {
			ItemData* id = lvid->getNextItem();
			if (id != NULL) continue;
			char nbuf[16];
			itoa(j, nbuf, 10);
			ddfile.write(id->getText(), keybuf, nbuf, nullStr);
		}
	}
	ddfile.write(num, GetString(STR_DLGDATA_ITEMNUM), nullStr);
	return TRUE;
}

BOOL
LViewCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_colnum, GetString(STR_DLGDATA_COLNUM), nullStr);
	m_states.loadData(ddfile);
	int	num, usehdr;
	ddfile.read(&num, GetString(STR_DLGDATA_ITEMNUM), nullStr);
	ddfile.read(&usehdr, GetString(STR_DLGDATA_USEHEADER), nullStr);
	StringBuffer keybuf(GetString(STR_DLGDATA_HEADER), -1, 32);
	keybuf.append((TCHAR)'_');
	if (usehdr) {
		if (!m_hdr) m_hdr = new LViewItemData();
		for (int i = 0; i < (int)m_colnum; i++) {
			char nbuf[16];
			itoa(i, nbuf, 10);
			ItemData* id = new ItemData(nullStr);
			ddfile.read(id->getText(), keybuf, nbuf, nullStr);
			m_hdr->addItem(id);
		}
	}
	keybuf.reset(GetString(STR_DLGDATA_ITEM));
	keybuf.append((TCHAR)'_');
	int keylen = keybuf.length();
	for (int i = 0; i < num; i++) {
		LViewItemData* lvid = new LViewItemData();
		keybuf.setlength(keylen);
		keybuf.append(i).append((TCHAR)'_');
		for (int j = 0; j < m_colnum; j++) {
			char nbuf[16];
			itoa(j, nbuf, 10);
			ItemData* id = new ItemData(nullStr);
			ddfile.read(id->getText(), keybuf, nbuf, nullStr);
			lvid->addItem(id);
		}
		m_item->addItem(lvid);
	}
	return SimpleCtrl::loadData(ddfile);
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
		if (!tid->initItem(m_pcp->m_hwndCtrl, TVI_ROOT, m_bSorted)) return FALSE;
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
	m_state = reinterpret_cast<TreeItemData*>(tvi.lParam)->getName();
	return TRUE;
}

BOOL
TreeCtrl::onSetSort(CmdLineParser& sstate)
{
	if (m_pcp->m_hwndCtrl) return FALSE;
	if (sstate.itemNum() > 0 &&
		sstate.getArgvStr(0).compareTo("yes") == 0) {
		m_bSorted = TRUE;
	} else {
		m_bSorted = FALSE;
	}
	return TRUE;
}

StringBuffer
TreeCtrl::onGetSort()
{
	return m_bSorted ? "yes" : "no";
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
	tid->getText() = RemoveQuote(text.getRawData());
	if (m_pcp->m_hwndCtrl != NULL) {
		TV_ITEM	tvi;
		tvi.mask	= TVIF_TEXT;
		tvi.pszText	= tid->getText().getBufPtr();
		tvi.hItem	= tid->m_hItem;
		BOOL ret = TreeView_SetItem(m_pcp->m_hwndCtrl, &tvi);
		if (ret && m_bSorted) {
			TVSORTCB tvs;
			tvs.hParent = tid->getParent() ? tid->getParent()->m_hItem : TVI_ROOT;
			tvs.lpfnCompare = (PFNTVCOMPARE)CompareTreeItems;
			TreeView_SortChildrenCB(m_pcp->m_hwndCtrl, &tvs, 0);
		}
		return ret;
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
		tid->initItem(m_pcp->m_hwndCtrl, hItem, m_bSorted);
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

BOOL
TreeCtrl::onSetCtrlFont(CmdLineParser& argv)
{
	if (argv.itemNum() < 1) return FALSE;
	argv.initSequentialGet();
	const StringBuffer& fface = argv.getNextArgvStr();
	const StringBuffer& color = argv.getNextArgvStr();
	const StringBuffer& fname = argv.getNextArgvStr();
	this->setFont(fface, color, fname);
	TreeView_SetTextColor(m_pcp->m_hwndCtrl, m_pcp->m_fontprop.m_color);
	this->sendData();
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

int
TreeCtrl::onSetFocusedItem(const StringBuffer& pos)
{
	return onSetState(RealCmdLineParser(pos));
}

StringBuffer
TreeCtrl::onGetFocusedItem()
{
	return onGetState();
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
	ddfile.write(m_state, GetString(STR_DLGDATA_STATE), nullStr);
	ddfile.write(m_bSorted, GetString(STR_DLGDATA_SORT), nullStr);
	int	num = m_item->initSequentialGet();
	StringBuffer keybuf(GetString(STR_DLGDATA_ROOT), -1, 32);
	keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_CHILD))
			.append((TCHAR)'_');
	for (int i = 0; i < num; i++) {
		TreeItemData*
			tid = static_cast<TreeItemData*>(m_item->getNextItem());
		if (tid == NULL) continue;
		char nbuf[16];
		itoa(i, nbuf, 10);
		ddfile.write(tid->getName(), keybuf, nbuf, nullStr);
		tid->dumpData(ddfile);
	}
	keybuf.reset(GetString(STR_DLGDATA_ROOT));
	keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_ITEMNUM));
	ddfile.write(num, keybuf, nullStr);
	return TRUE;
}

BOOL
TreeCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(m_state, GetString(STR_DLGDATA_STATE), nullStr);
	ddfile.read(&m_bSorted, GetString(STR_DLGDATA_SORT), nullStr);
	int	num;
	StringBuffer keybuf(GetString(STR_DLGDATA_ROOT), -1, 32), namebuf(32);
	keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_ITEMNUM));
	ddfile.read(&num, keybuf, nullStr);
	keybuf.reset(GetString(STR_DLGDATA_ROOT));
	keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_CHILD))
			.append((TCHAR)'_');
	for (int i = 0; i < num; i++) {
		char nbuf[16];
		itoa(i, nbuf, 10);
		ddfile.read(namebuf, keybuf, nbuf, nullStr);
		if (namebuf.length() <= 0) continue;
		TreeItemData* tid = new TreeItemData(namebuf, nullStr);
		if (tid->loadData(ddfile, *m_pHashItem)) {
			m_item->addItem(tid);
			m_pHashItem->setValue(tid->getName(), tid);
		}
	}
	return SimpleCtrl::loadData(ddfile);
}

FrameCtrl::FrameCtrl(
	const StringBuffer& name,
	const StringBuffer& text,
	CTRL_ID type)
	: HasListCtrl(name, text, type)
	, m_page(NULL)
{
	if (type == CTRLID_FRAME) {
		m_pcp->m_style		= BS_FLAT;
		m_pcp->m_classname	= (LPSTR)0x80;
		m_poffset.x	= m_poffset.y = m_pmargin.x = m_pmargin.y = 0;
		m_bVisible = FALSE;
	} else if (type == CTRLID_GROUP) {
		m_pcp->m_style		= BS_GROUPBOX;
		m_pcp->m_exstyle	= WS_EX_TRANSPARENT;
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

DlgPage*
FrameCtrl::getPageByIndex(int index)
{
	ItemData* id = m_item->getItemByIndex(index);
	if (!id) {
		return NULL;
	}

	DlgPage* pPage = m_pDlgPage->getDlgFrame().getPage(id->getText());

	return pPage;
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
FrameCtrl::getDefID() const
{
	if (m_state == 0) return 0xFFFF;
	ItemData* id = m_item->getItemByIndex(m_state - 1);
	if (!id) {
//		::MessageBox(NULL, "Failed to get id", NULL, MB_OK);
		return 0xFFFF;
	}
//	::MessageBox(NULL, (LPCSTR)id->getText(), NULL, MB_OK);
	DlgPage* pdp = m_pDlgPage->getDlgFrame().getPage(id->getText());
	if (!pdp) {
//		::MessageBox(NULL, "Failed to get pdp", NULL, MB_OK);
		return 0xFFFF;
	}
	return pdp->getDefID();
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

	// グループボックス内のダイアログのオフセットを文字の大きさから決定
	if (m_type == CTRLID_GROUP) {
		DWORD fontsize = m_pDlgPage->getDlgFrame().getFontSize();
		HDC hDC = ::GetDC(hDlg);
		DWORD dwLPX = ::GetDeviceCaps(hDC, LOGPIXELSX),
			  dwLPY = ::GetDeviceCaps(hDC, LOGPIXELSY);
		TEXTMETRIC tm;
		::GetTextMetrics(hDC, &tm);
		::ReleaseDC(hDlg, hDC);

		m_poffset.x = dwLPX * fontsize * 4 / (72 * 2 * tm.tmAveCharWidth);
		m_poffset.y = dwLPY * fontsize * 8 * 4 / (72 * 3 * tm.tmHeight);
	}
	
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
								m_cy - m_pmargin.y,
								m_pDlgPage->isInTabCtrl())) {
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
	BOOL bActualState = bChange ? bEnable : (m_bEnable && bEnable);
	if (m_pcp->m_hwndCtrl) {
		::EnableWindow(m_pcp->m_hwndCtrl, bActualState);
	}
	DlgPage* pPage = getPageByIndex(m_state - 1);
	if (!pPage) {
		return FALSE;
	}
	pPage->enablePage(bActualState);
	return TRUE;
}

BOOL
FrameCtrl::showCtrl(BOOL bVisible)
{
	if (m_state <= 0) m_state = 1;

	if (m_type == CTRLID_GROUP)
	{
		::ShowWindow(m_pcp->m_hwndCtrl, bVisible ? SW_SHOW : SW_HIDE);
	}
	else
	{
		::ShowWindow(m_pcp->m_hwndCtrl, SW_HIDE);
	}

	DlgPage* pPage = getPageByIndex(m_state - 1);
	if (!pPage) {
		return FALSE;
	}
	pPage->showPage(bVisible);
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

HBRUSH
FrameCtrl::onCtlColor(HWND hwndCtrl, UINT uMsg, HDC hDC)
{
	SimpleCtrl::onCtlColor(hwndCtrl, uMsg, hDC);
	return NULL;
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

DlgPage*
TabCtrl::getPageByIndex(int index)
{
	NamedItemData* id = static_cast<NamedItemData*>(m_item->getItemByIndex(index));
	if (!id) {
		return NULL;
	}

	DlgPage* pPage = m_pDlgPage->getDlgFrame().getPage(id->getName());

	return pPage;
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
TabCtrl::getDefID() const
{
	if (m_state == 0) return 0xFFFF;
	ItemData* id = m_item->getItemByIndex(m_state - 1);
	if (!id) {
//		::MessageBox(NULL, "Failed to get id", NULL, MB_OK);
		return 0xFFFF;
	}
	DlgPage* pdp = m_pDlgPage->getDlgFrame().getPage(
					static_cast<NamedItemData*>(id)->getName());
	if (!pdp) {
//		::MessageBox(NULL, "Failed to get pdp", NULL, MB_OK);
		return 0xFFFF;
	}
	return pdp->getDefID();
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

	DWORD fontsize = m_pDlgPage->getDlgFrame().getFontSize();
	HDC hDC = ::GetDC(hDlg);
	DWORD dwLPX = ::GetDeviceCaps(hDC, LOGPIXELSX),
		  dwLPY = ::GetDeviceCaps(hDC, LOGPIXELSY);
	TEXTMETRIC tm;
	::GetTextMetrics(hDC, &tm);
	::ReleaseDC(hDlg, hDC);

	m_poffset.x = dwLPX * fontsize * 4 / (72 * 2 * tm.tmAveCharWidth);
	m_poffset.y = dwLPY * fontsize * 8 * 2 / (72 * tm.tmHeight);
	
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
							m_cy - m_pmargin.y,
							true);
	}
	return TRUE;
}

BOOL
TabCtrl::enableCtrl(BOOL bEnable, BOOL bChange)
{
	if (!HasListCtrl::enableCtrl(bEnable, bChange)) return FALSE;
	if (m_state <= 0 || m_state > m_item->itemNum()) m_state = 1;
	BOOL bActualState = bChange ? bEnable : (m_bEnable && bEnable);
	if (m_pcp->m_hwndCtrl) {
		TC_ITEM	tci;
		tci.mask = TCIF_PARAM;
		TabCtrl_GetItem(m_pcp->m_hwndCtrl, m_state-1, &tci);
		if (tci.lParam != 0) {
			reinterpret_cast<DlgPage*>(tci.lParam)->enablePage(bActualState);
		}
	} else {
		DlgPage* pPage = getPageByIndex(m_state - 1);
		if (!pPage) {
			return FALSE;
		}
		pPage->enablePage(bActualState);
	}
	return TRUE;
}

BOOL
TabCtrl::showCtrl(BOOL bVisible)
{
	if (!HasListCtrl::showCtrl(bVisible)) return FALSE;
	if (m_state <= 0 || m_state > m_item->itemNum()) m_state = 1;
	if (m_pcp->m_hwndCtrl) {
		TC_ITEM	tci;
		tci.mask = TCIF_PARAM;
		TabCtrl_GetItem(m_pcp->m_hwndCtrl, m_state-1, &tci);
		if (tci.lParam != 0)
			reinterpret_cast<DlgPage*>(tci.lParam)->showPage(bVisible);
	} else {
		DlgPage* pPage = getPageByIndex(m_state - 1);
		if (!pPage) {
			return FALSE;
		}
		pPage->showPage(bVisible);
	}
	return TRUE;
}

BOOL
TabCtrl::sendData()
{
	if (!HasListCtrl::sendData()) return FALSE;
	if (m_state <= 0 || m_state > m_item->itemNum()) m_state = 1;
	if (m_pcp->m_hwndCtrl) {
		TabCtrl_SetCurSel(m_pcp->m_hwndCtrl, m_state-1);
	}
	return TRUE;
}

BOOL
TabCtrl::receiveData()
{
	if (!HasListCtrl::receiveData()) return FALSE;
	if (m_pcp->m_hwndCtrl) {
		m_state = TabCtrl_GetCurSel(m_pcp->m_hwndCtrl) + 1;
	}
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
			if ((::GetKeyState(VK_SHIFT) & 0x8000) == 0) {
				DEBUG_OUTPUT(("CTRL+TAB is pressed!!"));
				sel += 1;
				if (sel >= m_item->itemNum()) sel = 0;
			} else {
				DEBUG_OUTPUT(("CTRL+SHIFT+TAB is pressed!!"));
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
	ddfile.write(m_state, GetString(STR_DLGDATA_STATE), nullStr);
	int	num = m_item->initSequentialGet();
	StringBuffer keybuf(GetString(STR_DLGDATA_ITEM), -1, 32);
	keybuf.append((TCHAR)'_');
	int	keylen = keybuf.length();
	for (int i = 0; i < num; i++) {
		NamedItemData*
			nid = static_cast<NamedItemData*>(m_item->getNextItem());
		if (nid == NULL) continue;
		keybuf.setlength(keylen);
		keybuf.append(i);
		ddfile.write(nid->getText(), keybuf, nullStr);
		keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_NAME));
		ddfile.write(nid->getName(), keybuf, nullStr);
	}
	ddfile.write(num, GetString(STR_DLGDATA_ITEMNUM), nullStr);
	return TRUE;
}

BOOL
TabCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_state, GetString(STR_DLGDATA_STATE), nullStr);
	int	num;
	ddfile.read(&num, GetString(STR_DLGDATA_ITEMNUM), nullStr);
	if (m_state <= 0 || m_state > num) m_state = 1;
	StringBuffer keybuf(GetString(STR_DLGDATA_ITEM), -1, 32), namebuf(32);
	keybuf.append((TCHAR)'_');
	int	keylen = keybuf.length(), len;
	for (int i = 0; i < num; i++) {
		keybuf.setlength(keylen);
		keybuf.append(i);
		len = keybuf.length();
		keybuf.append((TCHAR)':').append(GetString(STR_DLGDATA_NAME));
		ddfile.read(namebuf, keybuf, nullStr);
		if (namebuf.length() <= 0) continue;
		NamedItemData* nid = new NamedItemData(namebuf, nullStr);
		keybuf.setlength(len);
		ddfile.read(nid->getText(), keybuf, nullStr);
		m_item->addItem(nid);
	}
	return SimpleCtrl::loadData(ddfile);
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
	BOOL bActualState = bChange ? bEnable : (m_bEnable && bEnable);
	for (int i = 0; i < m_cnum; i++) {
		if (m_pcp[i].m_hwndCtrl == NULL) continue;
		::EnableWindow(m_pcp[i].m_hwndCtrl, bActualState);
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
		if (m_pcp[i].m_fontprop.m_bchanged)
			m_pcp->changeFont();
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
//	::SetWindowText(m_pcp[0].m_hwndCtrl, m_pcp[0].m_text);
	m_pcp[0].setText();
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
		return m_notify[(m_notify[1] != 0xFFFF)];
	}
	return 0xFFFF;
}

BOOL
SpinCtrl::dumpData(DlgDataFile& ddfile)
{
	if (!MultipleCtrl::dumpData(ddfile)) return FALSE;
	ddfile.write(m_min, GetString(STR_DLGDATA_MIN), nullStr);
	ddfile.write(m_max, GetString(STR_DLGDATA_MAX), nullStr);
	ddfile.write(m_val, GetString(STR_DLGDATA_VAL), nullStr);
	return TRUE;
}

BOOL
SpinCtrl::loadData(DlgDataFile& ddfile)
{
	ddfile.read(&m_min, GetString(STR_DLGDATA_MIN), nullStr);
	ddfile.read(&m_max, GetString(STR_DLGDATA_MAX), nullStr);
	ddfile.read(&m_val, GetString(STR_DLGDATA_VAL), nullStr);
	return MultipleCtrl::loadData(ddfile);
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

WORD
OkCancelCtrl::getDefID() const
{
	for (int i = 0; i < 2; i++) {
//		DWORD dwStyle = ::GetWindowLong(m_pcp[i].m_hwndCtrl, GWL_STYLE);
//		if (dwStyle & BS_DEFPUSHBUTTON) return m_pcp[i].m_id;
		if (m_pcp[i].m_style & BS_DEFPUSHBUTTON) {
			return m_pcp[i].m_id;
		}
	}
	return 0xFFFF;
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
	return m_notify[(LOBYTE(LOWORD(wParam)) != IDOK)];
}

