//	$Id: dlgdata.h,v 1.16 2006-05-20 17:02:50 sugiura Exp $
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

// ダイアログ表示位置指定の単位
enum {
	DLGPOS_UNIT_FONTSIZE = 0,
	DLGPOS_UNIT_PIXEL    = 1
};

//	コントロール名のリスト
typedef	HashTable<CtrlListItem*,31>	CtrlHashTable;

//	ダイアログを初期化する際に表示すべき子ダイアログを登録するキューのアイテム
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
	bool isInTabCtrl() const { return m_bInTabCtrl; }

//	ダイアログの構築/初期化
	DWORD evalPageSize();	//	m_width, m_height を計算
	//	子ダイアログの構築
	HWND createPage(HWND, BYTE, WORD, WORD, WORD, WORD, bool, FontProperty&);
	BOOL initPage(HWND);	//	子ダイアログの初期化処理
	BOOL uninitPage();	//	子ダイアログの破棄時の処理
	BOOL showPage(BOOL);	//	子ダイアログの可視化
	BOOL enablePage(BOOL);	//	子ダイアログの有効化
	BOOL addShowPageQueue(LPCSTR pname, HWND hwndCtrl,
						  WORD x, WORD y, WORD cx, WORD cy,
						  bool bInTabCtrl);

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
	WORD onImeNotify(WPARAM, LPARAM);
	HBRUSH onCtlColor(UINT, WPARAM, LPARAM);

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
	bool m_bInTabCtrl;  //  Tab コントロールの内部に表示されるかどうか
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
					WORD ofx, WORD ofy, WORD cx, WORD cy,
					bool bInTabCtrl
				);	//	子ダイアログのウィンドウ作成

//	コントロールの取得
	CtrlListItem* getCtrl(const StringBuffer& ctrlname = nullStr,
						const StringBuffer& pagename = nullStr) const;

//	フォーカスを得(てい)るコントロール
	int setFocusedCtrl(const StringBuffer& name);
	const StringBuffer& getFocusedCtrl() const;

	// 純粋にフォーカスを得るコントロールの名前を返すメソッド
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

	//	コントロール内のフォーカスを得る項目を指定＆取得
	int setCtrlFocusedItem(const StringBuffer& ctrl, const StringBuffer& item);
	StringBuffer getCtrlFocusedItem(const StringBuffer& ctrl) const;

//	ダイアログの背景をTABコントロールと同色にする
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

//	データの書き込み・読込み
	BOOL dumpData(DlgDataFile&);
	BOOL loadData(DlgDataFile&);

private:
	SessionInstance* m_pSessionInstance; // セッションインスタンスへのポインタ
	CtrlIdHashTable	m_HashCtrl; // コントロール名→ＩＤ
	HWND m_hwndFrame;	//	ウィンドウハンドル
	ThemeWrapper* m_pThemeWrapper;
	NamedLinkList<DlgPage>* m_pPageList;	//	子ダイアログのリスト
	DlgPage* m_pCurDlgPage;	//	現在処理対象の子ダイアログ
	StringBuffer m_DlgTitle;	//	ダイアログのタイトル
	StringBuffer m_sbFocusedCtrl; // フォーカスを得るコントロール名
	LPFontProperty m_pFontProp;	//	ダイアログのフォント属性
	POINTS m_pos;	//	ダイアログの表示位置
	WORD m_wPosUnit;	// m_pos メンバの単位
	WORD m_width;	//	ダイアログの幅
	WORD m_height;	//	ダイアログの高さ
	WORD m_flags;	//	表示位置に関するフラグ
	int m_imestate;	//	IMEの状態を表す
	BOOL m_bAlreadyFocused; // 入力フォーカスを持ついずれかのコントロールにフォーカスが移ったかどうか
};

extern const StringBuffer strRootPageName;
extern const StringBuffer okStr;
extern const StringBuffer ngStr;

#endif
