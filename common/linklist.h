//	$Id: linklist.h,v 1.3 2002/02/19 15:34:21 sugiura Exp $
/*
 *	linklist.h
 *	(単方向)リンクリストクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_LINKLIST
#define	DENGAKUSERIES_CLASSES_LINKLIST

#include "strbuf.h"
#include "hashtbl.h"

class PtrLinkList {
protected:
	PtrLinkList();
	virtual ~PtrLinkList() = 0;

	int	itemNum() const { return m_num; }
	int	addItem_(LPVOID,int);
	int	delItem_(LPVOID);
	int	getItemIndex_(LPVOID) const;
	LPVOID getItem_(int) const;
	int	initSequentialGet_(int) const;
	LPVOID getNextItem_() const;

private:
	struct PtrLinkListContainer {
		PtrLinkListContainer* m_next;
		LPVOID m_pvalue;
	};
	PtrLinkListContainer *m_root, *m_last;
	mutable PtrLinkListContainer *m_sgcurptr;
	int	m_num;

	PtrLinkList(const PtrLinkList&);
	PtrLinkList& operator=(const PtrLinkList&);
};

template<class T, bool bTrueDelete = true>
class LinkList : private PtrLinkList {
public:
	virtual ~LinkList()
	{
		if (!bTrueDelete) return;
		this->LinkList::initSequentialGet();
		T* ptr;
		while ((ptr = this->LinkList::getNextItem()) != NULL)
			delete ptr;
	}

//	using PtrLinkList::itemNum;	//	import
	int itemNum() const { return this->PtrLinkList::itemNum(); }

	virtual int	addItem(T* pvalue, int ind = -1)
	{ return PtrLinkList::addItem_(pvalue,ind); }

	virtual int	delItemByIndex(int ind)
	{
		T* pvalue = this->getItemByIndex(ind);
		if (pvalue == NULL) return -1;
		if (bTrueDelete) delete pvalue;
		return PtrLinkList::delItem_(pvalue);
	}
	virtual int	delItemByPtr(T* pvalue)
	{
		if (PtrLinkList::getItemIndex_(pvalue) < 0) return -1;
		if (bTrueDelete) delete pvalue;
		return PtrLinkList::delItem_(pvalue);
	}
	virtual int	getItemIndexByPtr(T* pvalue) const
	{ return PtrLinkList::getItemIndex_(pvalue); }

	virtual T* getItemByIndex(int ind) const
	{ return static_cast<T*>(PtrLinkList::getItem_(ind)); }

	virtual int initSequentialGet(int ind = 0) const
	{ return PtrLinkList::initSequentialGet_(ind); }

	virtual T* getNextItem() const
	{ return static_cast<T*>(PtrLinkList::getNextItem_()); }
};


//	NamedLinkListItem のリストのクラス
//	class T には T::getName() メソッドが定義されている必要がある
template<class T, bool bTrueDelete = true>
class NamedLinkList : public LinkList<T,bTrueDelete> {
public:
	int	addItem(T* pvalue, int ind = -1)
	{
		const StringBuffer& name = pvalue->getName();
		if (name.length() <= 0) return -1;
		T* oldvalue = m_HashName.getValue(name);
		if (oldvalue != NULL) {
			m_HashName.setValue(name,0);
			this->delItemByPtr(oldvalue);
		}
		if (this->LinkList<T,bTrueDelete>::addItem(pvalue,ind) <= 0) return -1;
		m_HashName.setValue(name,pvalue);
		return this->itemNum();
	}

	int delItemByName(const StringBuffer& name)
	{
		T* oldvalue = m_HashName.getValue(name);
		if (oldvalue == NULL) return -1;
		m_HashName.setValue(oldvalue->getName(),0);
		return this->LinkList<T,bTrueDelete>::delItemByPtr(oldvalue);
	}

	int	delItemByPtr(T* pvalue)
	{
		if (pvalue == NULL) return -1;
		m_HashName.setValue(pvalue->getName(),0);
		return this->LinkList<T,bTrueDelete>::delItemByPtr(pvalue);
	}

	int getItemIndexByName(const StringBuffer& name) const
	{
		T* pvalue = m_HashName.getValue(name);
		if (pvalue == NULL) return -1;
		return getItemIndexByPtr(pvalue);
	}

	T* getItemByName(const StringBuffer& name) const
	{
		return m_HashName.getValue(name);
	}

protected:
	HashTable<T*,11> m_HashName;
};

#endif

