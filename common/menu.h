//	$Id: menu.h,v 1.2 2001/11/22 13:37:08 sugiura Exp $
/*
 *	menu.h
 *	メニュー表示に関するクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_MENU
#define	DENGAKUSERIES_CLASSES_MENU

#include "linklist.h"

class SessionInstance;

#define	MENUTYPE_SEPARATOR	0
#define	MENUTYPE_POPUP		1
#define	MENUTYPE_ITEM		2

#define MENURESULT_ERR_ID_MASK 0x8000

typedef struct {
	SessionInstance* m_pSessionInstance; // セッションインスタンス
	HMENU m_hMenu; // 最初に表示するメニューのハンドル
	POINT m_mnPosInfo; // メニュー表示位置を表す構造体
	BOOL m_bShowAtCursor; // マウスカーソルの位置にメニューを表示するか
	WORD m_wSelectedID;	//	選択されたメニュー項目
	WORD m_wFirstShow; // 初めて WM_INITMENUPOPUP が呼ばれたときにフラグを下ろす
} SHOWMENUPROCARGS, *LPSHOWMENUPROCARGS;

class MenuItem {
public:
	MenuItem(WORD type,
			 const StringBuffer& text = nullStr,
			 const StringBuffer& subname = nullStr,
			 LPCSTR opt = NULL)
		:	m_text(text),
			m_subname(subname),
			m_type(type),
			m_state(readOpt(opt)),
			m_hmenu(NULL)
	{}

	static WORD readOpt(const StringBuffer&);

	const StringBuffer&	getText() const
	{
		return m_text;
	}

	const StringBuffer&	setText(const StringBuffer& str)
	{
		return m_text = str;
	}

	WORD getState() const { return m_state; }
	WORD setState(const StringBuffer& opt)
	{
		return m_state = readOpt(opt);
	}

	const StringBuffer&	getSubMenuName() const
	{
		return m_subname;
	}

	friend class Menu;
	friend class MenuList;

private:
	StringBuffer m_text; //	メニューのテキスト
	const StringBuffer m_subname;	//	サブメニューの名前(もしあれば)
	WORD m_type;	//	項目タイプ
	WORD m_state;	//	チェック・有効/無効の状態
	HMENU m_hmenu;	//	サブメニューのメニューハンドル(もしあれば)

	MenuItem(const MenuItem&);
	MenuItem& operator=(const MenuItem&);
};

class Menu : public LinkList<MenuItem> {
public:
	Menu(const StringBuffer& name, BOOL bExMenu = FALSE)
		:	m_name(name),
			m_hMenu(NULL),
			m_bInitDone(FALSE),
			m_bExMenu(bExMenu)
	{}	//	nothing more to do.

	~Menu()
	{
		if (m_hMenu != NULL) ::DestroyMenu(m_hMenu);
	}

	BOOL isExMenu() const { return m_bExMenu; }
	const StringBuffer&	getName() const { return m_name; }

	friend class MenuList;

private:
	const StringBuffer m_name;	//	メニューの名前
	HMENU m_hMenu;		//	メニューハンドル
	BOOL m_bInitDone;	//	メニューを構築済みかどうか
	BOOL m_bExMenu;		//	拡張メニューかどうか
};

class MenuList : public NamedLinkList<Menu> {
public:
	MenuList()
		:	m_pLastMenu(NULL)
	{
		// nothing to do.
	}
	~MenuList()
	{
		this->deleteMenu();
	}

	int	addMenu(Menu*);

	Menu* getMenu(const StringBuffer& name = nullStr) const
	{
		if (name.length() <= 0) return m_pLastMenu;
		return m_HashName.getValue(name);
	}

	//	name をルートとするメニューのハンドルだけを再帰的に予約する
	HMENU precreateMenuRecursive(const StringBuffer& name);

	//	実際のメニューの構築(指定メニューのみ)
	HMENU createMenu(HMENU);

	//	メニューハンドルの削除
	void deleteMenu();

private:
	Menu* m_pLastMenu;	//	Poke の対象の Menu クラスへのポインタ
};

#endif

