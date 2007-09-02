//	$Id: ctrldata.h,v 1.25 2007-09-02 15:50:03 sugiura Exp $
/*
 *	ctrldata.h
 *	�R���g���[���������N���X
 */

/*
	�R���g���[���N���X�̎����ɂ��ẴR�����g�F

	�͂����茾���Ĕ������Ȃ��B�����ѐ����Ȃ��B
	���삳���邱�Ƃ����Ȃ̂ō��͂ق����炩���ɂ��Ă��邪�A
	������̓N���X�݌v���獪�{�I�ɂ�蒼���\��B

	�N���X�K�w�}�b�v(�p���͑S�� public)�F

	CtrlListItem				�S�ẴR���g���[���N���X��(����)���N���X
								�S�ẴC���^�[�t�F�C�X���`����B
	PageFormatCtrl : CtrlListItem	newcolumn, newpage
	SimpleCtrl : CtrlListItem	�����ɃR���g���[���E�B���h�E���P����(����)
								���N���X(�c�̗\�肾�������ARadioCtrl ������
								contract �Ɉᔽ���Ă���c�������Ȃ�(ToT))
		BtnCtrl : SimpleCtrl		button, defbutton
			CheckCtrl : BtnCtrl		check, rdbtn
		TextCtrl : SimpleCtrl		text
		EditCtrl : SimpleCtrl		edit, mledit, pwdedit
		LineCtrl : SimpleCtrl		hline, vline
		TrackCtrl : SimpleCtrl		track
		TreeCtrl : SimpleCtrl		tree
		HasListCtrl : SimpleCtrl	item �R�}���h���󂯕t����(����)���N���X�B
									(����܂� TreeCtrl �� contract �ᔽ(ToT))
			RefBtnCtrl : HasListCtrl	reffilebutton, refdirbutton, refcolorbutton
			RadioCtrl : HasListCtrl		radio
			ListCtrl : HasListCtrl		list
				ComboCtrl : ListCtrl	combo, cmblist
			ChkListCtrl : HasListCtrl	chklist
				LViewCtrl : ChkListCtrl	lview
			FrameCtrl : HasListCtrl		frame, group
				TabCtrl : FrameCtrl		tab
	MultipleCtrl : CtrlListItem	�����̃R���g���[���E�B���h�E������(����)���
								�N���X�B
		SpinCtrl : MultipleCtrl		spin
		OkCancelCtrl : MultipleCtrl	okcancel

	����炩��A������̃N���X�K�w�͂���Ȋ����ɂȂ邩�ȁH�H
	CtrlContainer	:	�S�Ắi���[�U�[���猩���j�R���g���[����\���N���X
	�ȉ��� Windows ��`�̃R���g���[���ɑΉ�����(�ėp)�N���X(�Ƃ��������b�p�[)
		SimpleTextCtrl	:	text, line, frame
		ButtonCtrl		:	button, check, radio, group
		EditBoxCtrl		:	edit, multi-line edit
		ListBoxCtrl		:	list
		ComboBoxCtrl	:	combo
		TrackBarCtrl	:	track
		TreeViewCtrl	:	tree
		ListViewCtrl	:	listview, checklist
		SpinButtonCtrl	:	spin
		TabCtrl			:	tab

*/

#ifndef	DENGAKUSERIES_CLASSES_CONTROL
#define	DENGAKUSERIES_CLASSES_CONTROL

#include "strutils.h"
#include "ddfile.h"
#include "linklist.h"
#include "auto_ptr.h"
#include "bitarray.h"
#include "ctrlname.h"

#include <commctrl.h>

class DlgPage;
class CmdLineParser;

//	�_�C�A���O�P�ʂŌv�����s�E��
#define	UWIDTH	4		//	���̒����̂P�P��(������)
#define	UHEIGHT	4		//	�c�̒����̂P�P��(�������̔���)
#define	OWIDTH	2		//	���̗]���̒���
//	#define	OHEIGHT	4		//	�c�̗]���̒���
#define	STRICTHEIGHT	2	//	�����ȂP�����̍���
#define	NARROWHEIGHT	3	//	�c�������Ȃ��R���g���[���̍���
#define	NORMALHEIGHT	4	//	���ʂ̃R���g���[���̍���

