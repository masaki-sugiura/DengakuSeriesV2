//	$Id: menu.cpp,v 1.2 2003/10/18 13:42:34 sugiura Exp $
/*
 *	menu.cpp
 *	���j���[�\���Ɋւ���N���X
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

//	���j���[�N���X��ǉ�
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

//	�ċA�I�Ƀ��j���[�n���h��"����"��\�񂷂�
//	(���ۂ̃��j���[�̍\�z�� Callback() �֐��̒�����
//	createMenu() ���Ăяo�����Ƃōs��)
HMENU
MenuList::precreateMenuRecursive(const StringBuffer& name)
{
	//	���j���[������ Menu �N���X�I�u�W�F�N�g�ւ̃|�C���^���擾
	Menu* pmenu = this->getMenu(name);
	if (pmenu == NULL) return NULL;
	else if (pmenu->m_hMenu) return pmenu->m_hMenu;	//	���ɍ\�z�ς�

	//	���j���[�̍쐬(�\��)
	if ((pmenu->m_hMenu = ::CreatePopupMenu()) == NULL) return NULL;

	//	�T�u���j���[������΍ċA�I�ɃT�u���j���[���\�z
	pmenu->initSequentialGet();
	MenuItem* mi;
	while ((mi = pmenu->getNextItem()) != NULL) {
		if (mi->m_type == MENUTYPE_POPUP)
			mi->m_hmenu = this->precreateMenuRecursive(mi->getSubMenuName());
	}
	return pmenu->m_hMenu;
}

//	���ۂ̃��j���[�̍\�z
HMENU
MenuList::createMenu(HMENU hmenu)
{
	if (!::IsMenu(hmenu)) return NULL;

	//	���j���[�n���h������ Menu �N���X��T��
	int	index = 1;	//	���j���[�̃C���f�b�N�X
	Menu* pmenu;
	this->initSequentialGet();
	while ((pmenu = this->getNextItem()) != NULL) {
		if (pmenu->m_hMenu == hmenu) break;
		index++;
	}
	if (pmenu == NULL) return NULL;
	else if (pmenu->m_bInitDone) return hmenu;	//	���ɍ\�z�ς�

	//	���j���[���ڂ̒ǉ�
	UINT flag, id, iditem = 1 /* ���j���[���ڂ̃C���f�b�N�X */;
	LPCSTR item;
	MenuItem* mi;
	pmenu->initSequentialGet();
	while ((mi = pmenu->getNextItem()) != NULL) {
		switch (mi->m_type) {
		case MENUTYPE_ITEM:
			flag = MFT_STRING;
			if (iditem == 1) flag |= MFS_HILITE;
			item = mi->getText();
			//	HIBYTE(LOWORD(id))	:	���j���[�C���f�b�N�X
			//	LOBYTE(LOWORD(id))	:	���ڃC���f�b�N�X
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
			//	�g�����j���[�̏ꍇ�Z�p���[�^���C���f�b�N�X���J�E���g����
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

//	���j���[�̃��\�[�X���J��
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

