//	$Id: dlgdata.cpp,v 1.17 2003-12-15 15:20:44 sugiura Exp $
/*
 *	dlgdata.cpp
 *	�_�C�A���O�������N���X
 */

#include "session.h"
#include "dlgdata.h"
#include "stack.h"
#include "ddfile.h"
#include "ctrldata.h"
#include "misc.h"
#include "str_tbl.h"

const StringBuffer strRootPageName("root");

//	�V�X�e���t�H���g(DEFAULT_GUI_FONT)�̑����𓾂�
BOOL
GetDefFontProperty(LPFontProperty pfp)
{
	if (pfp == NULL) return FALSE;
	HDC	hDc = ::GetDC(::GetDesktopWindow());
	if (hDc == NULL) return FALSE;
	HGDIOBJ	hOrgFont = ::SelectObject(hDc,::GetStockObject(DEFAULT_GUI_FONT));
	::GetTextFace(hDc,32,pfp->m_fontname.getBufPtr());
	TEXTMETRIC tm;
	::GetTextMetrics(hDc,&tm);
	pfp->m_fontsize = ::MulDiv(tm.tmHeight,72,::GetDeviceCaps(hDc,LOGPIXELSY));
	::SelectObject(hDc,hOrgFont);
	::ReleaseDC(::GetDesktopWindow(),hDc);
	return TRUE;
}

static POINTS
GetDlgPosOrigin(WORD wPosOrigin, HWND hwndFrame)
{
	HWND hwndParent = ::GetParent(hwndFrame);

	RECT crect, drect;
	::GetWindowRect(hwndFrame, &drect);
	::GetClientRect(hwndParent, &crect);
	POINT cs = { 0, 0 };
	::ClientToScreen(hwndParent, &cs);
	crect.left += cs.x;  crect.right  += cs.x;
	crect.top  += cs.y;  crect.bottom += cs.y;

	POINT pos;
	switch (wPosOrigin) {
	case DLGPOS_SCREEN_CENTER:
		/*	Screen Center	*/
		::GetWindowRect(::GetDesktopWindow(), &crect);
		pos.x = ((crect.left + crect.right) >> 1)
				- ((drect.right - drect.left) >> 1);
		pos.y = ((crect.top + crect.bottom) >> 1)
				- ((drect.bottom - drect.top) >> 1);
		break;
	case DLGPOS_CLIENT_COORD:
		/*	Window Coordinate	*/
		pos.x = crect.left;
		pos.y = crect.top;
		break;
	case DLGPOS_SCREEN_COORD:
		pos.x = pos.y = 0;
		break;
	case DLGPOS_CARET_COORD:
		/*  Caret Coordinate  */
		::GetCaretPos(&pos);
		::ClientToScreen(hwndParent, &pos);
		break;
	case DLGPOS_CURSOR_COORD:
		/*  Cursor Coordinate  */
		::GetCursorPos(&pos);
		break;
	case DLGPOS_CLIENT_CENTER:
	default:
		pos.x = ((crect.left + crect.right) >> 1)
				- ((drect.right - drect.left) >> 1);
		pos.y = ((crect.top + crect.bottom) >> 1)
				- ((drect.bottom - drect.top) >> 1);
		break;
	}

	POINTS rpos;
	rpos.x = (WORD)pos.x;
	rpos.y = (WORD)pos.y;

	return rpos;
}

static POINTS
GetDlgPosByPixel(const POINTS& pos, WORD wPosUnit, HWND hwndFrame)
{
	DWORD dlgbaseunit = GetDialogBaseUnits(hwndFrame, "M");
	POINTS ppos;
	if (wPosUnit == DLGPOS_UNIT_FONTSIZE) {
		ppos.x = (pos.x * LOWORD(dlgbaseunit)) >> 2;
		ppos.y = (pos.y * HIWORD(dlgbaseunit)) >> 3;
	} else {
		// �s�N�Z���P��
		ppos = pos;
	}

	return ppos;
}

static POINTS
GetDlgPosByUnit(const POINTS& pos, WORD wPosUnit, HWND hwndFrame)
{
	DWORD dlgbaseunit = GetDialogBaseUnits(hwndFrame, "M");
	POINTS rpos;
	if (wPosUnit == DLGPOS_UNIT_FONTSIZE) {
		rpos.x = pos.x * 4 / LOWORD(dlgbaseunit);
		rpos.y = pos.y * 8 / HIWORD(dlgbaseunit);
	} else {
		// �s�N�Z���P��
		rpos = pos;
	}

	return rpos;
}

//	DLGTEMPLATE �̍\�z
WORD*
CreateDialogTemplate(
	LPDLGTEMPLATE pDlgTemplate,
	DWORD style, DWORD exstyle,
	WORD cdit,
	WORD x, WORD y, WORD cx, WORD cy,
	const StringBuffer& title,
	const StringBuffer& fontname, WORD fontsize)
{
	//	DLGTEMPLATE
	pDlgTemplate->style				= style;
	pDlgTemplate->dwExtendedStyle	= exstyle;
	pDlgTemplate->cdit				= cdit;
	pDlgTemplate->x					= x;
	pDlgTemplate->y					= y;
	pDlgTemplate->cx				= cx;
	pDlgTemplate->cy				= cy;

	LPWORD lpwExData = reinterpret_cast<LPWORD>(pDlgTemplate + 1);
	lpwExData += 2;	//	���j���[���A�N���X���͂Ȃ�
	lpwExData += title.toUnicode((LPWSTR)lpwExData);

	*lpwExData++ = fontsize;
	lpwExData += fontname.toUnicode((LPWSTR)lpwExData);

	//	DLGITEMTEMPLATE �� DWORD ���E����n�܂�K�v����!!
//	if ((reinterpret_cast<DWORD>(lpwExData))%sizeof(DWORD)) lpwExData++;
	if (reinterpret_cast<DWORD>(lpwExData) & (sizeof(DWORD) - 1)) lpwExData++;

	return lpwExData;
}


