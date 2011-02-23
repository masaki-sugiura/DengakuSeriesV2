//	$Id: linklist.cpp,v 1.2 2002/02/19 15:34:21 sugiura Exp $
/*
 *	linklist.cpp
 *	�����N���X�g�N���X
 */

#include "linklist.h"
#include <exception>

PtrLinkList::PtrLinkList()
	: m_root(NULL), m_last(NULL), m_sgcurptr(NULL), m_num(0)
{
	//	nothing to do more.
}

PtrLinkList::~PtrLinkList()
{
	PtrLinkList::PtrLinkListContainer *item = m_root, *next;
	while (item != NULL) {
		next = item->m_next;
		delete item;
		item = next;
	}
}

int
PtrLinkList::addItem_(LPVOID pvalue, int ind)
{
	if (pvalue == NULL || ind > m_num) return -1;	//	error!!

	PtrLinkList::PtrLinkListContainer*
		newitem = new PtrLinkList::PtrLinkListContainer;
	newitem->m_pvalue = pvalue;

	if (m_root == NULL) { // ���߂Ă̒ǉ�
		m_root = m_last = newitem;
		newitem->m_next = NULL;
	} else if (ind < 0) { // ���̃C���f�b�N�X�̏ꍇ�͍Ō�֒ǉ�
		m_last->m_next = newitem;
		m_last = newitem;
		newitem->m_next = NULL;
	} else if (ind == 0) { // �擪�ɒǉ�
		//	�擪�ɑ}��
		newitem->m_next = m_root; // �擪�̃|�C���^����
		m_root = newitem; // �}�������f�[�^��擪�ɃZ�b�g
	} else {
		//	���X�g�̓r��(�܂��͍Ō�)�ɑ}��
		PtrLinkList::PtrLinkListContainer* prev = m_root;
		for (--ind; ind > 0; --ind) prev = prev->m_next;
		newitem->m_next = prev->m_next;
		prev->m_next = newitem;
		if (newitem->m_next == NULL) m_last = newitem;
	}

	return ++m_num;
}

#if 0
int
PtrLinkList::delItemByIndex_(int ind)
{
	if (m_num <= 0 || ind >= m_num) return -1;

	if (ind < 0) ind = m_num - 1;

	PtrLinkList::PtrLinkListContainer* ditem;
	if (ind == 0) {
		ditem = m_root;
		m_root = ditem->m_next;
		if (m_root == NULL) m_last = NULL; // �B��̗v�f���폜
	} else {
		PtrLinkList::PtrLinkListContainer* prev = m_root;
		while (prev != NULL && --ind > 0) prev = prev->m_next;
		if (prev == NULL) return -1;
		ditem = prev->m_next;
		prev->m_next = ditem->m_next;
		if (prev->m_next == NULL) m_last = prev;
	}
	delete ditem;

	return --m_num;
}
#endif

int
PtrLinkList::delItem_(LPVOID pvalue)
{
	if (pvalue == NULL || m_num <= 0) return -1;

	PtrLinkList::PtrLinkListContainer* ditem = NULL;
	if (pvalue == m_root->m_pvalue) {
		ditem = m_root;
		m_root = m_root->m_next;
		if (m_root == NULL) m_last = NULL; // �B��̗v�f���폜
	} else {
		PtrLinkList::PtrLinkListContainer* prev = m_root;
		while (prev != NULL && prev->m_next &&
				prev->m_next->m_pvalue != pvalue)
			prev = prev->m_next;
		if (prev == NULL || prev->m_next == NULL) return -1;
		ditem = prev->m_next;
		prev->m_next = ditem->m_next;
		if (prev->m_next == NULL) m_last = prev;
	}
	delete ditem;

	return --m_num;
}

int
PtrLinkList::getItemIndex_(LPVOID pvalue) const
{
	PtrLinkList::PtrLinkListContainer* it = m_root;
	for (int index = 0; it != NULL; it = it->m_next, index++)
		if (it->m_pvalue == pvalue) return index;
	return -1;
}

LPVOID
PtrLinkList::getItem_(int ind) const
{
	if (m_num <= 0 || ind >= m_num) return NULL;

	if (ind < 0) return m_last->m_pvalue;

	PtrLinkList::PtrLinkListContainer* item = m_root;
	while (item != NULL && ind-- > 0) item = item->m_next;
	return (item != NULL) ? item->m_pvalue : NULL;
}

int
PtrLinkList::initSequentialGet_(int ind) const
{
	m_sgcurptr = m_root;
	int i = 0;
	for ( ; i < ind; i++) this->getNextItem_();
	return m_num - i;
}

LPVOID
PtrLinkList::getNextItem_() const
{
	PtrLinkList::PtrLinkListContainer* ret = m_sgcurptr;
	if (ret == NULL) return NULL;
	m_sgcurptr = m_sgcurptr->m_next;
	return ret->m_pvalue;
}

