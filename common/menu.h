//	$Id: menu.h,v 1.2 2001/11/22 13:37:08 sugiura Exp $
/*
 *	menu.h
 *	���j���[�\���Ɋւ���N���X
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
	SessionInstance* m_pSessionInstance; // �Z�b�V�����C���X�^���X
	HMENU m_hMenu; // �ŏ��ɕ\�����郁�j���[�̃n���h��
	POINT m_mnPosInfo; // ���j���[�\���ʒu��\���\����
	BOOL m_bShowAtCursor; // �}�E�X�J�[�\���̈ʒu�Ƀ��j���[��\�����邩
	WORD m_wSelectedID;	//	�I�����ꂽ���j���[����
	WORD m_wFirstShow; // ���߂� WM_INITMENUPOPUP ���Ă΂ꂽ�Ƃ��Ƀt���O�����낷
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
	StringBuffer m_text; //	���j���[�̃e�L�X�g
	const StringBuffer m_subname;	//	�T�u���j���[�̖��O(���������)
	WORD m_type;	//	���ڃ^�C�v
	WORD m_state;	//	�`�F�b�N�E�L��/�����̏��
	HMENU m_hmenu;	//	�T�u���j���[�̃��j���[�n���h��(���������)

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
	const StringBuffer m_name;	//	���j���[�̖��O
	HMENU m_hMenu;		//	���j���[�n���h��
	BOOL m_bInitDone;	//	���j���[���\�z�ς݂��ǂ���
	BOOL m_bExMenu;		//	�g�����j���[���ǂ���
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

	//	name �����[�g�Ƃ��郁�j���[�̃n���h���������ċA�I�ɗ\�񂷂�
	HMENU precreateMenuRecursive(const StringBuffer& name);

	//	���ۂ̃��j���[�̍\�z(�w�胁�j���[�̂�)
	HMENU createMenu(HMENU);

	//	���j���[�n���h���̍폜
	void deleteMenu();

private:
	Menu* m_pLastMenu;	//	Poke �̑Ώۂ� Menu �N���X�ւ̃|�C���^
};

#endif

