//	$Id: dlgdata.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
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

//	�R���g���[�����̃��X�g
typedef	HashTable<CtrlListItem*,31>	CtrlHashTable;

//	�_�C�A���O������������ۂɕ\�����ׂ��q�_�C�A���O��o�^����L���[�̃A�C�e��
class ShowPageQueueItem {
public:
	ShowPageQueueItem(DlgPage* pdp, HWND hwndCtrl,
					WORD x, WORD y, WORD cx, WORD cy)
		:	m_pdp(pdp), m_hwndCtrl(hwndCtrl),
			m_x(x), m_y(y), m_cx(cx), m_cy(cy)
	{}
	DlgPage* m_pdp;
	HWND m_hwndCtrl;
	WORD m_x;
	WORD m_y;
	WORD m_cx;
	WORD m_cy;
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

//	�_�C�A���O�̍\�z/������
	DWORD evalPageSize();	//	m_width, m_height ���v�Z
	//	�q�_�C�A���O�̍\�z
	HWND createPage(HWND, BYTE, WORD, WORD, WORD, WORD, FontProperty&);
	BOOL initPage(HWND);	//	�q�_�C�A���O�̏���������
	BOOL uninitPage();	//	�q�_�C�A���O�̔j�����̏���
	BOOL showPage(BOOL);	//	�q�_�C�A���O�̉���
	BOOL enablePage(BOOL);	//	�q�_�C�A���O�̗L����
	BOOL addShowPageQueue(LPCSTR pname, HWND hwndCtrl,
							WORD x, WORD y, WORD cx, WORD cy);

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
	HBRUSH onCtlColor(WPARAM, LPARAM);
	WORD onImeNotify(WPARAM, LPARAM);

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
					WORD fontsize = 0
				);

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
					WORD ofx, WORD ofy, WORD cx, WORD cy
				);	//	�q�_�C�A���O�̃E�B���h�E�쐬

//	�R���g���[���̎擾
	CtrlListItem* getCtrl(const StringBuffer& ctrlname = nullStr,
						const StringBuffer& pagename = nullStr);

//	�f�[�^�̏������݁E�Ǎ���
	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

private:
	SessionInstance* m_pSessionInstance; // �Z�b�V�����C���X�^���X�ւ̃|�C���^
	CtrlIdHashTable	m_HashCtrl; // �R���g���[�������h�c
	HWND m_hwndFrame;	//	�E�B���h�E�n���h��
	NamedLinkList<DlgPage>* m_pPageList;	//	�q�_�C�A���O�̃��X�g
	DlgPage* m_pCurDlgPage;	//	���ݏ����Ώۂ̎q�_�C�A���O
	StringBuffer m_DlgTitle;	//	�_�C�A���O�̃^�C�g��
	LPFontProperty m_pFontProp;	//	�_�C�A���O�̃t�H���g����
	POINTS m_pos;	//	�_�C�A���O�̕\���ʒu
	WORD m_width;	//	�_�C�A���O�̕�
	WORD m_height;	//	�_�C�A���O�̍���
	WORD m_flags;	//	�\���ʒu�Ɋւ���t���O
};

extern const StringBuffer strRootPageName;

#endif