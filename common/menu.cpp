//	$Id: menu.cpp,v 1.2 2003/10/18 13:42:34 sugiura Exp $
/*
 *	menu.cpp
 *	メニュー表示に関するクラス
 */

#include "session.h"
#include "menu.h"

WORD
MenuItem::readOpt(const StringBuffer& opt)
{
	LPCSTR popt = opt;
	WORD ret = 0;
	while (*popt) {
		switch (*popt++) {
		case 'c':
			ret |= 0x01; break;
		case 'd':
			ret |= 0x02; break;
		}
	}
	return ret;
}

//	メニュークラスを追加
int
MenuList::addMenu(Menu* menu)
{
	if (menu == NULL || menu->getName().length() <= 0) {
//		::MessageBox(NULL, "addMenu() failed.", NULL, MB_OK);
		return -1;
	}
	int	ret = this->addItem(menu);
	if (ret >= 0) m_pLastMenu = menu;
	return ret;
}

//	再帰的にメニューハンドル"だけ"を予約する
//	(実際のメニューの構築は Callback() 関数の中から
//	createMenu() を呼び出すことで行う)
HMENU
MenuList::precreateMenuRecursive(const StringBuffer& name)
{
	//	メニュー名から Menu クラスオブジェクトへのポインタを取得
	Menu* pmenu = this->getMenu(name);
	if (pmenu == NULL) return NULL;
	else if (pmenu->m_hMenu) return pmenu->m_hMenu;	//	既に構築済み

	//	メニューの作成(予約)
	if ((pmenu->m_hMenu = ::CreatePopupMenu()) == NULL) return NULL;

	//	サブメニューがあれば再帰的にサブメニューも構築
	pmenu->initSequentialGet();
	MenuItem* mi;
	while ((mi = pmenu->getNextItem()) != NULL) {
		if (mi->m_type == MENUTYPE_POPUP)
			mi->m_hmenu = this->precreateMenuRecursive(mi->getSubMenuName());
	}
	return pmenu->m_hMenu;
}

//	実際のメニューの構築
HMENU
MenuList::createMenu(HMENU hmenu)
{
	if (!::IsMenu(hmenu)) return NULL;

	//	メニューハンドルから Menu クラスを探す
	int	index = 1;	//	メニューのインデックス
	Menu* pmenu;
	this->initSequentialGet();
	while ((pmenu = this->getNextItem()) != NULL) {
		if (pmenu->m_hMenu == hmenu) break;
		index++;
	}
	if (pmenu == NULL) return NULL;
	else if (pmenu->m_bInitDone) return hmenu;	//	既に構築済み

	//	メニュー項目の追加
	UINT flag, id, iditem = 1 /* メニュー項目のインデックス */;
	LPCSTR item;
	MenuItem* mi;
	pmenu->initSequentialGet();
	while ((mi = pmenu->getNextItem()) != NULL) {
		switch (mi->m_type) {
		case MENUTYPE_ITEM:
			flag = MFT_STRING;
			if (iditem == 1) flag |= MFS_HILITE;
			item = mi->getText();
			//	HIBYTE(LOWORD(id))	:	メニューインデックス
			//	LOBYTE(LOWORD(id))	:	項目インデックス
			id = iditem++ + (0xFFFFFF00&(index << 8));
			break;
		case MENUTYPE_POPUP:
			flag = MF_POPUP;
			if (iditem == 1) flag |= MFS_HILITE;
			item = mi->getText();
			id = (UINT)mi->m_hmenu;
			iditem++;
			break;
		case MENUTYPE_SEPARATOR:
			flag = MFT_SEPARATOR;
			item = NULL;
			id = 0;
			//	拡張メニューの場合セパレータもインデックスをカウントする
			if (pmenu->isExMenu()) iditem++;
			break;
		default:
			continue;
		}
		WORD state = mi->getState();
		if ((state & 0x01) != 0) flag |= MF_CHECKED;
		if ((state & 0x02) != 0) flag |= MF_GRAYED;
		::AppendMenu(pmenu->m_hMenu,flag,id,item);
	}
	pmenu->m_bInitDone = TRUE;
	return hmenu;
}

//	メニューのリソースを開放
void
MenuList::deleteMenu()
{
	this->initSequentialGet();
	Menu* pmenu;
	while ((pmenu = this->getNextItem()) != NULL) {
		if (pmenu->m_hMenu != NULL) {
			if (::IsMenu(pmenu->m_hMenu)) ::DestroyMenu(pmenu->m_hMenu);
			pmenu->m_hMenu = NULL;
			pmenu->m_bInitDone = FALSE;
		}
	}
}

