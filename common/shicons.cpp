//	$Id: shicons.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	shicons.cpp
 *	shell32.dll からアイコンを抜き出す＆管理するクラス
 */

#include "shicons.h"

ShellIcons::ShellIcons()
	:	m_nIconNum(32),
		m_nCurIconNum(0)
{
	m_phIcons = new HICON[m_nIconNum];
	m_nIconSize = ::GetSystemMetrics(SM_CXSMICON);
}

ShellIcons::~ShellIcons()
{
	for (int i = 0; i < m_nCurIconNum; i++) {
		::DestroyIcon(m_phIcons[i]);
	}
	delete [] m_phIcons;
}

int
ShellIcons::existIcon(LPCSTR iconfile, int index) const
{
	StringBuffer buf(iconfile,-1,8);
	buf.append('|').append(index);
	int ret = m_HashIcon.getValue(buf);
	return (ret > 0) ? ret : -1;
}

int
ShellIcons::addIconEntry(
	HIMAGELIST hImgList,
	LPCSTR iconfile,
	int index,
	const HICON hIcon)
{
	int ret = this->existIcon(iconfile,index);
	if (ret > 0) return ret;
	if (ImageList_ReplaceIcon(hImgList,-1,hIcon) <= 0) return -1;
	addIcon(::CopyIcon(hIcon));
	StringBuffer buf(iconfile,-1,8);
	buf.append('|').append(index);
	m_HashIcon.setValue(buf,m_nCurIconNum);
	return m_nCurIconNum;
}

int
ShellIcons::addIconEntry(HIMAGELIST hImgList, const SHFILEINFO& shFileInfo)
{
	HICON hIcon = ::CopyIcon(shFileInfo.hIcon);
	if (ImageList_ReplaceIcon(hImgList,-1,hIcon) <= 0) {
		::DestroyIcon(hIcon);
		return -1;
	}
	addIcon(hIcon);
	return m_nCurIconNum;
}

void
ShellIcons::addIcon(const HICON hIcon)
{
	if (m_nCurIconNum >= m_nIconNum) {
		m_nIconNum += 8;
		HICON* phIcons = new HICON[m_nIconNum];
		::CopyMemory(phIcons,m_phIcons, sizeof(HICON) * m_nCurIconNum);
		::ZeroMemory(phIcons + m_nCurIconNum,
					sizeof(HICON) * (m_nIconNum - m_nCurIconNum));
		delete[] m_phIcons;
		m_phIcons = phIcons;
	}
	m_phIcons[m_nCurIconNum++] = hIcon;
}