//	�q�_�C�A���O�̃R�[���o�b�N�֐�
BOOL CALLBACK
DlgPageProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DlgPage* pdp;
	HWND hwndFrame;

	if (uMsg == WM_INITDIALOG) {
		//	�q�_�C�A���O�̏�����
		::SetWindowLong(hDlg,DWL_USER,(LONG)lParam);
		pdp = reinterpret_cast<DlgPage*>(lParam);
		if (!pdp->initPage(hDlg)) {
			::PostMessage(pdp->getDlgFrame().getUserDlg(),WM_CLOSE,0,0);
		}
		return FALSE;
	}

	//	�q�_�C�A���O�N���X�ւ̃|�C���^�̎擾
	if ((pdp = reinterpret_cast<DlgPage*>(::GetWindowLong(hDlg,DWL_USER)))
		== NULL) return FALSE;

	switch (uMsg) {
	case WM_USER_MESSAGEBOX:
	case WM_USER_QUESTIONBOX:
		{
			//	���b�Z�[�W�{�b�N�X�̏����͐e�_�C�A���O�ɔC��
			HWND hwndFrame = pdp->getDlgFrame().getUserDlg();
			if (hwndFrame != NULL) ::PostMessage(hwndFrame,uMsg,wParam,lParam);
		}
		break;

	case WM_COMMAND:
	case WM_NOTIFY:
	case WM_HSCROLL:
		{
			WORD notify = 0xFFFF;
			switch (uMsg) {
			case WM_COMMAND:
				notify = pdp->onCommand(wParam,lParam);
				break;
			case WM_NOTIFY:
				notify = pdp->onNotify(wParam,lParam);
				break;
			case WM_HSCROLL:	//	�g���b�N�o�[�̒ʒm�R�[�h���擾����
				notify = pdp->onHScroll(wParam,lParam);
				break;
			}
			if (notify != 0xFFFF) {
				//	�ʒm�R�[�h�̑��M
				hwndFrame = pdp->getDlgFrame().getUserDlg();
				if (hwndFrame != NULL)
					::PostMessage(hwndFrame,WM_USER_NOTIFY,(WPARAM)notify,0L);
			}
		}
		break;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLOREDIT:
		//	�R���g���[���̃t�H���g�̐F��ύX
		return reinterpret_cast<BOOL>(pdp->onCtlColor(uMsg, wParam,lParam));

#if 0
	case WM_CTLCOLORSTATIC:
		pdp->onCtlColor(wParam,lParam);
		return FALSE;
#endif

	case DM_SETDEFID:
		// prevent the default message handling
		break;

	case WM_CLOSE:
		hwndFrame = pdp->getDlgFrame().getUserDlg();
		if (hwndFrame != NULL) ::PostMessage(hwndFrame, WM_CLOSE, 0, 0);
		else ::DestroyWindow(hDlg);
		break;

	case WM_DESTROY:
		// this is invoked in uninitFrame()
		// pdp->uninitPage();
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

DlgPage::DlgPage(DlgFrame* pdf, const StringBuffer& name, WORD width)
	:	m_pDlgFrame(pdf),
		m_strName(name),
		m_hwndPage(NULL),
		m_dwPageSize(0),
		m_pCtrlList(new LinkList<CtrlListItem>),
		m_pHashCtrl(new CtrlHashTable),
		m_pCurCtrlData(NULL),
		m_pShowPageQueue(new LinkList<ShowPageQueueItem>),
		m_bVisible(FALSE),
		m_bEnable(TRUE),
		m_cdit(0),
		m_iniwidth(width),
		m_width(0),
		m_height(0)
{
	//	nothing to do more.
}

DlgPage::~DlgPage()
{
	if (m_hwndPage != NULL) ::DestroyWindow(m_hwndPage);
	delete m_pCtrlList;
	delete m_pHashCtrl;
	delete m_pShowPageQueue;
}

//	�q�_�C�A���O�̃E�B���h�E�̐���
HWND
DlgPage::createPage(
	HWND hwndCtrl,
	BYTE hbId,
	WORD x, WORD y, WORD cx, WORD cy,
	bool bInTabCtrl,
	FontProperty& fp)
{
	//	hwndCtrl	:	�q�_�C�A���O�����R���g���[���̃E�B���h�E�n���h��
	//					NULL �łȂ��ꍇ�Ax, y �͂��̃R���g���[���̍��ォ���
	//					�I�t�Z�b�g�ɂȂ�

	if (m_dwPageSize == 0) {
		//	�q�_�C�A���O�̃e���v���[�g�̃T�C�Y�ƃE�B���h�E�̑傫���̕]��
		if (!this->evalPageSize()) return FALSE;
	}

	//	DLGTEMPLATE �̑傫���𑫂�
	m_dwPageSize += sizeof(DLGTEMPLATE) + 
//					sizeof(WORD) * (fp.m_fontname.length() + 1) + 16;
					((fp.m_fontname.length() + 1) << 1) + 16;
	//	�o�b�t�@�̊m��
	Array<BYTE> pDlgTemplate(m_dwPageSize);
	pDlgTemplate.zero();

	//	DLGTEMPLATE �����̍\�z
	WORD* lpwExData = CreateDialogTemplate((LPDLGTEMPLATE)(BYTE*)pDlgTemplate,
										WS_CHILD|WS_VISIBLE|
											WS_CLIPCHILDREN|WS_CLIPSIBLINGS|
											DS_CONTROL|DS_SETFONT,
										0L,
										m_cdit,
										x, y, cx, cy,
										nullStr,
										fp.m_fontname,
										(WORD)fp.m_fontsize);

	//	DLGITEMTEMPLATE �����̍\�z
	CtrlListItem::CtrlTemplateArgs cta;
	cta.m_hdr = lpwExData;
	cta.m_id = (hbId << 8) + 0x10;	//	hbId �� DlgPage ���ʂh�c
	m_pCtrlList->initSequentialGet();
	CtrlListItem* cli;
	while ((cli = m_pCtrlList->getNextItem()) != NULL) {
		if (cli->getCtrlNum() > 0)	//	NewFormatCtrl ������
			cli->createCtrlTemplate(cta);
	}

	//	�_�C�A���O�̍쐬
	HWND hwndFrame = m_pDlgFrame->getUserDlg();
	m_bInTabCtrl = bInTabCtrl;
	m_hwndPage = ::CreateDialogIndirectParam(
					m_pDlgFrame->getSessionInstance()->getInstanceHandle(),
					(LPDLGTEMPLATE)(BYTE*)pDlgTemplate,
					hwndFrame,
					(DLGPROC)DlgPageProc,
					(LPARAM)this);
	if (m_hwndPage != NULL && hwndCtrl != NULL) {
		//	"root" �ȊO�̎q�_�C�A���O�̈ʒu�̕ύX
		RECT rct;
		::GetWindowRect(hwndCtrl,&rct);
		::ScreenToClient(hwndFrame,(LPPOINT)&rct);
		DWORD dlgunit = GetDialogBaseUnits(hwndFrame);
		::SetWindowPos(m_hwndPage,
						NULL,
						rct.left + ((x * LOWORD(dlgunit)) >> 2),
						rct.top + ((y * HIWORD(dlgunit)) >> 3),
						0, 0,
						SWP_NOSIZE|SWP_NOZORDER);
	}

	//	��n��
	m_dwPageSize = 0;

	if (m_pShowPageQueue->itemNum() > 0) {
		ShowPageQueueItem* childPageQI;
		while ((childPageQI = m_pShowPageQueue->getItemByIndex(0)) != NULL) {
			m_pDlgFrame->createPage(
								childPageQI->m_pdp->getName(),
								childPageQI->m_hwndCtrl,
								childPageQI->m_x,childPageQI->m_y,
								childPageQI->m_cx,childPageQI->m_cy,
								childPageQI->m_bInTabCtrl
							);
			m_pShowPageQueue->delItemByIndex(0);
		}
	}

	return m_hwndPage;
}

//	�q�_�C�A���O�̏�����
BOOL
DlgPage::initPage(HWND hDlg)
{
	if (hDlg == NULL) return FALSE;
	m_hwndPage = hDlg;
	if (m_bInTabCtrl) {
		m_pDlgFrame->setBackGroundToTabColor(m_hwndPage);
	}
	//	�R���g���[���̏�����
	m_pCtrlList->initSequentialGet();
	CtrlListItem* cli;
	while ((cli = m_pCtrlList->getNextItem()) != NULL) {
		cli->onInitCtrl(hDlg);
		cli->enableCtrl(m_bEnable,FALSE);
	}
	::ShowWindow(m_hwndPage,m_bVisible ? SW_SHOW : SW_HIDE);
	return TRUE;
}

//	�q�_�C�A���O�̔j��
BOOL
DlgPage::uninitPage()
{
	if (m_hwndPage == NULL) return FALSE;
	//	�R���g���[���E�B���h�E�̏I������
	m_pCtrlList->initSequentialGet();
	CtrlListItem* cli;
	while ((cli = m_pCtrlList->getNextItem()) != NULL) {
		cli->onUninitCtrl();
	}
	m_hwndPage = NULL;
	return TRUE;
}

//	�q�_�C�A���O������
BOOL
DlgPage::showPage(BOOL bVisible)
{
	if (m_hwndPage == NULL) return FALSE;
	m_bVisible = bVisible;
	::ShowWindow(m_hwndPage,m_bVisible ? SW_SHOW : SW_HIDE);
	if (m_bVisible) {
		::SetWindowPos(m_hwndPage,HWND_TOP,0,0,0,0,
						SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		::InvalidateRect(m_hwndPage,NULL,TRUE);
	}

	//	�R���g���[���̉���
	m_pCtrlList->initSequentialGet();
	CtrlListItem* cli;
	while ((cli = m_pCtrlList->getNextItem()) != NULL) {
		cli->showCtrl(bVisible);
	}

	return TRUE;
}

//	�q�_�C�A���O�̗L����
BOOL
DlgPage::enablePage(BOOL bEnable)
{
	m_bEnable = bEnable;
	if (m_hwndPage != NULL) {
		m_pCtrlList->initSequentialGet();
		CtrlListItem* cli;
		while ((cli = m_pCtrlList->getNextItem()) != NULL) {
			cli->enableCtrl(bEnable,FALSE);
		}
	}
	return TRUE;
}

//	DLGITEMTEMPLATE �����̃T�C�Y�ƃE�B���h�E�̑傫���̕]��
//	LOWORD(return)	:	�E�B���h�E�̕�(�����P��)
//	HIWORD(return)	:	�E�B���h�E�̍���(�����P��)
DWORD
DlgPage::evalPageSize()
{
	if (m_pCtrlList == NULL) return 0;
	//	�_�C�A���O�̏I���
	m_pCtrlList->addItem(new PageFormatCtrl("0",CTRLID_NEWPAGE));

	WORD	dcx = m_iniwidth, dcy = 0,
			py = 0, pcx = m_iniwidth, pcy = 0,
			cx = 0, ccx = m_iniwidth, ccy = 0,
			ly = 0, lcx = 0, lcy = 0,
			cw = 0, ch = 0;
	CtrlListItem* cli;
	m_cdit = 0;	//	���ۂ̃R���g���[���̐�
	m_dwPageSize = 0;	//	DLGITEMTEMPLATE �����̃T�C�Y
	m_pCtrlList->initSequentialGet();
	while ((cli = m_pCtrlList->getNextItem()) != NULL) {
		switch (cli->getCtrlType()) {
		case CTRLID_NEWCOLUMN:
			//	new column
			if (pcy < ccy) pcy = ccy;	//	�y�[�W���̒���
			cx = pcx + 1;	//	�R�������ʒu
			ccx = cli->getWidth();	//	�R������
			pcx += ccx + 1;	//	�y�[�W���̒���
			ccy = ly = lcx = lcy = 0;
			break;
		case CTRLID_NEWPAGE:
			//	new page
			if (lcy > 0) ccy += lcy;	//	���O�̍s�̍�����ǉ�
			if (pcy < ccy) pcy = ccy;	//	�y�[�W���̒���
			if (dcx < pcx) dcx = pcx;	//	�_�C�A���O���̒���
			dcy += pcy;	//	�_�C�A���O���̒���
			py = dcy;	//	�y�[�W�c�ʒu
			pcx = ccx = cli->getWidth();	//	�y�[�W�E�R������
			pcy = cx = ccy = ly = lcx = lcy = 0;
			break;
		case CTRLID_UNKNOWN:
			//	invalid data.
			m_pCtrlList->delItemByIndex(-1); // �ǉ����� NewFormatCtrl ���폜
			return 0L;
		default:
			m_cdit += cli->getCtrlNum();
			m_dwPageSize += (DWORD)cli->getDataSize();
			ch = cli->getHeight();
			if (ch < 1) ch = 3;	//	ListCtrl, ChkList, LViewCtrl �̏����l
			cw = cli->getWidth();
			if (!cw || cw != 0xFFFF && cw > ccx - lcx) {
				//	���w��Ȃ����A���݂̃R�����������L���w��
				cw = ccx - lcx;
				cli->setRect(cx+lcx,py+ly,cw,ch);
				if (lcy < ch) lcy = ch;	//	�s���̒���
				//	���̍s��
				ly += lcy;
				ccy = ly;
				lcx = 0;
				lcy = 0;
			} else {
				//	���w�肠��
				if (cw == 0xFFFF) cw = (ccx - lcx)/2;	//	"half"
				cli->setRect(cx+lcx,py+ly,cw,ch);
				if (lcy < ch) lcy = ch;	//	�s���̒���
				lcx += cw;	//	��ʒu�̒���
				if (lcx > ccx) lcx = ccx;	//	�R�������̒���
			}
		}
	}
	//	end of defs.
	if (dcy == 0) dcy = 1;
	m_width = dcx;
	m_height = dcy;
	m_pCtrlList->delItemByIndex(-1); // �ǉ����� NewFormatCtrl ���폜
	return (DWORD)MAKELONG(m_width,m_height);
}

BOOL
DlgPage::addShowPageQueue(
	LPCSTR pname,
	HWND hwndCtrl,
	WORD x, WORD y, WORD cx, WORD cy,
	bool bInTabCtrl)
{
	if (m_pDlgFrame->getUserDlg() == NULL) return FALSE;
	DlgPage* pdp = m_pDlgFrame->getPage(pname);
	if (pdp == NULL) return FALSE;
	m_pShowPageQueue->addItem(new ShowPageQueueItem(pdp, hwndCtrl,
													x, y, cx, cy,
													bInTabCtrl),
							  -1);
	return TRUE;
}

WORD
DlgPage::getDefID() const
{
	CtrlListItem* cli;
	m_pCtrlList->initSequentialGet();
	while ((cli = m_pCtrlList->getNextItem()) != NULL) {
		WORD id = cli->getDefID();
		if (id != 0xFFFF) return id;
	}
	return 0xFFFF;
}

//	�R���g���[���̒ǉ�
int
DlgPage::addCtrl(CtrlListItem* cli)
{
	if (cli == NULL) return 0;
	cli->setParentPage(this);	//	�e�_�C�A���O�̃Z�b�g
	const StringBuffer& name = cli->getName();
	if (name.length() > 0) // �R���g���[���̖��O�̓o�^
		m_pHashCtrl->setValue(name,cli);
	m_pCurCtrlData = cli;	//	Poke �̑Ώ�
	return m_pCtrlList->addItem(cli);
}

int
DlgPage::addCtrl(int type, const StringBuffer& name, const StringBuffer& text)
{
	return this->addCtrl(CtrlListItem::createCtrl(type,name,text));
}

//	�R���g���[���̎擾
CtrlListItem*
DlgPage::getCtrl(const StringBuffer& name)
{
	return (name.length() > 0) ? m_pHashCtrl->getValue(name) : m_pCurCtrlData;
}

HWND
DlgPage::getFocusedCtrl()
{
	if (m_hwndPage == NULL) return NULL;
	CtrlListItem* cli;
	HWND hwndCtrl;
	m_pCtrlList->initSequentialGet();
	while ((cli = m_pCtrlList->getNextItem()) != NULL) {
		if ((hwndCtrl = cli->getFocusedCtrl()) != NULL) return hwndCtrl;
	}
	return NULL;
}

static CtrlListItem*
GetCtrlListItemByHWND(HWND hwndCtrl)
{
	if (hwndCtrl == NULL) return NULL;
	CtrlListItem*
		cli = (CtrlListItem*)::SendMessage(hwndCtrl, WM_GET_CTRL_PTR, 0, 0);
	if (cli == NULL || !cli->isValid()) {
		return NULL;
	}
	return cli;
}

static inline CtrlListItem*
GetCtrlListItemByID(HWND hwndPage, WORD id)
{
	if (hwndPage == NULL) return NULL;
	return GetCtrlListItemByHWND(::GetDlgItem(hwndPage, id));
}

//	WM_COMMAND �n���h��
WORD
DlgPage::onCommand(WPARAM wParam, LPARAM lParam)
{
	CtrlListItem* cli = GetCtrlListItemByID(m_hwndPage, LOWORD(wParam));
	return cli ? cli->onWmCommand(wParam, lParam) : 0xFFFF;
}

//	WM_NOTIFY �n���h��
WORD
DlgPage::onNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* pnmh = reinterpret_cast<NMHDR*>(lParam);
	CtrlListItem* cli = GetCtrlListItemByID(m_hwndPage, pnmh->idFrom);
	return cli ? cli->onWmNotify(wParam, lParam) : 0xFFFF;
}

//	WM_HSCROLL �n���h��(�g���b�N�o�[�̂��߂ɂ̂ݑ��݂���)
WORD
DlgPage::onHScroll(WPARAM wParam, LPARAM lParam)
{
	CtrlListItem* cli = GetCtrlListItemByHWND((HWND)lParam);
	return cli ? cli->onWmCommand(wParam, lParam) : 0xFFFF;
}

//	WM_CTLCOLOR �n���h��
HBRUSH
DlgPage::onCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CtrlListItem* cli = GetCtrlListItemByHWND((HWND)lParam);
	return cli ? cli->onWmCtlColor(uMsg, wParam, lParam) : NULL;
}