#define WM_GET_CTRL_PTR  (WM_USER + 2000)
#define CTRL_PTR_MAGIC   0x5A5AA5A5

//	���ڂ�\���N���X
class ItemData {
public:
	ItemData(const StringBuffer& str = nullStr)
		: m_text(str)
	{}	//	nothing to do more.
	virtual ~ItemData() {}

	virtual	StringBuffer& getText(){ return m_text; }

	virtual int getViewIndex() const { return m_nViewIndex; }
	virtual void setViewIndex(int nViewIndex)
	{
		m_nViewIndex = nViewIndex;
	}

protected:
	StringBuffer m_text;
	int m_nViewIndex;

	ItemData(const ItemData&);
	ItemData& operator=(const ItemData&);
};

//	���O�Ŏ��ʂł��鍀�ڂ�\���N���X
class NamedItemData : public ItemData {
public:
	NamedItemData(const StringBuffer& name,
				const StringBuffer& text = nullStr)
		: ItemData(text), m_name(name)
	{}	//	nothing to do more.

	const StringBuffer& getName() const { return m_name; }

protected:
	const StringBuffer m_name;
};

//	lview �R���g���[���̍��ڂ�\���N���X
class LViewItemData : public ItemData, public LinkList<ItemData> {
//	nothing more.
};

typedef	HashTable<class TreeItemData*,11> TreeHashTable;

//	tree �R���g���[���̍��ڂ�\���N���X
class TreeItemData : public NamedItemData, public NamedLinkList<TreeItemData> {
public:
	TreeItemData(const StringBuffer& name,
				const StringBuffer& text = nullStr)
		:	NamedItemData(name,text), m_parent(NULL)
	{}	//	nothing to do more.

	HTREEITEM m_hItem;

	int	addItem(TreeItemData* tid, int ind = -1)
	{
		tid->m_parent = this;
		return LinkList<TreeItemData>::addItem(tid,ind);
	}

	TreeItemData* getParent() const { return m_parent; }

	BOOL initItem(HWND, HTREEITEM, BOOL);
	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&, TreeHashTable&);

protected:
	TreeItemData* m_parent;
};

//	�ł���{�I�ȃR���g���[����\�����N���X
class CtrlListItem {
public:
	struct CtrlTemplateArgs {
		WORD*	m_hdr;	//	DLGITEMTEMPLATE �\���̂̃w�b�_�A�h���X
		WORD	m_id;	//	�R���g���[���h�c
		WORD	m_x;	//	�R���g���[���̍���� x ���W
		WORD	m_y;	//	�R���g���[���̍���� y ���W
		WORD	m_cx;	//	�R���g���[���̕�
		WORD	m_cy;	//	�R���g���[���̍���
	};

	//	�t�H���g������\���\����
	struct CtrlFontProperty {
		HFONT		 m_hfont;
		BOOL		 m_bchanged;
		COLORREF	 m_color;
		BYTE		 m_fface;
		StringBuffer m_fname;

		CtrlFontProperty()
			: m_hfont(NULL), m_bchanged(TRUE), m_color(0), m_fface(0), m_fname(nullStr)
		{}

		BOOL dumpData(DlgDataFile&, StringBuffer& key);
		BOOL loadData(DlgDataFile&, StringBuffer& key);
	};

	CtrlListItem(
			CTRL_ID type = CTRLID_UNKNOWN,
			const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr
		);
	virtual	~CtrlListItem() = 0;

	//	ctrltypeID ����R���g���[���N���X�𐶐�����t�@�N�g���[�֐�
	static CtrlListItem* createCtrl(int ctrltypeID,
									const StringBuffer& name,
									const StringBuffer& text);
	static CTRL_ID getCtrlTypeFromName(const StringBuffer&);

	//	���z�łȂ����\�b�h
	//	�}�W�b�N�i���o�[�̃`�F�b�N
	DWORD isValid() const { return m_dwMagic == CTRL_PTR_MAGIC; }
	//	�R���g���[���́i���[�U�[��`�́j���O��Ԃ�
	const StringBuffer&	getName() const { return m_name; }
	//	�R���g���[���̎�ނ�\���h�c
	DWORD getCtrlType() const { return m_type; }

