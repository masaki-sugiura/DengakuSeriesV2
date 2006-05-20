//	$Id: dlgdata.h,v 1.16 2006-05-20 17:02:50 sugiura Exp $
/*
 *	dlgdata.h
 *	�_�C�A���O�������N���X
 */

/*
	�_�C�A���O�̎����ɂ��ẴR�����g�F

	�S�ẴR���g���[���E�B���h�E�� DlgPage �N���X�ɑ�����E�B���h�E
	(�q�_�C�A���O)�̎q�E�B���h�E�ł���B�����āA�S�Ă̎q�_�C�A���O��
	DlgFrame �N���X�ɑ�����E�B���h�E(�e�_�C�A���O)�̎q�E�B���h�E�ł���B
	�Ȃ��Anewdialog(ex) �ō쐬�����_�C�A���O�� "root" �Ƃ������O��
	�q�_�C�A���O�ł���B
*/

#ifndef	DENGAKUSERIES_CLASSES_DIALOG
#define	DENGAKUSERIES_CLASSES_DIALOG

#include "linklist.h"
#include "ctrlname.h"
#include "ThemeWrapper.h"

class SessionInstance;
class DlgPage;
class DlgFrame;
class CtrlListItem;
class DlgDataFile;

typedef struct {
	SessionInstance* m_psi;
	HWND m_hwndOwner;
	BOOL m_bOnTop;
} SHOWDLGPROCARGS, *LPSHOWDLGPROCARGS;

//	�_�C�A���O�̃t�H���g�����i�[����\����
typedef struct FontProperty_tag	{
	int				m_fontsize;		//	�|�C���g��
	StringBuffer	m_fontname;	//	�t�H���g���̃o�b�t�@
	FontProperty_tag() : m_fontsize(0), m_fontname(32) {}
} FontProperty, *LPFontProperty;

enum {
	DLGPOS_CLIENT_CENTER = 0,
	DLGPOS_SCREEN_CENTER = 1,
	DLGPOS_CLIENT_COORD  = 2,
	DLGPOS_SCREEN_COORD  = 3,
	DLGPOS_CARET_COORD   = 4,
	DLGPOS_CURSOR_COORD  = 5
};

// �_�C�A���O�\���ʒu�w��̒P��
enum {
	DLGPOS_UNIT_FONTSIZE = 0,
	DLGPOS_UNIT_PIXEL    = 1
};

//	�R���g���[�����̃��X�g
typedef	HashTable<CtrlListItem*,31>	CtrlHashTable;

//	�_�C�A���O������������ۂɕ\�����ׂ��q�_�C�A���O��o�^����L���[�̃A�C�e��
class ShowPageQueueItem {
public:
	ShowPageQueueItem(DlgPage* pdp, HWND hwndCtrl,
					  WORD x, WORD y, WORD cx, WORD cy,
					  bool bInTabCtrl)
		:	m_pdp(pdp), m_hwndCtrl(hwndCtrl),
			m_x(x), m_y(y), m_cx(cx), m_cy(cy),
			m_bInTabCtrl(bInTabCtrl)
	{}
	DlgPage* m_pdp;
	HWND m_hwndCtrl;
	WORD m_x;
	WORD m_y;
	WORD m_cx;
	WORD m_cy;
	bool m_bInTabCtrl;
};

//	�q�_�C�A���O��\���N���X
class DlgPage {
public:
	DlgPage(DlgFrame *pdf, const StringBuffer &name, WORD width);
	~DlgPage();

//	�����o�l�̎擾
	const StringBuffer&	getName() const { return m_strName; }
	DlgFrame& getDlgFrame(){ return *m_pDlgFrame; }
	HWND gethwndPage() const { return m_hwndPage; }
	WORD getWidth() const { return m_width; }
	WORD getHeight() const { return m_height; }
	WORD getDefID() const;
	bool isInTabCtrl() const { return m_bInTabCtrl; }

//	�_�C�A���O�̍\�z/������
	DWORD evalPageSize();	//	m_width, m_height ���v�Z
	//	�q�_�C�A���O�̍\�z
	HWND createPage(HWND, BYTE, WORD, WORD, WORD, WORD, bool, FontProperty&);
	BOOL initPage(HWND);	//	�q�_�C�A���O�̏���������
	BOOL uninitPage();	//	�q�_�C�A���O�̔j�����̏���
	BOOL showPage(BOOL);	//	�q�_�C�A���O�̉���
	BOOL enablePage(BOOL);	//	�q�_�C�A���O�̗L����
	BOOL addShowPageQueue(LPCSTR pname, HWND hwndCtrl,
						  WORD x, WORD y, WORD cx, WORD cy,
						  bool bInTabCtrl);

//	�R���g���[���̒ǉ��E�擾
	int addCtrl(CtrlListItem*);	//	�R���g���[���̒ǉ�
	int addCtrl(int type, const StringBuffer& name, const StringBuffer& text);
	//	�R���g���[���̎擾
	CtrlListItem* getCtrl(const StringBuffer& name = nullStr);
	HWND getFocusedCtrl();

//	���b�Z�[�W�n���h��
	WORD onCommand(WPARAM, LPARAM);
	WORD onNotify(WPARAM, LPARAM);
	WORD onHScroll(WPARAM, LPARAM);
	WORD onImeNotify(WPARAM, LPARAM);
	HBRUSH onCtlColor(UINT, WPARAM, LPARAM);

//	�f�[�^�̏������݁E�ǂݏo��
	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