WORD
DlgPage::onImeNotify(WPARAM wParam, LPARAM lParam)
{
	CtrlListItem* cli = GetCtrlListItemByHWND((HWND)lParam);
	return cli ? cli->onWmImeNotify(wParam, lParam) : 0xFFFF;
}

//	�_�C�A���O�f�[�^�̏�������
BOOL
DlgPage::dumpData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	StringBuffer secname(GetString(STR_DLGDATA_DLGPAGEPROPERTY),-1,6);
	secname.append((TCHAR)'_').append(m_strName);
	ddfile.write(m_strName,GetString(STR_DLGDATA_PAGENAME),secname);
	ddfile.write(m_bEnable,GetString(STR_DLGDATA_ENABLE),secname);
	ddfile.write(m_iniwidth,GetString(STR_DLGDATA_WIDTH),secname);
	int	num = m_pCtrlList->itemNum();
	ddfile.write(num,GetString(STR_DLGDATA_CTRLNUM),secname);
	secname.reset(GetString(STR_DLGDATA_CTRLPROPERTY));
	secname.append((TCHAR)'_').append(m_strName).append((TCHAR)':');
	int	len = secname.length();
	m_pCtrlList->initSequentialGet();
	CtrlListItem* cli;
	for (int i = 0; i < num; i++) {
		secname.setlength(len);
		secname.append(i);
		ddfile.setSecName(secname);
		cli = m_pCtrlList->getNextItem();
		cli->dumpData(ddfile);
	}
	return TRUE;
}