	DlgPage& getParentPage() { return *m_pDlgPage; }
	void setParentPage(DlgPage* pdp) { m_pDlgPage = pdp; }

	POINT getRect() const;
	void setRect(WORD x, WORD y, WORD cx, WORD cy);

	int getItemNum() const { return m_item.ptr() != NULL ? m_item->itemNum() : 0; }

	BOOL isEnabled() const { return m_bEnable; }

	BOOL onInitCtrl(HWND hDlg);
	BOOL onUninitCtrl();

	WORD onWmCommand(WPARAM wParam, LPARAM lParam);
	WORD onWmNotify(WPARAM wParam, LPARAM lParam);
	WORD onWmImeNotify(WPARAM wParam, LPARAM lParam);
	HBRUSH onWmCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam);

	//	virtual functions, should/may be overrided.
	virtual	int getCtrlNum() const { return m_cnum; }
	virtual HWND getCtrlHWND(int i = 0) const { return NULL; }
	virtual HWND getFocusedCtrl() const { return NULL; } // �t�H�[�J�X�𓾂�R���g���[���� HWND
	virtual WORD getDefID() const; // �f�t�H���g�v�b�V���{�^���̂h�c��Ԃ�
	virtual	int getDataSize();	//	DLGITEMTEMPLATE �̑傫��
	virtual	WORD getWidth();
	virtual WORD getHeight();

	//	DLGITEMTEMPLATE �\���̂ւ̏�������
	virtual	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	//	�R���g���[���̗L��/�����̐ؑ�
	virtual	BOOL enableCtrl(BOOL bEnable, BOOL bChange = FALSE);
	virtual	BOOL showCtrl(BOOL);	//	�R���g���[���̉���

	//	return TRUE if the control whose id is ctrl_id
	//	is included in this obj.
	virtual	BOOL isCommand(WORD);

	//	command handlers
	virtual	BOOL onSetState(CmdLineParser&);	//	�I�����ڕύX
	virtual	BOOL onSetString(const StringBuffer&);	//	�e�L�X�g�ύX
	virtual	BOOL onSetCtrlFont(CmdLineParser&);	//	�t�H���g�̕ύX
	virtual	BOOL onSetHeight(WORD);		//	�����̕ύX
	virtual	BOOL onSetWidth(WORD);		//	���̕ύX
	virtual	BOOL onSetNotify(CmdLineParser&);	//	�ʒm�ԍ��̕ύX
	//	���ڂ̏�ԕύX
	virtual	BOOL onSetItem(CmdLineParser&,const StringBuffer&);
	//	���ڂ̒ǉ�
	virtual	BOOL onInsertItem(CmdLineParser&,const StringBuffer&);
	virtual	BOOL onDeleteItem(const StringBuffer&);	//	���ڂ̍폜
	virtual	BOOL onResetList();	//	�S���ڂ̍폜
	virtual	StringBuffer onGetState();	//	�I�����ڂ̎擾
	virtual	StringBuffer onGetString();	//	�e�L�X�g�̎擾
	virtual	StringBuffer onGetCtrlFont();	//	�t�H���g�̎擾
	virtual	StringBuffer onGetItem(const StringBuffer&);	//	���ڂ̎擾
	virtual BOOL onSetImeState(int);	//	IME �̏�����Ԃ�ύX
	virtual int onGetImeState();	//	IME �̏�����Ԃ�ݒ�
	virtual BOOL onSetSort(CmdLineParser&); // �\�[�g�̏�Ԃ�ύX
	virtual StringBuffer onGetSort(); // �\�[�g�̏�Ԃ��擾
	virtual int onSetFocusedItem(const StringBuffer& item);	//	�t�H�[�J�X�𓾂鍀�ڂ̎w��
	virtual StringBuffer onGetFocusedItem();	//	�t�H�[�J�X�𓾂Ă��鍀�ڂ̎擾

	//	datafile handlers
	virtual	BOOL dumpData(DlgDataFile&);	//	�f�[�^�̏�������
	virtual	BOOL loadData(DlgDataFile&);	//	�f�[�^�̓Ǎ���

	//	�R���g���[���̑�����\���N���X
	struct CtrlProperty {
		CtrlListItem* m_pCtrl;  // ������ CtrlListItem �ւ̃|�C���^
		HWND	m_hwndCtrl;		//	�R���g���[���̃E�B���h�E�n���h��
		LPSTR	m_classname;	//	�R���g���[���N���X��
		WNDPROC m_pfnDefCallback; // �f�t�H���g�R�[���o�b�N�֐�
		DWORD	m_style;		//	�E�B���h�E�X�^�C���t���O
		DWORD	m_exstyle;		//	extra �E�B���h�E�X�^�C���t���O
		WORD	m_id;			//	�R���g���[���h�c
		WORD	m_bufsize;		//	�N���X���̒���+1
		StringBuffer	m_text;	//	�R���g���[���e�L�X�g�o�b�t�@
		CtrlFontProperty	m_fontprop;		//	�t�H���g����
		HBRUSH  m_hbrBackground; // �w�i�F�̃n���h��
		HDC		m_hDC;			//	�X�^�e�B�b�N�e�L�X�g�̏ꍇ�̂ݗp���郁����DC

		CtrlProperty(CtrlListItem* pCtrl = NULL);
		~CtrlProperty();
		void setCtrlTemplate(CtrlTemplateArgs&);
		void changeFont();
		BOOL init(HWND);

		BOOL setText();
		BOOL getText();

		BOOL dumpData(DlgDataFile&, StringBuffer& key);
		BOOL loadData(DlgDataFile&, StringBuffer& key);
	};
	typedef CtrlProperty* LPCtrlProperty;

	virtual LRESULT dispatchRawMsg(CtrlProperty*, HWND, UINT, WPARAM, LPARAM);