	friend class DlgFrame;

private:
	DlgFrame* m_pDlgFrame;	//	�e�_�C�A���O�ւ̃|�C���^
	const StringBuffer m_strName;	//	�_�C�A���O�y�[�W��
	HWND m_hwndPage;		//	�E�B���h�E�n���h��
	DWORD m_dwPageSize;	//	DLGTEMPLATE �̑傫��
	LinkList<CtrlListItem>* m_pCtrlList;	//	�R���g���[���̃��X�g
	CtrlHashTable* m_pHashCtrl;	//	�R���g���[�����̃��X�g
	CtrlListItem* m_pCurCtrlData;	//	����ΏۂƂȂ�R���g���[���ւ̃|�C���^
	LinkList<ShowPageQueueItem>* m_pShowPageQueue;
	BOOL m_bVisible;	//	�E�B���h�E�������ǂ���
	BOOL m_bEnable;		//	�E�B���h�E���L�����ǂ���
	WORD m_cdit;		//	���ۂ̃R���g���[����
						//	(!=m_pCtrlList->itemNum() �ɒ���!!)
	WORD m_iniwidth;	//	�_�C�A���O�̏�����
	WORD m_width;		//	�_�C�A���O�̕�
	WORD m_height;		//	�_�C�A���O�̍���
	bool m_bInTabCtrl;  //  Tab �R���g���[���̓����ɕ\������邩�ǂ���
};

//	�R���g���[��������R���g���[�����ʂh�c�����߂�n�b�V���e�[�u��
typedef HashTable<int,61> CtrlIdHashTable;

//	�e�_�C�A���O��\���N���X
class DlgFrame {
public:
	DlgFrame();
	~DlgFrame();

	void setSessionInstance(SessionInstance* psi)
	{
		m_pSessionInstance = psi;
	}
	SessionInstance* getSessionInstance() const { return m_pSessionInstance; }

	void setNotify(const StringBuffer& newvalue);	//	notify code �̒ʒm
	BOOL getNotify(StringBuffer& buf, DWORD wait) const;
	void resetNotify();

//	�����o�l�̎擾�E�ύX
	HWND getUserDlg() const { return m_hwndFrame; }
	const StringBuffer&	getDlgTitle() const { return m_DlgTitle; }
	void setDlgTitle(const StringBuffer& title)
	{
		m_DlgTitle = title;
		if (::IsWindow(m_hwndFrame)) ::SetWindowText(m_hwndFrame,m_DlgTitle);
	}
	void setFrameProperty(
					const StringBuffer& title,
					POINTS* ppt = NULL,
					WORD wflags = 0,
					const StringBuffer& fontname = nullStr,
					WORD fontsize = 0,
					WORD wPosUnit = DLGPOS_UNIT_FONTSIZE
				);
	const StringBuffer& getFontName() const
	{
		return m_pFontProp->m_fontname;
	}
	int getFontSize() const
	{
		return m_pFontProp->m_fontsize;
	}
	WORD getDefID() const;

	POINTS getDlgPos() const;
	void setDlgPos(const POINTS& pos, WORD wOrigin, WORD wPosUnit);

//	�_�C�A���O�̍\�z�E�������E�j��
	HWND createFrame(HWND hwndOwner, BOOL bOnTop = FALSE); // �_�C�A���O�̍\�z
	BOOL showFrame();	//	�_�C�A���O�̕\��
	BOOL closeFrame();	//	�_�C�A���O�̔j��
	void resetFrame();	//	�_�C�A���O�f�[�^�̍ď�����
	BOOL initFrame(HWND);	//	�_�C�A���O�̏�����(WM_INITDIALOG �n���h��)
	BOOL uninitFrame();	//	�_�C�A���O�̔j��������

//	�q�_�C�A���O�̒ǉ��E�擾�E�쐬
	int addPage(const StringBuffer&, WORD);	//	�q�_�C�A���O�̒ǉ�
	BOOL setCurrentPage(const StringBuffer&); // �ǉ��Ώۂ̎q�_�C�A���O�̕ύX
	DlgPage* getPage(const StringBuffer& name = nullStr) // �q�_�C�A���O�̎擾
	{
		return (name.length() > 0) ?
			m_pPageList->getItemByName(name) : m_pCurDlgPage;
	}
	DlgPage* getPage(int index)	//	�q�_�C�A���O�̎擾
	{
		return m_pPageList->getItemByIndex(index);
	}
	HWND createPage(
					const StringBuffer& name,
					HWND hWnd,
					WORD ofx, WORD ofy, WORD cx, WORD cy,
					bool bInTabCtrl
				);	//	�q�_�C�A���O�̃E�B���h�E�쐬

//	�R���g���[���̎擾
	CtrlListItem* getCtrl(const StringBuffer& ctrlname = nullStr,
						const StringBuffer& pagename = nullStr) const;

//	�t�H�[�J�X��(�Ă�)��R���g���[��
	int setFocusedCtrl(const StringBuffer& name);
	const StringBuffer& getFocusedCtrl() const;