//	�_�C�A���O�f�[�^�̓Ǎ���
BOOL
DlgPage::loadData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	StringBuffer secname(GetString(STR_DLGDATA_DLGPAGEPROPERTY),-1,6);
	secname.append((TCHAR)'_').append(m_strName);
	ddfile.read(&m_bEnable,GetString(STR_DLGDATA_ENABLE),secname);
	ddfile.read(&m_iniwidth,GetString(STR_DLGDATA_WIDTH),secname);
	int	num;
	ddfile.read(&num,GetString(STR_DLGDATA_CTRLNUM),secname);
	secname.reset(GetString(STR_DLGDATA_CTRLPROPERTY));
	secname.append((TCHAR)'_').append(m_strName).append((TCHAR)':');
	int	len = secname.length(), type;
	StringBuffer namebuf(32), textbuf(80);
	for (int i = 0; i < num; i++) {
		secname.setlength(len);
		secname.append(i);
		ddfile.read(&type,GetString(STR_DLGDATA_TYPE),secname);
		ddfile.read(namebuf,GetString(STR_DLGDATA_NAME),secname);
		ddfile.read(textbuf,GetString(STR_DLGDATA_TEXT),secname);
		m_pCurCtrlData = CtrlListItem::createCtrl(type,namebuf,textbuf);
		if (m_pCurCtrlData == NULL) continue;
		ddfile.setSecName(secname);
		if (m_pCurCtrlData->loadData(ddfile)) this->addCtrl(m_pCurCtrlData);
		else {
			delete m_pCurCtrlData;
			m_pCurCtrlData = NULL;
		}
	}
	return TRUE;
}