protected:
	//	�O���[�o�����O��Ԃɒu���Ă��������Ȃ��N���X

	//	�����̑I�����ڂ����R���g���[���̂��߂̏�ԊǗ��N���X
	class States {
	public:
		void reset()
		{
			m_states.clear();
		}

		BOOL getState(int ind) const
		{
			return m_states.getBit(ind);
		}

		int getFirstIndex(int last) const
		{
			for (int i = 0; i < last; i++) {
				if (m_states.getBit(i)) return i;
			}
			return -1;
		}

		void setState(int ind, BOOL flag)
		{
			m_states.setBit(ind, flag);
		}

		BOOL dumpData(DlgDataFile &ddfile) const;
		BOOL loadData(DlgDataFile &ddfile);

	private:
		BitArray m_states;
	};

	//	�N���X�����o�E���\�b�h
	DWORD m_dwMagic;    //  �}�W�b�N�i���o�[
	DWORD m_type;		//	�R���g���[�����ʂh�c
	const StringBuffer m_name;	//	�R���g���[���̖��O
	const StringBuffer m_text;	//	newcontrol �̂R�Ԗڂ̈���
	class DlgPage* m_pDlgPage;	//	������ DlgPage �ւ̃|�C���^
	int m_cnum;		//	�R���g���[���E�B���h�E�̐�
	CtrlListItem::CtrlProperty* m_pcp;	//	�R���g���[�������\���̂ւ̃|�C���^
	Auto_Ptr< LinkList<ItemData> > m_item;	//	�A�C�e�����X�g
	BOOL m_bInitDone;	//	�������I���t���O
	BOOL m_bEnable;	//	�R���g���[���̗L��/�����t���O
	WORD m_notify[32];	//	�ʒm�R�[�h
	WORD m_x;	//	�R���g���[���ʒu�� x ���W
	WORD m_y;	//	�R���g���[���ʒu�� y ���W
	WORD m_cx;	//	�R���g���[���̕�
	WORD m_cy;	//	�R���g���[���̍���
	WORD m_width;	//	part �R�}���h�̎w��l
	WORD m_height;	//	height �R�}���h�̎w��l

	CtrlListItem(const CtrlListItem&);
	CtrlListItem& operator=(const CtrlListItem&);

	//	�������E�j������
	virtual	BOOL initCtrl(HWND);	//	�R���g���[���̏�����
	virtual	BOOL uninitCtrl();	//	�R���g���[���̔j��
	//	�I�u�W�F�N�g�ƃR���g���[���E�B���h�E�Ԃ̃f�[�^�̂��Ƃ�
	virtual	BOOL sendData();		//	�I�u�W�F�N�g���E�B���h�E
	virtual BOOL receiveData();	//	�E�B���h�E���I�u�W�F�N�g

	//	�E�B���h�E���b�Z�[�W�n���h��
	virtual	WORD onCommand(WPARAM,LPARAM);	//	WM_COMMAND
	virtual	WORD onNotify(WPARAM,LPARAM);	//	WM_NOTIFY
	virtual	HBRUSH onCtlColor(HWND hwndCtrl, UINT uMsg, HDC hDC);	//	WM_CTLCOLOR
	virtual WORD onImeNotify(WPARAM,LPARAM);	//	WM_IME_NOTIFY
};

