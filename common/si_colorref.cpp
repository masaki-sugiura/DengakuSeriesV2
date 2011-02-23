//	$Id: si_colorref.cpp,v 1.1 2002/02/15 17:46:08 sugiura Exp $
/*
 *	si_colorref.cpp
 *	カラーテーブルの管理に関するメソッドの実装
 */

#include "session.h"
#include "cmdline.h"
#include "colortbl.h"

int
SessionInstance::si_setcolorref(
	const StringBuffer& cname,
	const StringBuffer& cref)
{
	if (cname.length() == 0 || cref.length() == 0) return FALSE;
	COLORREF color = ColorTable::colorStrToColorRef(cref);
	CRContainer* pCRC = m_ColorTbl.getItemByName(cname);
	if (pCRC == NULL) {
		pCRC = new CRContainer(cname, color);
		if (pCRC == NULL) return FALSE;
		return m_ColorTbl.addItem(pCRC) > 0;
	} else {
		pCRC->setColorRef(color);
		return TRUE;
	}
}

StringBuffer
SessionInstance::si_getcolorref(const StringBuffer& cname)
{
	CRContainer* pCRC = m_ColorTbl.getItemByName(cname);
	if (pCRC == NULL) return nullStr;
	return ColorTable::colorRefToColorStr(pCRC->getColorRef());
}

StringBuffer
SessionInstance::si_getcolorname(const StringBuffer& color)
{
	COLORREF cref = ColorTable::colorStrToColorRef(color);
	m_ColorTbl.initSequentialGet();
	StringBuffer ret(40);
	CRContainer* pCRC;
	while ((pCRC = m_ColorTbl.getNextItem()) != NULL) {
		if (pCRC->getColorRef() == cref) {
			ret.append(pCRC->getName());
			ret.append((TCHAR)',');
		}
	}
	ret.setcharAt(-1, '\0');
	return ret;
}

int
SessionInstance::si_loadcolorrefs(const StringBuffer& file)
{
	return m_ColorTbl.load(file);
}

int
SessionInstance::si_savecolorrefs(const StringBuffer& file)
{
	return m_ColorTbl.save(file);
}

