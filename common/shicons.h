//	$Id: shicons.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	shicons.h
 *	shell32.dll からアイコンを抜き出す＆管理するクラス
 */

#ifndef DENGAKUSERIES_CLASSES_SHELLICONS
#define DENGAKUSERIES_CLASSES_SHELLICONS

#include "hashtbl.h"
#include <shlobj.h>

class ShellIcons {
public:
	ShellIcons();
	~ShellIcons();

	int existIcon(LPCSTR,int) const;
	int addIconEntry(HIMAGELIST hImgList, LPCSTR, int, const HICON);
	int addIconEntry(HIMAGELIST hImgList, const SHFILEINFO& shFileInfo);

	HICON getIconHandle(int index) const
	{
		return m_phIcons[index];
	}

	int getIconSize() const
	{ return m_nIconSize; }

private:
	int m_nIconNum;
	int m_nCurIconNum;
	HICON* m_phIcons;
	HashTable<int,32> m_HashIcon;
	int m_nIconSize;

	void addIcon(const HICON);
};

#endif