//	�R���g���[���̎q(�܂��͌Z��)�R���g���[�����T�u�N���X�����鎞
//	GWL_USERDATA �œn���N���X
struct ChildCtrlSubClassInfo {
	WNDPROC m_pfnDefCallback;
	CtrlListItem* m_pCtrl;
};

//	newpage, newcolumn
class PageFormatCtrl : public CtrlListItem {
public:
	PageFormatCtrl(const StringBuffer& width, CTRL_ID type)
		:	CtrlListItem(type)
	{
		m_cnum = -1;
		m_width = (WORD)ival(width);
	}
};

class SimpleCtrl : public CtrlListItem {
public:
	SimpleCtrl(const StringBuffer& name = nullStr,
			   const StringBuffer& text = nullStr,
			   CTRL_ID type = CTRLID_UNKNOWN);
	~SimpleCtrl();

	HWND getCtrlHWND(int i = 0) const { return m_pcp->m_hwndCtrl; }
	int getDataSize();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	BOOL initCtrl(HWND);
	BOOL uninitCtrl();
	BOOL enableCtrl(BOOL bEnable, BOOL bChange = FALSE);
	BOOL showCtrl(BOOL);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetString(const StringBuffer&);
	BOOL onSetCtrlFont(CmdLineParser&);
	StringBuffer onGetString();
	StringBuffer onGetCtrlFont();

	HBRUSH onCtlColor(HWND hwndCtrl, UINT, HDC);

	BOOL isCommand(WORD);

protected:
	BOOL setFont(const StringBuffer& fface = nullStr,
				 const StringBuffer& color = nullStr,
				 const StringBuffer& fname = nullStr);	//	�t�H���g�̕ύX
};

//	button, defbutton
class BtnCtrl : public SimpleCtrl {
public:
	BtnCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr,
			CTRL_ID type = CTRLID_BUTTON);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }
	WORD getDefID() const; // �f�t�H���g�v�b�V���{�^���̂h�c��Ԃ�

	WORD getHeight();

	WORD onCommand(WPARAM, LPARAM);

protected:
	HBRUSH onCtlColor(HWND hwndCtrl, UINT, HDC); // WM_CTLCOLORBTN
};

//	check, rdbtn
class CheckCtrl : public BtnCtrl {
public:
	CheckCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr,
			CTRL_ID type = CTRLID_CHECK);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }

	BOOL sendData();
	BOOL receiveData();

	BOOL onSetState(CmdLineParser&);
	StringBuffer onGetState();

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	BOOL m_state;
};

//	text
class TextCtrl : public SimpleCtrl {
public:
	TextCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr);

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);

	BOOL onSetCtrlFont(CmdLineParser&);

protected:
	HBRUSH onCtlColor(HWND hwndCtrl, UINT, HDC); // WM_CTLCOLORSTATIC
};

