//	$Id: colortbl.cpp,v 1.1 2002-02-15 17:46:08 sugiura Exp $
/*
 *	colortbl.cpp
 *	カラーテーブルを管理するクラスの実装
 */

#include "colortbl.h"
#include "str_tbl.h"
#include "strutils.h"

inline COLORREF
swap_bytes(COLORREF color)
{
	return ((0x000000ff & color) << 16) |
			(0x0000ff00 & color)        |
		   ((0x00ff0000 & color) >> 16);
}

void
ColorTable::init()
{
	while (this->itemNum() > 0) this->delItemByIndex(0);
	this->addItem(new CRContainer("black",   RGB(0x00, 0x00, 0x00)));
	this->addItem(new CRContainer("white",   RGB(0xFF, 0xFF, 0xFF)));
	this->addItem(new CRContainer("red",     RGB(0xFF, 0x00, 0x00)));
	this->addItem(new CRContainer("lime",    RGB(0x00, 0xFF, 0x00)));
	this->addItem(new CRContainer("blue",    RGB(0x00, 0x00, 0xFF)));
	this->addItem(new CRContainer("fuchsia", RGB(0xFF, 0x00, 0xFF)));
	this->addItem(new CRContainer("yellow",  RGB(0xFF, 0xFF, 0x00)));
	this->addItem(new CRContainer("aqua",    RGB(0x00, 0xFF, 0xFF)));
	this->addItem(new CRContainer("navy",    RGB(0x00, 0x00, 0x80)));
	this->addItem(new CRContainer("teal",    RGB(0x00, 0x80, 0x80)));
	this->addItem(new CRContainer("green",   RGB(0x00, 0x80, 0x00)));
	this->addItem(new CRContainer("maroon",  RGB(0x80, 0x00, 0x00)));
	this->addItem(new CRContainer("purple",  RGB(0x80, 0x00, 0x80)));
	this->addItem(new CRContainer("olive",   RGB(0x80, 0x80, 0x00)));
	this->addItem(new CRContainer("gray",    RGB(0x80, 0x80, 0x80)));
	this->addItem(new CRContainer("silver",  RGB(0xC0, 0xC0, 0xC0)));
}

int
ColorTable::load(const StringBuffer& file)
{
	this->init();
	TCHAR buf[0x10000]; // 64KBytes
	::GetPrivateProfileString(GetString(STR_COLORTBL_SECTION),
							  NULL, "", buf, 0x10000, file);
	LPCSTR pcname = buf;
	while (*pcname != '\0') {
		TCHAR cref[8];
		::GetPrivateProfileString(GetString(STR_COLORTBL_SECTION),
								  pcname, "", cref, 8, file);
		if (cref[0] == '#' && lstrlen(cref) == 7) {
			this->addItem(new CRContainer(pcname, colorStrToColorRef(cref)));
		}
		pcname += lstrlen(pcname) + 1;
	}
	return TRUE;
}

int
ColorTable::save(const StringBuffer& file)
{
	::WritePrivateProfileString(GetString(STR_COLORTBL_SECTION),
								NULL, "", file);
	this->initSequentialGet();
	CRContainer* pCRC;
	while ((pCRC = this->getNextItem()) != NULL) {
		::WritePrivateProfileString(GetString(STR_COLORTBL_SECTION),
									pCRC->getName(),
									colorRefToColorStr(pCRC->getColorRef()),
									file);
	}
	return TRUE;
}

COLORREF
ColorTable::colorStrToColorRef(const StringBuffer& cref)
{
	COLORREF color = 0;
	if (cref.charAt(0) == '#' && cref.length() >= 7) {
		LPCSTR pcolor = cref;
		++pcolor;
		for (int i = 0; i < 6; i++) {
			color <<= 4;
			if (IsCharDigit(*pcolor))
				color += *pcolor - '0';
			else if (IsCharXDigit(*pcolor))
				color += *pcolor - (IsCharUpper(*pcolor) ? 'A' : 'a') + 10;
			pcolor++;
		}
		color = swap_bytes(color);
	}
	return color;
}

StringBuffer
ColorTable::colorRefToColorStr(COLORREF color)
{
	TCHAR buf[8];
	wsprintf(buf, "#%-06.6x", 0x00FFFFFF & swap_bytes(color));
	return buf;
}