	// �����Ƀt�H�[�J�X�𓾂�R���g���[���̖��O��Ԃ����\�b�h
	const StringBuffer& getFocusedCtrlName() const
	{
		return m_sbFocusedCtrl;
	}

	int setImeState(int nState);
	int getImeState() const
	{
		return m_imestate;
	}
	void setImeAlreadyFocused()
	{
		m_bAlreadyFocused = TRUE;
	}
	BOOL isImeAlreadyFocused() const
	{
		return m_bAlreadyFocused;
	}

	//	�R���g���[�����̃t�H�[�J�X�𓾂鍀�ڂ��w�聕�擾
	int setCtrlFocusedItem(const StringBuffer& ctrl, const StringBuffer& item);
	StringBuffer getCtrlFocusedItem(const StringBuffer& ctrl) const;

//	�_�C�A���O�̔w�i��TAB�R���g���[���Ɠ��F�ɂ���
	void setBackGroundTheme(HWND hwndPage, BOOL bInTab)
	{
		if (m_pThemeWrapper) {
			m_pThemeWrapper->EnableThemeDialogTexture(hwndPage,
													  bInTab ? ETDT_ENABLETAB : ETDT_ENABLE);
		}
	}
	void drawThemeParentBackground(HWND hwndCtrl, HDC hDC, const RECT* pRect)
	{
		if (m_pThemeWrapper) {
			m_pThemeWrapper->DrawThemeParentBackground(hwndCtrl, hDC, pRect);
		}
	}
	void setWindowTheme(HWND hwndCtrl)
	{
		if (m_pThemeWrapper) {
			m_pThemeWrapper->SetWindowTheme(hwndCtrl, NULL, NULL);
		}
	}
	BOOL getThemeColor(HWND hwndPage, LPCWSTR pwszClass, int iPart, int iState, int iProp, COLORREF* pCRef)
	{
		if (!m_pThemeWrapper) return FALSE;
		HTHEME hTheme = m_pThemeWrapper->OpenThemeData(hwndPage, pwszClass);
		if (!hTheme) {
//			::MessageBox(NULL, "hTheme == NULL", NULL, MB_OK);
			return FALSE;
		}
		HRESULT hr = m_pThemeWrapper->GetThemeColor(hTheme, iPart, iState, iProp, pCRef);
		m_pThemeWrapper->CloseThemeData(hTheme);
		return hr == S_OK;
	}
	BOOL isThemeActive()
	{
		if (!m_pThemeWrapper) return FALSE;
		return m_pThemeWrapper->IsThemeActive();
	}

//	�f�[�^�̏������݁E�Ǎ���
	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

private:
	SessionInstance* m_pSessionInstance; // �Z�b�V�����C���X�^���X�ւ̃|�C���^
	CtrlIdHashTable	m_HashCtrl; // �R���g���[�������h�c
	HWND m_hwndFrame;	//	�E�B���h�E�n���h��
	ThemeWrapper* m_pThemeWrapper;
	NamedLinkList<DlgPage>* m_pPageList;	//	�q�_�C�A���O�̃��X�g
	DlgPage* m_pCurDlgPage;	//	���ݏ����Ώۂ̎q�_�C�A���O
	StringBuffer m_DlgTitle;	//	�_�C�A���O�̃^�C�g��
	StringBuffer m_sbFocusedCtrl; // �t�H�[�J�X�𓾂�R���g���[����
	LPFontProperty m_pFontProp;	//	�_�C�A���O�̃t�H���g����
	POINTS m_pos;	//	�_�C�A���O�̕\���ʒu
	WORD m_wPosUnit;	// m_pos �����o�̒P��
	WORD m_width;	//	�_�C�A���O�̕�
	WORD m_height;	//	�_�C�A���O�̍���
	WORD m_flags;	//	�\���ʒu�Ɋւ���t���O
	int m_imestate;	//	IME�̏�Ԃ�\��
	BOOL m_bAlreadyFocused; // ���̓t�H�[�J�X���������ꂩ�̃R���g���[���Ƀt�H�[�J�X���ڂ������ǂ���
};

extern const StringBuffer strRootPageName;
extern const StringBuffer okStr;
extern const StringBuffer ngStr;

#endif