//	edit, mledit
class EditCtrl : public SimpleCtrl {
public:
	EditCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr,
			CTRL_ID type = CTRLID_EDIT);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }

	WORD getHeight();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	BOOL initCtrl(HWND);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetState(CmdLineParser&);
	StringBuffer onGetState();
	BOOL onSetImeState(int);
	int onGetImeState();

	WORD onCommand(WPARAM, LPARAM);
	WORD onImeNotify(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	DWORD m_imestate;
	BOOL  m_bAlreadyFocused;
};

//	hline, vline
class LineCtrl : public SimpleCtrl {
public:
	LineCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& width = nullStr,
			CTRL_ID type = CTRLID_HLINE);

	WORD getHeight();
	WORD getWidth();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
};

//	track
class	TrackCtrl : public SimpleCtrl	{
public:
	TrackCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }

	WORD getHeight();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	BOOL initCtrl(HWND);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetState(CmdLineParser&);
	StringBuffer onGetState();

	WORD onCommand(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	WORD m_min;
	WORD m_max;
	WORD m_val;
	WORD m_tic;
	WORD m_wPrevNotify;

	HBRUSH onCtlColor(HWND hwndCtrl, UINT, HDC);
};

//	tree
class TreeCtrl : public SimpleCtrl {
public:
	TreeCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr);
	~TreeCtrl();

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }

	BOOL initCtrl(HWND);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetSort(CmdLineParser&);
	StringBuffer onGetSort();

	BOOL onSetState(CmdLineParser&);
	BOOL onSetItem(CmdLineParser&, const StringBuffer&);
	BOOL onInsertItem(CmdLineParser&, const StringBuffer&);
	BOOL onDeleteItem(const StringBuffer&);
	BOOL onResetList();
	BOOL onSetCtrlFont(CmdLineParser&);	//	�t�H���g�̕ύX
	StringBuffer onGetState();
	StringBuffer onGetItem(const StringBuffer&);
	int onSetFocusedItem(const StringBuffer& item);	//	�t�H�[�J�X�𓾂鍀�ڂ̎w��
	StringBuffer onGetFocusedItem();	//	�t�H�[�J�X�𓾂Ă��鍀�ڂ̎擾

	WORD onNotify(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	Auto_Ptr<TreeHashTable> m_pHashItem;
	StringBuffer m_state;

	BOOL m_bSorted;
};


class HasListCtrl : public SimpleCtrl {
public:
	HasListCtrl(const StringBuffer& name = nullStr,
				const StringBuffer& text = nullStr,
				CTRL_ID type = CTRLID_UNKNOWN);
	~HasListCtrl();

	WORD getHeight();

	BOOL onSetState(CmdLineParser&);
	BOOL onSetItem(CmdLineParser&, const StringBuffer&);
	BOOL onInsertItem(CmdLineParser&, const StringBuffer&);
	BOOL onDeleteItem(const StringBuffer&);
	BOOL onResetList();
	StringBuffer onGetState();
	StringBuffer onGetItem(const StringBuffer&);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	int	m_state;

	int	getItemData(const StringBuffer&);

	// �\�[�g�\�ȃR���g���[���ɂ��āA�����ڂ̏����� m_item ���̏�����
	// �Ή��t�����X�V���郁�\�b�h
	virtual void setViewIndex();

	virtual void getStateFromView();
};

//	reffilebutton, refdirbutton, refcolorbutton
class RefBtnCtrl : public HasListCtrl {
public:
	RefBtnCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr,
			CTRL_ID type = CTRLID_REFFILEBUTTON);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }
	WORD getDefID() const; // �f�t�H���g�v�b�V���{�^���̂h�c��Ԃ�

	WORD getHeight();

	BOOL onSetString(const StringBuffer&);
	StringBuffer onGetString();

	WORD onCommand(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	StringBuffer m_exbuffer;
};

