//	$Id: colortbl.h,v 1.2 2002-03-05 14:19:34 sugiura Exp $
/*
 *	colortbl.h
 *	カラーテーブルを管理するクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_COLORTBL
#define	DENGAKUSERIES_CLASSES_COLORTBL

#include "linklist.h"

class CRContainer {
public:
	CRContainer(const StringBuffer& cname, COLORREF cref)
		: m_cname(cname), m_cref(cref) {}

	const StringBuffer& getName() const { return m_cname; }
	COLORREF getColorRef() const { return m_cref; }
	void setColorRef(COLORREF cref) { m_cref = cref; }

private:
	StringBuffer m_cname;
	COLORREF m_cref;
};

class ColorTable : public NamedLinkList<CRContainer, FALSE> {
public:
	ColorTable()
	{
		this->init();
	}

	COLORREF getColorRef(const StringBuffer& cname) const
	{
		if (cname[0] == '#')
			return colorStrToColorRef(cname);
		CRContainer* pCRC = this->getItemByName(cname);
		if (pCRC != NULL) return pCRC->getColorRef();
		return 0;
	}

	int load(const StringBuffer&);
	int save(const StringBuffer&);

	static COLORREF colorStrToColorRef(const StringBuffer&);
	static StringBuffer colorRefToColorStr(COLORREF);

private:
	void init();
};

#endif