//	�e�_�C�A���O�̃R�[���o�b�N�֐�
BOOL CALLBACK
DlgFrameProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DlgFrame* pdf;

	if (uMsg == WM_INITDIALOG) {
		//	�e�_�C�A���O�̏�����
		::SetWindowLong(hDlg,DWL_USER,(LONG)lParam);
		pdf = reinterpret_cast<DlgFrame*>(lParam);
		if (!pdf->initFrame(hDlg)) {
			::DestroyWindow(hDlg);
		}
		return FALSE;
	}
	if ((pdf = reinterpret_cast<DlgFrame*>(::GetWindowLong(hDlg,DWL_USER)))
		== NULL) return FALSE;

	switch (uMsg) {
	case WM_USER_MESSAGEBOX:
		if (wParam) {
			//	���b�Z�[�W�̕\��
			StringBuffer* msg = reinterpret_cast<StringBuffer*>(wParam);
			if (::MessageBox(hDlg,
							*msg,
							GetString(STR_USRMSGBOX_MESSAGE),
							MB_OK|MB_ICONINFORMATION) == IDOK) {
				StringBuffer buf(32);
				pdf->setNotify(buf.append((ULONG)lParam));
			}
			delete msg;
		}
		break;

	case WM_USER_QUESTIONBOX:
		if (wParam) {
			//	�m�F���b�Z�[�W�̕\��
			StringBuffer* msg = reinterpret_cast<StringBuffer*>(wParam);
			StringBuffer buf(32);
			if (::MessageBox(hDlg,
							*msg,
							GetString(STR_USRMSGBOX_QUESTION),
							MB_YESNO|MB_ICONQUESTION) == IDYES) {
				buf.append((int)HIWORD(lParam));
			} else {
				buf.append((int)LOWORD(lParam));
			}
			pdf->setNotify(buf);
			delete msg;
		}
		break;

	case WM_USER_NOTIFY:
		{
			//	�q�_�C�A���O����̒ʒm�R�[�h
			pdf->setNotify(StringBuffer(16).append((int)wParam));
		}
		break;

	case WM_USER_GETFOCUSEDCTRL:
		{
			::SetWindowLong(hDlg, DWL_MSGRESULT, (LONG)(LPCSTR)pdf->getFocusedCtrl());
		}
		break;

	case DM_SETDEFID:
		// prevent the default message handling
		break;

	case DM_GETDEFID:
		{
			WORD wDefID = pdf->getDefID();
#if 1
			if (wDefID == 0xFFFF) {
				// �f�t�H���g�v�b�V���{�^�������݂��Ȃ�
				wDefID = IDOK;
			}
#endif
			::SetWindowLong(hDlg, DWL_MSGRESULT, (DC_HASDEFID << 16) | wDefID);
		}
		break;

	case WM_COMMAND:
		{
			//	���^�[���L�[�����������ADlgFrame �� DM_GETDEFID �������A
			//	���̌㑗���Ă��� WM_COMMAND �𐳂��� DlgPage �ɓn��
			int	index = HIBYTE(LOWORD(wParam));	//	ctrlID �� HIBYTE ��
												//	DlgPage �� index
			if (index > 0) {
				DlgPage* pdp = pdf->getPage(--index);
				if (pdp != NULL) {
					::PostMessage(pdp->gethwndPage(),WM_COMMAND,wParam,lParam);
					return TRUE;
				}
			} else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				return ::SendMessage(hDlg,
									WM_USER_NOTIFY,
									(LOWORD(wParam) == IDOK),
									0);
			}
		}
		return FALSE;

	case WM_SYSCOMMAND:
		if (wParam != SC_CLOSE) return FALSE;
		/* Through Down */

	case WM_CLOSE:
		::SendMessage(hDlg,WM_USER_NOTIFY,0,0);	//	"0" ��ʒm