//	radio
class RadioCtrl : public HasListCtrl {
public:
	RadioCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr);
	~RadioCtrl();

	WORD getHeight();
	int getCtrlNum() const;
	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }
	int getDataSize();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	BOOL initCtrl(HWND);
	BOOL enableCtrl(BOOL, BOOL);
	BOOL showCtrl(BOOL);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetString(const StringBuffer&);
	BOOL onSetItem(CmdLineParser&,const StringBuffer&);
	BOOL onInsertItem(CmdLineParser&,const StringBuffer&);
	BOOL onDeleteItem(const StringBuffer&);
	BOOL onResetList();
	StringBuffer onGetString();
	int onSetFocusedItem(const StringBuffer& item);	//	�t�H�[�J�X�𓾂鍀�ڂ̎w��
	StringBuffer onGetFocusedItem();	//	�t�H�[�J�X�𓾂Ă��鍀�ڂ̎擾

	BOOL isCommand(WORD);

	WORD onCommand(WPARAM, LPARAM);

protected:
	HBRUSH onCtlColor(HWND hwndCtrl, UINT, HDC); // WM_CTLCOLORSTATIC
};

//	list
class ListCtrl : public HasListCtrl {
public:
	ListCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr,
			CTRL_ID type = CTRLID_LIST);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	BOOL initCtrl(HWND);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetSort(CmdLineParser&);
	StringBuffer onGetSort();

	BOOL onSetItem(CmdLineParser&,const StringBuffer&);
	BOOL onInsertItem(CmdLineParser&,const StringBuffer&);
	BOOL onDeleteItem(const StringBuffer&);
	BOOL onResetList();
	int onSetFocusedItem(const StringBuffer& item);	//	�t�H�[�J�X�𓾂鍀�ڂ̎w��
	StringBuffer onGetFocusedItem();	//	�t�H�[�J�X�𓾂Ă��鍀�ڂ̎擾

	WORD onCommand(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	UINT m_style_sort;
	UINT m_msg_addstr;
	UINT m_msg_setstr;
	UINT m_msg_getstr;
	UINT m_msg_delstr;
	UINT m_msg_delall;
	UINT m_msg_setsel;
	UINT m_msg_getsel;
	UINT m_msg_setdata;
	UINT m_msg_getdata;
	UINT m_msg_err;

	BOOL m_bSorted;

	StringBuffer m_strFocusedItem;

	void setViewIndex();
	void getStateFromView();
};

//	combo
class ComboCtrl : public ListCtrl {
public:
	ComboCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr,
			CTRL_ID type = CTRLID_COMBO);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }

	WORD getHeight();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	BOOL initCtrl(HWND);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetString(const StringBuffer&);
	BOOL onSetImeState(int);
	int onGetImeState();

	WORD onCommand(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

	LRESULT dispatchRawMsg(CtrlProperty*, HWND, UINT, WPARAM, LPARAM);

protected:
	DWORD m_imestate;
	BOOL  m_bEditable;
	BOOL  m_bAlreadyFocused;

	void getStateFromView();
};

//	chklist
class ChkListCtrl : public HasListCtrl {
public:
	ChkListCtrl(const StringBuffer& name = nullStr,
				const StringBuffer& text = nullStr,
				CTRL_ID type = CTRLID_CHKLIST);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	BOOL initCtrl(HWND);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetSort(CmdLineParser&);
	StringBuffer onGetSort();

	BOOL onSetState(CmdLineParser&);
	BOOL onSetItem(CmdLineParser&,const StringBuffer&);
	BOOL onInsertItem(CmdLineParser&,const StringBuffer&);
	BOOL onDeleteItem(const StringBuffer&);
	BOOL onResetList();
	StringBuffer onGetState();
	BOOL onSetCtrlFont(CmdLineParser&);	//	�t�H���g�̕ύX
	int onSetFocusedItem(const StringBuffer& item);	//	�t�H�[�J�X�𓾂鍀�ڂ̎w��
	StringBuffer onGetFocusedItem();	//	�t�H�[�J�X�𓾂Ă��鍀�ڂ̎擾

	WORD onNotify(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	States m_states;
	StringBuffer m_strFocusedItem;

	BOOL m_bSorted;

	void setViewIndex();
	void getStateFromView();
};

//	lview
class LViewCtrl : public ChkListCtrl {
public:
	LViewCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr);
	~LViewCtrl();

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }

	WORD getHeight();

	BOOL initCtrl(HWND);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetSort(CmdLineParser&);
	StringBuffer onGetSort();

	BOOL onSetItem(CmdLineParser&,const StringBuffer&);
	BOOL onInsertItem(CmdLineParser&,const StringBuffer&);
	StringBuffer onGetItem(const StringBuffer&);

	WORD onNotify(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	LViewItemData* m_hdr;
	WORD m_colnum;

	int  m_lastSortKey;
	BOOL m_bAscending;

	void getStateFromView();

	BOOL setHeader(CmdLineParser&);
	void setHeaderView();
};


