//	$Id: shicons.cpp,v 1.2 2002-02-19 15:34:22 sugiura Exp $
/*
 *	shicons.cpp
 *	shell32.dll からアイコンを抜き出す＆管理するクラス
 */

#include "shicons.h"

ShellIcons::ShellIcons()
	:	m_nCurIconNum(0),
		m_phIcons(32)
{
	m_nIconSize = ::GetSystemMetrics(SM_CXSMICON);
}

ShellIcons::~ShellIcons()
{
	for (int i = 0; i < m_nCurIconNum; i++) {
		::DestroyIcon(m_phIcons[i]);
	}
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
	if (m_nCurIconNum >= m_phIcons.size()) {
		m_phIcons.resize(m_phIcons.size() + 8);
		m_phIcons.zero(m_nCurIconNum, -1);
	}
	m_phIcons[m_nCurIconNum++] = hIcon;
}

