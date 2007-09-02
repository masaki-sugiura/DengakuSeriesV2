//	$Id: ctrldata.h,v 1.25 2007-09-02 15:50:03 sugiura Exp $
/*
 *	ctrldata.h
 *	コントロールを扱うクラス
 */

/*
	コントロールクラスの実装についてのコメント：

	はっきり言って美しくない。首尾一貫性がない。
	動作させることが第一なので今はほったらかしにしているが、
	いずれはクラス設計から根本的にやり直す予定。

	クラス階層マップ(継承は全て public)：

	CtrlListItem				全てのコントロールクラスの(純粋)基底クラス
								全てのインターフェイスを定義する。
	PageFormatCtrl : CtrlListItem	newcolumn, newpage
	SimpleCtrl : CtrlListItem	内部にコントロールウィンドウを１つ持つ(純粋)
								基底クラス(…の予定だったが、RadioCtrl がその
								contract に違反している…美しくない(ToT))
		BtnCtrl : SimpleCtrl		button, defbutton
			CheckCtrl : BtnCtrl		check, rdbtn
		TextCtrl : SimpleCtrl		text
		EditCtrl : SimpleCtrl		edit, mledit, pwdedit
		LineCtrl : SimpleCtrl		hline, vline
		TrackCtrl : SimpleCtrl		track
		TreeCtrl : SimpleCtrl		tree
		HasListCtrl : SimpleCtrl	item コマンドを受け付ける(純粋)基底クラス。
									(これまた TreeCtrl が contract 違反(ToT))
			RefBtnCtrl : HasListCtrl	reffilebutton, refdirbutton, refcolorbutton
			RadioCtrl : HasListCtrl		radio
			ListCtrl : HasListCtrl		list
				ComboCtrl : ListCtrl	combo, cmblist
			ChkListCtrl : HasListCtrl	chklist
				LViewCtrl : ChkListCtrl	lview
			FrameCtrl : HasListCtrl		frame, group
				TabCtrl : FrameCtrl		tab
	MultipleCtrl : CtrlListItem	複数のコントロールウィンドウを持つ(純粋)基底
								クラス。
		SpinCtrl : MultipleCtrl		spin
		OkCancelCtrl : MultipleCtrl	okcancel

	これらから、次世代のクラス階層はこんな感じになるかな？？
	CtrlContainer	:	全ての（ユーザーから見た）コントロールを表すクラス
	以下は Windows 定義のコントロールに対応した(汎用)クラス(というかラッパー)
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

//	ダイアログ単位で計った行・列幅
#define	UWIDTH	4		//	横の長さの１単位(文字幅)
#define	UHEIGHT	4		//	縦の長さの１単位(文字高の半分)
#define	OWIDTH	2		//	横の余白の長さ
//	#define	OHEIGHT	4		//	縦の余白の長さ
#define	STRICTHEIGHT	2	//	厳密な１文字の高さ
#define	NARROWHEIGHT	3	//	縦幅が少ないコントロールの高さ
#define	NORMALHEIGHT	4	//	普通のコントロールの高さ

#define WM_GET_CTRL_PTR  (WM_USER + 2000)
#define CTRL_PTR_MAGIC   0x5A5AA5A5

//	項目を表すクラス
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

//	名前で識別できる項目を表すクラス
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

//	lview コントロールの項目を表すクラス
class LViewItemData : public ItemData, public LinkList<ItemData> {
//	nothing more.
};

typedef	HashTable<class TreeItemData*,11> TreeHashTable;

//	tree コントロールの項目を表すクラス
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

//	最も基本的なコントロールを表す基底クラス
class CtrlListItem {
public:
	struct CtrlTemplateArgs {
		WORD*	m_hdr;	//	DLGITEMTEMPLATE 構造体のヘッダアドレス
		WORD	m_id;	//	コントロールＩＤ
		WORD	m_x;	//	コントロールの左上の x 座標
		WORD	m_y;	//	コントロールの左上の y 座標
		WORD	m_cx;	//	コントロールの幅
		WORD	m_cy;	//	コントロールの高さ
	};

	//	フォント属性を表す構造体
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

	//	ctrltypeID からコントロールクラスを生成するファクトリー関数
	static CtrlListItem* createCtrl(int ctrltypeID,
									const StringBuffer& name,
									const StringBuffer& text);
	static CTRL_ID getCtrlTypeFromName(const StringBuffer&);

	//	仮想でないメソッド
	//	マジックナンバーのチェック
	DWORD isValid() const { return m_dwMagic == CTRL_PTR_MAGIC; }
	//	コントロールの（ユーザー定義の）名前を返す
	const StringBuffer&	getName() const { return m_name; }
	//	コントロールの種類を表すＩＤ
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
	virtual HWND getFocusedCtrl() const { return NULL; } // フォーカスを得るコントロールの HWND
	virtual WORD getDefID() const; // デフォルトプッシュボタンのＩＤを返す
	virtual	int getDataSize();	//	DLGITEMTEMPLATE の大きさ
	virtual	WORD getWidth();
	virtual WORD getHeight();

	//	DLGITEMTEMPLATE 構造体への書き込み
	virtual	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);
	//	コントロールの有効/無効の切替
	virtual	BOOL enableCtrl(BOOL bEnable, BOOL bChange = FALSE);
	virtual	BOOL showCtrl(BOOL);	//	コントロールの可視化

	//	return TRUE if the control whose id is ctrl_id
	//	is included in this obj.
	virtual	BOOL isCommand(WORD);

	//	command handlers
	virtual	BOOL onSetState(CmdLineParser&);	//	選択項目変更
	virtual	BOOL onSetString(const StringBuffer&);	//	テキスト変更
	virtual	BOOL onSetCtrlFont(CmdLineParser&);	//	フォントの変更
	virtual	BOOL onSetHeight(WORD);		//	高さの変更
	virtual	BOOL onSetWidth(WORD);		//	幅の変更
	virtual	BOOL onSetNotify(CmdLineParser&);	//	通知番号の変更
	//	項目の状態変更
	virtual	BOOL onSetItem(CmdLineParser&,const StringBuffer&);
	//	項目の追加
	virtual	BOOL onInsertItem(CmdLineParser&,const StringBuffer&);
	virtual	BOOL onDeleteItem(const StringBuffer&);	//	項目の削除
	virtual	BOOL onResetList();	//	全項目の削除
	virtual	StringBuffer onGetState();	//	選択項目の取得
	virtual	StringBuffer onGetString();	//	テキストの取得
	virtual	StringBuffer onGetCtrlFont();	//	フォントの取得
	virtual	StringBuffer onGetItem(const StringBuffer&);	//	項目の取得
	virtual BOOL onSetImeState(int);	//	IME の初期状態を変更
	virtual int onGetImeState();	//	IME の初期状態を設定
	virtual BOOL onSetSort(CmdLineParser&); // ソートの状態を変更
	virtual StringBuffer onGetSort(); // ソートの状態を取得
	virtual int onSetFocusedItem(const StringBuffer& item);	//	フォーカスを得る項目の指定
	virtual StringBuffer onGetFocusedItem();	//	フォーカスを得ている項目の取得

	//	datafile handlers
	virtual	BOOL dumpData(DlgDataFile&);	//	データの書き込み
	virtual	BOOL loadData(DlgDataFile&);	//	データの読込み

	//	コントロールの属性を表すクラス
	struct CtrlProperty {
		CtrlListItem* m_pCtrl;  // 所属先 CtrlListItem へのポインタ
		HWND	m_hwndCtrl;		//	コントロールのウィンドウハンドル
		LPSTR	m_classname;	//	コントロールクラス名
		WNDPROC m_pfnDefCallback; // デフォルトコールバック関数
		DWORD	m_style;		//	ウィンドウスタイルフラグ
		DWORD	m_exstyle;		//	extra ウィンドウスタイルフラグ
		WORD	m_id;			//	コントロールＩＤ
		WORD	m_bufsize;		//	クラス名の長さ+1
		StringBuffer	m_text;	//	コントロールテキストバッファ
		CtrlFontProperty	m_fontprop;		//	フォント属性
		HBRUSH  m_hbrBackground; // 背景色のハンドル
		HDC		m_hDC;			//	スタティックテキストの場合のみ用いるメモリDC

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
	//	グローバル名前空間に置いておきたくないクラス

	//	複数の選択項目を持つコントロールのための状態管理クラス
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

	//	クラスメンバ・メソッド
	DWORD m_dwMagic;    //  マジックナンバー
	DWORD m_type;		//	コントロール識別ＩＤ
	const StringBuffer m_name;	//	コントロールの名前
	const StringBuffer m_text;	//	newcontrol の３番目の引数
	class DlgPage* m_pDlgPage;	//	所属先 DlgPage へのポインタ
	int m_cnum;		//	コントロールウィンドウの数
	CtrlListItem::CtrlProperty* m_pcp;	//	コントロール属性構造体へのポインタ
	Auto_Ptr< LinkList<ItemData> > m_item;	//	アイテムリスト
	BOOL m_bInitDone;	//	初期化終了フラグ
	BOOL m_bEnable;	//	コントロールの有効/無効フラグ
	WORD m_notify[32];	//	通知コード
	WORD m_x;	//	コントロール位置の x 座標
	WORD m_y;	//	コントロール位置の y 座標
	WORD m_cx;	//	コントロールの幅
	WORD m_cy;	//	コントロールの高さ
	WORD m_width;	//	part コマンドの指定値
	WORD m_height;	//	height コマンドの指定値

	CtrlListItem(const CtrlListItem&);
	CtrlListItem& operator=(const CtrlListItem&);

	//	初期化・破棄処理
	virtual	BOOL initCtrl(HWND);	//	コントロールの初期化
	virtual	BOOL uninitCtrl();	//	コントロールの破棄
	//	オブジェクトとコントロールウィンドウ間のデータのやりとり
	virtual	BOOL sendData();		//	オブジェクト→ウィンドウ
	virtual BOOL receiveData();	//	ウィンドウ→オブジェクト

	//	ウィンドウメッセージハンドラ
	virtual	WORD onCommand(WPARAM,LPARAM);	//	WM_COMMAND
	virtual	WORD onNotify(WPARAM,LPARAM);	//	WM_NOTIFY
	virtual	HBRUSH onCtlColor(HWND hwndCtrl, UINT uMsg, HDC hDC);	//	WM_CTLCOLOR
	virtual WORD onImeNotify(WPARAM,LPARAM);	//	WM_IME_NOTIFY
};

//	コントロールの子(または兄弟)コントロールをサブクラス化する時
//	GWL_USERDATA で渡すクラス
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
				 const StringBuffer& fname = nullStr);	//	フォントの変更
};

//	button, defbutton
class BtnCtrl : public SimpleCtrl {
public:
	BtnCtrl(const StringBuffer& name = nullStr,
			const StringBuffer& text = nullStr,
			CTRL_ID type = CTRLID_BUTTON);

	HWND getFocusedCtrl() const { return m_pcp->m_hwndCtrl; }
	WORD getDefID() const; // デフォルトプッシュボタンのＩＤを返す

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
	BOOL onSetCtrlFont(CmdLineParser&);	//	フォントの変更
	StringBuffer onGetState();
	StringBuffer onGetItem(const StringBuffer&);
	int onSetFocusedItem(const StringBuffer& item);	//	フォーカスを得る項目の指定
	StringBuffer onGetFocusedItem();	//	フォーカスを得ている項目の取得

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

	// ソート可能なコントロールについて、見た目の順序と m_item 内の順序の
	// 対応付けを更新するメソッド
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
	WORD getDefID() const; // デフォルトプッシュボタンのＩＤを返す

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
	int onSetFocusedItem(const StringBuffer& item);	//	フォーカスを得る項目の指定
	StringBuffer onGetFocusedItem();	//	フォーカスを得ている項目の取得

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
	int onSetFocusedItem(const StringBuffer& item);	//	フォーカスを得る項目の指定
	StringBuffer onGetFocusedItem();	//	フォーカスを得ている項目の取得

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
	BOOL onSetCtrlFont(CmdLineParser&);	//	フォントの変更
	int onSetFocusedItem(const StringBuffer& item);	//	フォーカスを得る項目の指定
	StringBuffer onGetFocusedItem();	//	フォーカスを得ている項目の取得

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
	WORD getDefID() const; // デフォルトプッシュボタンのＩＤを返す

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
	WORD getDefID() const; // デフォルトプッシュボタンのＩＤを返す

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
	WORD getDefID() const; // デフォルトプッシュボタンのＩＤを返す
	WORD getHeight();

	BOOL createCtrlTemplate(CtrlListItem::CtrlTemplateArgs&);

	WORD onCommand(WPARAM, LPARAM);
};

#endif