//		::DestroyWindow(hDlg);
		return FALSE;

	case WM_DESTROY:
		{
			HWND hwndOwner = ::GetParent(hDlg);
			if (hwndOwner != NULL) {
//	���[�h���X�_�C�A���O�Ɗ���؂��āA�ȉ��̃R�[�h�͂΂�����(^^;�폜�B
//	�G�ۃE�B���h�E���N���b�N����Ɓu�}�N�����I�����܂����H�v�Əo�邯��
//	�܂����������Ȃ��B
//				::EnableWindow(hwndOwner,TRUE);
				::SetForegroundWindow(hwndOwner);
			}
			pdf->uninitFrame();
			::PostQuitMessage(0L);
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

DlgFrame::DlgFrame()
	:	m_pSessionInstance(NULL),
		m_hwndFrame(NULL),
		m_pThemeWrapper(NULL),
		m_pPageList(new NamedLinkList<DlgPage>),
		m_pCurDlgPage(NULL),
		m_DlgTitle(80),
		m_pFontProp(new FontProperty),
		m_width(0),
		m_height(0),
		m_flags(0)
{
	try {
		m_pThemeWrapper = new ThemeWrapper();
		m_pThemeWrapper->SetThemeAppProperties(STAP_ALLOW_NONCLIENT |
											   STAP_ALLOW_CONTROLS  |
											   STAP_ALLOW_WEBCONTENT);
	} catch (ThemeNotSupportedError&) {
		m_pThemeWrapper = NULL;
	}

	GetDefFontProperty(m_pFontProp);
	m_pos.x = m_pos.y = 0;
}

DlgFrame::~DlgFrame()
{
	if (m_hwndFrame != NULL) ::DestroyWindow(m_hwndFrame);
	delete m_pPageList;
	delete m_pThemeWrapper;
	delete m_pFontProp;
}

void
DlgFrame::setNotify(const StringBuffer& newvalue)
{
	m_pSessionInstance->setNotify(newvalue);
}

BOOL
DlgFrame::getNotify(StringBuffer& buf, DWORD wait) const
{
	return m_pSessionInstance->getNotify(buf,wait);
}

void
DlgFrame::resetNotify()
{
	m_pSessionInstance->resetNotify();
}

void
DlgFrame::setFrameProperty(
	const StringBuffer& title,
	POINTS* ppt,
	WORD wflags,
	const StringBuffer& fontname,
	WORD fontsize,
	WORD wPosUnit)
{
	m_DlgTitle = title;
	if (ppt != NULL) m_pos = *ppt;
	m_flags = wflags;
	if (fontname.length() > 0) m_pFontProp->m_fontname = fontname;
	if (fontsize > 0) m_pFontProp->m_fontsize = fontsize;
	m_wPosUnit = wPosUnit;
}

//	�e�_�C�A���O�̐���
HWND
DlgFrame::createFrame(HWND hwndOwner, BOOL bOnTop)
{
	//	"root" �q�_�C�A���O�̓o�^�̊m�F
	m_pCurDlgPage = this->getPage(strRootPageName);
	if (m_pCurDlgPage == NULL) return NULL;

	//	"root" �q�_�C�A���O�̑傫�����v�Z
	DWORD size = m_pCurDlgPage->evalPageSize();
	m_width = LOWORD(size) + 2;
	m_height = HIWORD(size) + 1;

	//	�e�_�C�A���O�� DLGTEMPLATE �̐���
	int	bufsize = 2 + sizeof(DLGTEMPLATE) + 
//				sizeof(WORD) * (m_DlgTitle.length() + 
//								m_pFontProp->m_fontname.length() + 2) + 
				((m_DlgTitle.length() + 
				 m_pFontProp->m_fontname.length() + 2) << 1) + 
				16;
	Array<BYTE> pDlgTemplate(bufsize);
	pDlgTemplate.zero();

	CreateDialogTemplate((LPDLGTEMPLATE)(BYTE*)pDlgTemplate,
						WS_POPUP|WS_CAPTION|WS_SYSMENU|
							DS_MODALFRAME|DS_SETFONT,
						0L,
						0,
						m_pos.x*4, m_pos.y*8,
						m_width*UWIDTH, m_height*UHEIGHT,
						m_DlgTitle,
						m_pFontProp->m_fontname,
						(WORD)m_pFontProp->m_fontsize);
	if (bOnTop) m_flags |= 0x10;	//	alwaysontop

	//	�e�_�C�A���O�̃E�B���h�E�̐���
	m_hwndFrame = ::CreateDialogIndirectParam(
								m_pSessionInstance->getInstanceHandle(),
								(LPDLGTEMPLATE)(BYTE*)pDlgTemplate,
								hwndOwner,
								(DLGPROC)DlgFrameProc,
								(LPARAM)this);

	return m_hwndFrame;
}

//	�_�C�A���O�̕\��
BOOL
DlgFrame::showFrame()
{
	if (m_hwndFrame == NULL) return FALSE;

	//	�_�C�A���O�̈ʒu�̕ύX
	POINTS pos = GetDlgPosByPixel(m_pos, m_wPosUnit, m_hwndFrame);
	POINTS org = GetDlgPosOrigin(m_flags & 0x000F, m_hwndFrame);

	switch (m_flags & 0x000F) {
	case DLGPOS_SCREEN_COORD:
	case DLGPOS_CLIENT_COORD:
	case DLGPOS_CARET_COORD:
	case DLGPOS_CURSOR_COORD:
		pos.x += org.x;
		pos.y += org.y;
		break;
	case DLGPOS_SCREEN_CENTER:
	case DLGPOS_CLIENT_CENTER:
	default:
		pos = org;
		break;
	}

#if 0
	RECT rect, drect;
	::GetWindowRect(m_hwndFrame, &rect);

	HWND hwndParent = ::GetParent(m_hwndFrame);

	switch (m_flags & 0x000F) {
	case DLGPOS_SCREEN_COORD:
		/* Screen Coordinate */
		break;
	case DLGPOS_CLIENT_COORD:
		/*	Window Coordinate	*/
		::ClientToScreen(hwndParent, &pos);
		break;
	case DLGPOS_SCREEN_CENTER:
		/*	Screen Center	*/
		::GetWindowRect(::GetDesktopWindow(), &drect);
		pos.x = ((drect.left + drect.right) >> 1)
				- ((rect.right - rect.left) >> 1);
		pos.y = ((drect.top + drect.bottom) >> 1)
				- ((rect.bottom - rect.top) >> 1);
		break;
	case DLGPOS_CARET_COORD:
		/*  Caret Coordinate  */
		if (::GetCaretPos(&cpos)) {
			::ClientToScreen(hwndParent, &cpos);
			pos.x += cpos.x;
			pos.y += cpos.y;
		}
		break;
	case DLGPOS_CURSOR_COORD:
		/*  Cursor Coordinate  */
		if (::GetCursorPos(&cpos)) {
			pos.x += cpos.x;
			pos.y += cpos.y;
		}
		break;
	case DLGPOS_CLIENT_CENTER:
	default:
		::GetWindowRect(hwndParent, &drect);
		pos.x = ((drect.left + drect.right) >> 1)
				- ((rect.right - rect.left) >> 1);
		pos.y = ((drect.top + drect.bottom) >> 1)
				- ((rect.bottom - rect.top) >> 1);
	}
#endif

	HWND hwndIA = HWND_TOP;
	UINT uFlag = SWP_NOSIZE;
	if ((m_flags & 0x10) != 0) hwndIA = HWND_TOPMOST;	//	alwaysontop
	::SetWindowPos(m_hwndFrame, hwndIA, pos.x, pos.y, 0, 0, uFlag);

//	���[�h���X�_�C�A���O�Ɗ���؂��āA�ȉ��̃R�[�h�͂΂�����(^^;�폜�B
//	�G�ۃE�B���h�E���N���b�N����Ɓu�}�N�����I�����܂����H�v�Əo�邯��
//	�܂����������Ȃ��B
//	HWND hwndOwner = ::GetParent(m_hwndFrame);
//	if (hwndOwner != NULL) ::EnableWindow(hwndOwner,FALSE);

	// �f�t�H���g�t�H�[�J�X�̐ݒ�
//	this->setFocusedCtrl(m_sbFocusedCtrl);
	DlgPage* pdproot = this->getPage(strRootPageName);
	if (pdproot != NULL) {
		pdproot->showPage(TRUE);
	}
	::ShowWindow(m_hwndFrame, SW_SHOW);
	// �f�t�H���g�t�H�[�J�X�̐ݒ�
	this->setFocusedCtrl(m_sbFocusedCtrl);

	return TRUE;
}

//	�e�_�C�A���O�̔j��
BOOL
DlgFrame::closeFrame()
{
	if (m_hwndFrame == NULL) return FALSE;
	BOOL ret = !::SendMessage(m_hwndFrame,WM_DESTROY,0,0);
	m_hwndFrame = NULL;
	return ret;
}

//	�e�_�C�A���O�f�[�^�̍ď�����
void
DlgFrame::resetFrame()
{
	if (m_hwndFrame != NULL) return;
	m_pCurDlgPage = NULL;
	m_DlgTitle.reset();
	GetDefFontProperty(m_pFontProp);
	m_pos.x = m_pos.y = 0;
	m_width = 0;
	m_flags = 0;
}

//	�e�_�C�A���O�̏�����
BOOL
DlgFrame::initFrame(HWND hDlg)
{
	if (hDlg == NULL) return FALSE;

	m_hwndFrame = hDlg;

	//	"root" �q�_�C�A���O�̐���
	return this->createPage(strRootPageName,
							NULL,
							UWIDTH,UHEIGHT / 2,
							m_width-2,m_height - 1,
							false) != NULL;
}

//	�e�_�C�A���O�̔j��
BOOL
DlgFrame::uninitFrame()
{
	m_hwndFrame = NULL;
	if (m_pPageList != NULL) {
		DlgPage* pDlgPage;
		m_pPageList->initSequentialGet();
		while ((pDlgPage = m_pPageList->getNextItem()) != NULL)
			pDlgPage->uninitPage();
	}
	return TRUE;
}

//	�q�_�C�A���O�̒ǉ�
int
DlgFrame::addPage(const StringBuffer& name, WORD width)
{
	if (name.length() <= 0 || m_hwndFrame != NULL) return 0;
	m_pCurDlgPage = new DlgPage(this,name,width);	//	Poke �̑Ώ�
	return m_pPageList->addItem(m_pCurDlgPage);
}

//	�R���g���[���ǉ��Ώۂ̎q�_�C�A���O�̕ύX
BOOL
DlgFrame::setCurrentPage(const StringBuffer& name)
{
	if (name.length() <= 0 || m_hwndFrame != NULL) return FALSE;
	DlgPage* pdp = this->getPage(name);
	if (pdp == NULL) return FALSE;
	m_pCurDlgPage = pdp;
	return TRUE;
}

//	�q�_�C�A���O�E�B���h�E�̐���
HWND
DlgFrame::createPage(
	const StringBuffer& name,
	HWND hWnd,
	WORD ofx, WORD ofy,
	WORD cx, WORD cy,
	bool bInTabCtrl)
{
	//	ofx, ofy �̓_�C�A���O�P��(��=������/4, �c=������/8)�A
	//	cx, cy �͕����P�ʂŗ^������
	if (m_hwndFrame == NULL) return NULL;
	DlgPage* pdp = this->getPage(name);
	if (pdp == NULL) return NULL;
	//	�R���g���[����ID�� HIBYTE() �ɂ͎q�_�C�A���O����index������
	BYTE index = (BYTE)(m_pPageList->getItemIndexByPtr(pdp) + 1);
	return pdp->createPage(hWnd, index,
						   ofx, ofy, cx*UWIDTH, cy*UHEIGHT,
						   bInTabCtrl, *m_pFontProp);
}

//	�R���g���[���𖼑O�Ŏ擾
CtrlListItem*
DlgFrame::getCtrl(const StringBuffer& ctrlname, const StringBuffer& pagename)
{
	if (ctrlname.length() <= 0 && pagename.length() <= 0) {
		//	���݂̎q�_�C�A���O���猻�݂̃R���g���[�����擾
		if (m_pCurDlgPage == NULL) return NULL;
		return m_pCurDlgPage->getCtrl();
	} else if (ctrlname.length() > 0) {
		//	�w�肳�ꂽ�R���g���[����S�Ă̎q�_�C�A���O���猟������
		DlgPage* pdp;
		CtrlListItem* cli = NULL;
		m_pPageList->initSequentialGet();
		while ((pdp = m_pPageList->getNextItem()) != NULL) {
			if ((cli = pdp->getCtrl(ctrlname)) != NULL) break;
		}
		return cli;
	} else {
		//	�w��q�_�C�A���O����w��R���g���[�����擾
		DlgPage* pdp = m_pPageList->getItemByName(pagename);
		if (pdp == NULL) return NULL;
		return pdp->getCtrl(ctrlname);
	}
	//	not reached.
}

int
DlgFrame::setFocusedCtrl(const StringBuffer& name)
{
	if (name.length() <= 0) {
		m_sbFocusedCtrl = nullStr;
		if (m_hwndFrame != NULL) {
			DlgPage* pdproot = this->getPage(strRootPageName);
			if (pdproot == NULL) return FALSE;
			::SetFocus(pdproot->getFocusedCtrl());
		}
	} else {
		CtrlListItem* pctrl = this->getCtrl(name);
		if (pctrl == NULL) return FALSE;
		m_sbFocusedCtrl = name;
		if (m_hwndFrame != NULL) {
			HWND hwndFocused = pctrl->getCtrlHWND();
			if (hwndFocused == NULL) return FALSE;
			::SetFocus(hwndFocused);
		}
	}
	return TRUE;
}

const StringBuffer&
DlgFrame::getFocusedCtrl() const
{
	if (m_hwndFrame == NULL) return m_sbFocusedCtrl;
	HWND hCtrl = ::GetFocus();
	if (hCtrl != NULL) {
		CtrlListItem*
			pCtrl = (CtrlListItem*)::SendMessage(hCtrl, WM_GET_CTRL_PTR, 0, 0);
		if (pCtrl != NULL && pCtrl->isValid()) {
			return pCtrl->getName();
		}
	}
	return nullStr;
}

WORD
DlgFrame::getDefID() const
{
	if (m_hwndFrame == NULL) return 0xFFFF;

	DlgPage* pdpRoot = ((DlgFrame*)this)->getPage(strRootPageName);
	if (pdpRoot == NULL) return 0xFFFF;

	return pdpRoot->getDefID();
}

POINTS
DlgFrame::getDlgPos() const
{
	if (!m_hwndFrame) {
		return m_pos;
	}

	RECT rcFrame;
	::GetWindowRect(m_hwndFrame, &rcFrame);

	POINTS pos;
	pos.x = (WORD)rcFrame.left;  pos.y = (WORD)rcFrame.top;

	POINTS org = GetDlgPosOrigin(m_flags & 0x000F, m_hwndFrame);
	pos.x -= org.x;  pos.y -= org.y;

	pos = GetDlgPosByUnit(pos, m_wPosUnit, m_hwndFrame);

	return pos;
}

void
DlgFrame::setDlgPos(const POINTS& pos, WORD wOrigin, WORD wUnit)
{
	m_pos = pos;
	m_wPosUnit = wUnit;
	m_flags = (m_flags & 0xF0) | wOrigin;

	if (m_hwndFrame) {
		POINTS newpos = GetDlgPosByPixel(m_pos, m_wPosUnit, m_hwndFrame);
		POINTS org = GetDlgPosOrigin(m_flags & 0xF, m_hwndFrame);
		newpos.x += org.x;  newpos.y += org.y;
		HWND hwndIA = HWND_TOP;
		UINT uFlag = SWP_NOSIZE;
		if ((m_flags & 0x10) != 0) hwndIA = HWND_TOPMOST;	//	alwaysontop
		::SetWindowPos(m_hwndFrame, hwndIA, newpos.x, newpos.y, 0, 0, uFlag);
	}
}

//	�_�C�A���O�f�[�^�̏�������
BOOL
DlgFrame::dumpData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	LPCSTR	secname = GetString(STR_DLGDATA_DLGFRAMEPROPERTY);
	ddfile.write(m_DlgTitle,GetString(STR_DLGDATA_TITLE),secname);
	ddfile.write(m_pFontProp->m_fontsize,
				GetString(STR_DLGDATA_FONTSIZE),secname);
	ddfile.write(m_pFontProp->m_fontname,
				GetString(STR_DLGDATA_FONTNAME),secname);
	ddfile.write(m_pos.x,GetString(STR_DLGDATA_PLACEX),secname);
	ddfile.write(m_pos.y,GetString(STR_DLGDATA_PLACEY),secname);
	ddfile.write(m_width,GetString(STR_DLGDATA_WIDTH),secname);
	ddfile.write(m_flags,GetString(STR_DLGDATA_FLAGS),secname);
	m_pPageList->initSequentialGet();
	DlgPage* pdp;
	StringBuffer buf(GetString(STR_DLGDATA_PAGENAME),-1,6);
	buf.append((TCHAR)'_');
	int	len = buf.length(), num = m_pPageList->itemNum();
	for (int i = 0; i < num; i++) {
		pdp = m_pPageList->getNextItem();
		buf.setlength(len);
		buf.append(i);
		ddfile.write(pdp->getName(),buf,secname);
		pdp->dumpData(ddfile);
	}
	ddfile.write(num,GetString(STR_DLGDATA_PAGENUM),secname);
	return TRUE;
}

