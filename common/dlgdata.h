//	$Id: dlgdata.h,v 1.5 2003-07-06 16:27:46 sugiura Exp $
/*
 *	dlgdata.h
 *	ダイアログを扱うクラス
 */

/*
	ダイアログの実装についてのコメント：

	全てのコントロールウィンドウは DlgPage クラスに属するウィンドウ
	(子ダイアログ)の子ウィンドウである。そして、全ての子ダイアログは
	DlgFrame クラスに属するウィンドウ(親ダイアログ)の子ウィンドウである。
	なお、newdialog(ex) で作成されるダイアログは "root" という名前の
	子ダイアログである。
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

//	ダイアログのフォント情報を格納する構造体
typedef struct FontProperty_tag	{
	int				m_fontsize;		//	ポイント数
	StringBuffer	m_fontname;	//	フォント名のバッファ
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

//	コントロール名のリスト
typedef	HashTable<CtrlListItem*,31>	CtrlHashTable;

//	ダイアログを初期化する際に表示すべき子ダイアログを登録するキューのアイテム
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

//	子ダイアログを表すクラス
class DlgPage {
public:
	DlgPage(DlgFrame *pdf, const StringBuffer &name, WORD width);
	~DlgPage();

//	メンバ値の取得
	const StringBuffer&	getName() const { return m_strName; }
	DlgFrame& getDlgFrame(){ return *m_pDlgFrame; }
	HWND gethwndPage() const { return m_hwndPage; }
	WORD getWidth() const { return m_width; }
	WORD getHeight() const { return m_height; }
	WORD getDefID() const;

//	ダイアログの構築/初期化
	DWORD evalPageSize();	//	m_width, m_height を計算
	//	子ダイアログの構築
	HWND createPage(HWND, BYTE, WORD, WORD, WORD, WORD, FontProperty&);
	BOOL initPage(HWND);	//	子ダイアログの初期化処理
	BOOL uninitPage();	//	子ダイアログの破棄時の処理
	BOOL showPage(BOOL);	//	子ダイアログの可視化
	BOOL enablePage(BOOL);	//	子ダイアログの有効化
	BOOL addShowPageQueue(LPCSTR pname, HWND hwndCtrl,
							WORD x, WORD y, WORD cx, WORD cy);

//	コントロールの追加・取得
	int addCtrl(CtrlListItem*);	//	コントロールの追加
	int addCtrl(int type, const StringBuffer& name, const StringBuffer& text);
	//	コントロールの取得
	CtrlListItem* getCtrl(const StringBuffer& name = nullStr);
	HWND getFocusedCtrl();

//	メッセージハンドラ
	WORD onCommand(WPARAM, LPARAM);
	WORD onNotify(WPARAM, LPARAM);
	WORD onHScroll(WPARAM, LPARAM);
	HBRUSH onCtlColor(WPARAM, LPARAM);
	WORD onImeNotify(WPARAM, LPARAM);

//	データの書き込み・読み出し
	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

	friend class DlgFrame;

private:
	DlgFrame* m_pDlgFrame;	//	親ダイアログへのポインタ
	const StringBuffer m_strName;	//	ダイアログページ名
	HWND m_hwndPage;		//	ウィンドウハンドル
	DWORD m_dwPageSize;	//	DLGTEMPLATE の大きさ
	LinkList<CtrlListItem>* m_pCtrlList;	//	コントロールのリスト
	CtrlHashTable* m_pHashCtrl;	//	コントロール名のリスト
	CtrlListItem* m_pCurCtrlData;	//	操作対象となるコントロールへのポインタ
	LinkList<ShowPageQueueItem>* m_pShowPageQueue;
	BOOL m_bVisible;	//	ウィンドウが可視かどうか
	BOOL m_bEnable;		//	ウィンドウが有効かどうか
	WORD m_cdit;		//	実際のコントロール数
						//	(!=m_pCtrlList->itemNum() に注意!!)
	WORD m_iniwidth;	//	ダイアログの初期幅
	WORD m_width;		//	ダイアログの幅
	WORD m_height;		//	ダイアログの高さ
};

//	コントロール名からコントロール識別ＩＤを求めるハッシュテーブル
typedef HashTable<int,61> CtrlIdHashTable;

//	親ダイアログを表すクラス
class DlgFrame {
public:
	DlgFrame();
	~DlgFrame();

	void setSessionInstance(SessionInstance* psi)
	{
		m_pSessionInstance = psi;
	}
	SessionInstance* getSessionInstance() const { return m_pSessionInstance; }

	void setNotify(const StringBuffer& newvalue);	//	notify code の通知
	BOOL getNotify(StringBuffer& buf, DWORD wait) const;
	void resetNotify();

//	メンバ値の取得・変更
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
	const StringBuffer& getFontName() const
	{
		return m_pFontProp->m_fontname;
	}
	int getFontSize() const
	{
		return m_pFontProp->m_fontsize;
	}
	WORD getDefID() const;

//	ダイアログの構築・初期化・破棄
	HWND createFrame(HWND hwndOwner, BOOL bOnTop = FALSE); // ダイアログの構築
	BOOL showFrame();	//	ダイアログの表示
	BOOL closeFrame();	//	ダイアログの破棄
	void resetFrame();	//	ダイアログデータの再初期化
	BOOL initFrame(HWND);	//	ダイアログの初期化(WM_INITDIALOG ハンドラ)
	BOOL uninitFrame();	//	ダイアログの破棄時処理

//	子ダイアログの追加・取得・作成
	int addPage(const StringBuffer&, WORD);	//	子ダイアログの追加
	BOOL setCurrentPage(const StringBuffer&); // 追加対象の子ダイアログの変更
	DlgPage* getPage(const StringBuffer& name = nullStr) // 子ダイアログの取得
	{
		return (name.length() > 0) ?
			m_pPageList->getItemByName(name) : m_pCurDlgPage;
	}
	DlgPage* getPage(int index)	//	子ダイアログの取得
	{
		return m_pPageList->getItemByIndex(index);
	}
	HWND createPage(
					const StringBuffer& name,
					HWND hWnd,
					WORD ofx, WORD ofy, WORD cx, WORD cy
				);	//	子ダイアログのウィンドウ作成

//	コントロールの取得
	CtrlListItem* getCtrl(const StringBuffer& ctrlname = nullStr,
						const StringBuffer& pagename = nullStr);

//	フォーカスを得(てい)るコントロール
	int setFocusedCtrl(const StringBuffer& name);
	const StringBuffer& getFocusedCtrl() const;

//	データの書き込み・読込み
	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

private:
	SessionInstance* m_pSessionInstance; // セッションインスタンスへのポインタ
	CtrlIdHashTable	m_HashCtrl; // コントロール名→ＩＤ
	HWND m_hwndFrame;	//	ウィンドウハンドル
	NamedLinkList<DlgPage>* m_pPageList;	//	子ダイアログのリスト
	DlgPage* m_pCurDlgPage;	//	現在処理対象の子ダイアログ
	StringBuffer m_DlgTitle;	//	ダイアログのタイトル
	StringBuffer m_sbFocusedCtrl; // フォーカスを得るコントロール名
	LPFontProperty m_pFontProp;	//	ダイアログのフォント属性
	POINTS m_pos;	//	ダイアログの表示位置
	WORD m_width;	//	ダイアログの幅
	WORD m_height;	//	ダイアログの高さ
	WORD m_flags;	//	表示位置に関するフラグ
};

extern const StringBuffer strRootPageName;

#endif