//	frame, group
class FrameCtrl : public HasListCtrl {
public:
	FrameCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr,
			CTRL_ID type = CTRLID_FRAME);
	~FrameCtrl();

	HWND getFocusedCtrl() const;
	WORD getDefID() const; // �f�t�H���g�v�b�V���{�^���̂h�c��Ԃ�

	WORD getHeight();

	BOOL initCtrl(HWND);
	BOOL uninitCtrl();
	BOOL enableCtrl(BOOL bEnable, BOOL bChange = FALSE);
	BOOL showCtrl(BOOL);
	BOOL sendData();

	BOOL onSetItem(CmdLineParser&,const StringBuffer&);
	BOOL onInsertItem(CmdLineParser&,const StringBuffer&);
	BOOL onDeleteItem(const StringBuffer&);
	BOOL onResetList();

	WORD onCommand(WPARAM,LPARAM);

protected:
	class DlgPage** m_page;
	POINTS m_poffset;
	POINTS m_pmargin;
	BOOL m_bVisible;

	HBRUSH onCtlColor(HWND hwndCtrl, UINT, HDC);

	virtual DlgPage* getPageByIndex(int index);
};

//	tab
class TabCtrl : public FrameCtrl {
public:
	TabCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr);

	HWND getFocusedCtrl() const;
	WORD getDefID() const; // �f�t�H���g�v�b�V���{�^���̂h�c��Ԃ�

	WORD getHeight();

	BOOL initCtrl(HWND);
	BOOL enableCtrl(BOOL bEnable, BOOL bChange = FALSE);
	BOOL showCtrl(BOOL);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetState(CmdLineParser&);
	BOOL onSetItem(CmdLineParser&,const StringBuffer&);
	BOOL onInsertItem(CmdLineParser&,const StringBuffer&);

	WORD onNotify(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	DlgPage* getPageByIndex(int index);
};


class MultipleCtrl : public CtrlListItem {
public:
	MultipleCtrl(const StringBuffer& name = nullStr,
				const StringBuffer& text = nullStr,
				CTRL_ID type = CTRLID_UNKNOWN,
				int cnum = 2);
	~MultipleCtrl();

	HWND getCtrlHWND(int i = 0) const { return m_pcp[i].m_hwndCtrl; }
	int getDataSize();

	BOOL initCtrl(HWND);
	BOOL uninitCtrl();
	BOOL enableCtrl(BOOL bEnable, BOOL bChange = FALSE);
	BOOL showCtrl(BOOL);
	BOOL sendData();
	BOOL receiveData();

	BOOL isCommand(WORD);
};

//	spin
class SpinCtrl : public MultipleCtrl {
public:
	SpinCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr);

	HWND getFocusedCtrl() const { return m_pcp[0].m_hwndCtrl; }
	WORD getHeight();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	BOOL initCtrl(HWND);
	BOOL sendData();
	BOOL receiveData();

	BOOL onSetState(CmdLineParser&);
	BOOL onSetString(const StringBuffer&);
	StringBuffer onGetState();
	StringBuffer onGetString();

	WORD onCommand(WPARAM, LPARAM);
	WORD onNotify(WPARAM, LPARAM);

	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

protected:
	int	m_min;
	int	m_max;
	int	m_val;
};

//	okcancel
class OkCancelCtrl : public MultipleCtrl {
public:
	OkCancelCtrl();

	HWND getFocusedCtrl() const { return m_pcp[0].m_hwndCtrl; }
	WORD getDefID() const; // �f�t�H���g�v�b�V���{�^���̂h�c��Ԃ�
	WORD getHeight();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);

	WORD onCommand(WPARAM, LPARAM);
};

#endif