//	�_�C�A���O�f�[�^�̓Ǎ���
BOOL
DlgFrame::loadData(DlgDataFile& ddfile)
{
	if (!ddfile.isValid()) return FALSE;
	LPCSTR secname = GetString(STR_DLGDATA_DLGFRAMEPROPERTY);
	ddfile.read(m_DlgTitle,GetString(STR_DLGDATA_TITLE),secname);
	ddfile.read(&m_pFontProp->m_fontsize,
				GetString(STR_DLGDATA_FONTSIZE),secname);
	ddfile.read(m_pFontProp->m_fontname,
				GetString(STR_DLGDATA_FONTNAME),secname);
	ddfile.read((WORD*)&m_pos.x,GetString(STR_DLGDATA_PLACEX),secname);
	ddfile.read((WORD*)&m_pos.y,GetString(STR_DLGDATA_PLACEY),secname);
	ddfile.read(&m_width,GetString(STR_DLGDATA_WIDTH),secname);
	ddfile.read(&m_flags,GetString(STR_DLGDATA_FLAGS),secname);
	int	num;
	ddfile.read(&num,GetString(STR_DLGDATA_PAGENUM),secname);
	DlgPage* pdp;
	StringBuffer keybuf(GetString(STR_DLGDATA_PAGENAME),-1,6);
	keybuf.append((TCHAR)'_');
	int	len = keybuf.length();
	StringBuffer buf(80);
	for (int i = 0; i < num; i++) {
		keybuf.setlength(len);
		keybuf.append(i);
		ddfile.read(buf,keybuf,secname);
		if (buf.length() <= 0) continue;
		this->addPage(buf,1);
		pdp = this->getPage(buf);
		pdp->loadData(ddfile);
	}
	return TRUE;
}

