//	$Id: sorter.h,v 1.1 2002-01-16 15:57:23 sugiura Exp $
/*
 *	sorter.h
 *	ソートクラス
 */

#ifndef DENGAKUSERIES_CLASSES_SORTER
#define DENGAKUSERIES_CLASSES_SORTER

#include "stack.h"
#include "auto_ptr.h"
#include "block_alloc.h"

template<class T>
class Comparator {
public:
	virtual ~Comparator() {}

	virtual int doCompare(const T& lhs, const T& rhs) const = 0;
};

template<class U>
class _tree_element {
public:
	_tree_element<U>* m_prev;
	_tree_element<U>* m_next;
	U m_element;
};

template<class T>
class Sorter {
public:
	Sorter(Comparator<T>* pcomparator)
		: m_pComparator(pcomparator),
		  m_top(NULL), m_balloc(32), m_num(0),
		  m_pstElements(NULL)
	{}

	void setComparator(Comparator<T>* pcomparator)
	{
		m_pComparator = pcomparator;
	}

	int getElementCount() const { return m_num; }

	BOOL addElement(const T& new_elem)
	{
		_tree_element<T>** pptr = &m_top;
		int comp;
		while (*pptr != NULL) {
			comp = m_pComparator->doCompare((*pptr)->m_element, new_elem);
			if (comp == 0) {
				return FALSE;
			} else if (comp < 0) {
				pptr = &(*pptr)->m_next;
			} else {
				pptr = &(*pptr)->m_prev;
			}
		}
		_tree_element<T>* ptr = m_balloc.allocate();
		ptr->m_prev = ptr->m_next = NULL;
		ptr->m_element = new_elem;
		*pptr = ptr;
		m_num++;
		return TRUE;
	}

	int initSequentialGet()
	{
		m_pstElements = new Stack< _tree_element<T> >(m_num);
		pushLowerElements(m_top);
		return m_num;
	}

	const T* getNextElement()
	{
		_tree_element<T>* ptr = m_pstElements->pop();
		if (ptr == NULL) return NULL;
		pushLowerElements(ptr->m_next);
		return &ptr->m_element;
	}

private:
	Auto_Ptr< Comparator<T> > m_pComparator;

	BlockAllocator< _tree_element<T> > m_balloc;
	_tree_element<T>* m_top;
	int m_num;
	Auto_Ptr< Stack< _tree_element<T> > > m_pstElements;

	//	ptr 自身とより小さい要素を全てスタック(m_stElements)に追加
	//	ツリーのルート要素に対してこの関数を呼び出すと、スタックトップが
	//	ツリー中の最も小さい要素になる
	void pushLowerElements(_tree_element<T>* ptr)
	{
		while (ptr != NULL) {
			m_pstElements->push(ptr);
			ptr = ptr->m_prev;
		}
	}
};

#endif
