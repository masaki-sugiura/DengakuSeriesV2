//	$Id: enumpath.cpp,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	enumpath.cpp
 *	enum*** �Ɋւ���N���X�̎���
 */

#include "strutils.h"
#include "enumpath.h"
#include "tokenizer.h"

static int
compareByName(const WIN32_FIND_DATA& lhs,
			  const WIN32_FIND_DATA& rhs)
{
	return lstrcmp(lhs.cFileName,rhs.cFileName);
}

static int
compareByExt(const WIN32_FIND_DATA& lhs,
			 const WIN32_FIND_DATA& rhs)
{
	LPCSTR	ptr1 = lstrrchr(lhs.cFileName,'.'),
			ptr2 = lstrrchr(rhs.cFileName,'.');
	if (ptr1 == NULL) ptr1 = "";
	else ptr1++;
	if (ptr2 == NULL) ptr2 = "";
	else ptr2++;
	return lstrcmp(ptr1,ptr2);
}

static int
compareBySize(const WIN32_FIND_DATA& lhs,
			  const WIN32_FIND_DATA& rhs)
{
	int diff = lhs.nFileSizeHigh - rhs.nFileSizeHigh;
	if (diff != 0) return diff;
	return lhs.nFileSizeLow - rhs.nFileSizeLow;
}

static int
compareByTime(const WIN32_FIND_DATA& lhs,
			  const WIN32_FIND_DATA& rhs)

{
	//	explorer �Ɠ����\�[�g�����ɂȂ�悤�ɋt�̒l��Ԃ�
	return (int)::CompareFileTime(&lhs.ftLastWriteTime,
								  &rhs.ftLastWriteTime);
}

PathComparator::PathComparator(const StringBuffer& sbOrder, BOOL bReverse)
	: m_bReverse(bReverse)
{
	LPCSTR ptr = (LPCSTR)sbOrder;
	int findex = 0, bn = 0, be = 0, bs = 0, bt = 0;
	for (int i = 0; i < 4; i++) {
		if (*ptr == '\0') break;
		switch (*ptr++) {
		case 'n':
			if (bn) throw InvalidArgumentException();
			bn = 1;
//			m_pfnCompare[i] = (PFN_COMPARE)compareByName;
			m_ncOrder = i;
			break;
		case 'e':
			if (be) throw InvalidArgumentException();
			be = 1;
			m_pfnCompare[findex++] = (PFN_COMPARE)compareByExt;
			break;
		case 's':
			if (bs) throw InvalidArgumentException();
			bs = 1;
			m_pfnCompare[findex++] = (PFN_COMPARE)compareBySize;
			break;
		case 't':
			if (bt) throw InvalidArgumentException();
			bt = 1;
			m_pfnCompare[findex++] = (PFN_COMPARE)compareByTime;
			break;
		default:
			throw InvalidArgumentException();
		}
	}
	if (!bn || !be || !bs || !bt) throw InvalidArgumentException();
}

int
PathComparator::doCompare(const WIN32_FIND_DATA& lhs,
						  const WIN32_FIND_DATA& rhs) const
{
	// "." �� ".." �̓\�[�g���@�Ɋ֌W�Ȃ���ɍŏ�
	int lipn = IsPathNameDots(lhs.cFileName),
		ripn = IsPathNameDots(rhs.cFileName);
	if ((lipn | ripn) != 0) {
		if (lipn == 0)      return  1; // normal file > dot
		else if (ripn == 0) return -1; // dot < normal file
		return lipn - ripn; // dot ?? dot
	}

	int compName, comp;
	if ((compName = compareByName(lhs,rhs)) == 0) return 0;

#define COMPARE(i) ((m_ncOrder == i) ? compName : (*m_pfnCompare[i])(lhs,rhs))

	// m_pfnCompare[]() �����ԂɌĂсA�Ⴂ���������炻�̒l�� comp �ɑ��
	// ��₱�����̂͊��ɍs�������O��r�̌��ʂ��ė��p���邽�߂����c
	if ((comp = COMPARE(0)) == 0 &&
		(comp = COMPARE(1)) == 0 &&
		(comp = COMPARE(2)) == 0) comp = compName;
	return m_bReverse == 0 ? comp : -comp;
}

EnumSortedPath::EnumSortedPath(
	const StringBuffer& topdir,
	const StringBuffer& filter,
	const StringBuffer& s_order,
	DWORD flags)
	:	Sorter<WIN32_FIND_DATA>(
			new PathComparator(s_order,
							   (flags&ENUMPATH_SORTREVERSE) != 0)),
		m_topdir(topdir),
		m_filter(filter),
		m_flags(flags)
{
	m_dwAttrTopDir = m_topdir.getAttributes();
	if (m_dwAttrTopDir == 0xFFFFFFFF && lstrlen(m_topdir.getBaseName()) == 0)
		m_dwAttrTopDir = FILE_ATTRIBUTE_DIRECTORY;	//	root directory
}

int
EnumSortedPath::doEnum()
{
	Tokenizer wcard(m_filter,";",TRUE);
	HANDLE hFindFile;
	WIN32_FIND_DATA fd;
	while (wcard.hasMoreTokens()) {
		StringBuffer wcstr = wcard.getNextToken();
		if (wcstr.length() <= 0) continue;
		m_topdir.addPath(wcstr);
		hFindFile = ::FindFirstFile(m_topdir,&fd);
		m_topdir.delPath(1);
		if (hFindFile != INVALID_HANDLE_VALUE) {
			do {
				if (tobeAdded(fd)) {
					this->addElement(fd);
				}
			} while (::FindNextFile(hFindFile,&fd));
			::FindClose(hFindFile);
			m_flags |= ENUMPATH_EXCLUDEDOTS;
		}
	}
	// �ł��������v�f���X�^�b�N�̃g�b�v�ɂȂ�
	return this->initSequentialGet();
}

//	fd �ŕ\�����t�@�C���܂��̓t�H���_�����݂̌����Ώۂ��ǂ�����Ԃ�
BOOL
EnumSortedPath::tobeAdded(const WIN32_FIND_DATA& fd) const
{
	if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0) {
		//	�t�H���_�̏ꍇ
		//	dwFileAttributes == 0xFFFFFFFF : ���[�g�t�H���_���܂�
		return	(m_flags & ENUMPATH_FINDDIR) != 0 &&
				((m_flags & ENUMPATH_EXCLUDEDOTS) == 0 ||
				 !IsPathNameDots(fd.cFileName));
	} else {
		//	��ʂ̃t�@�C���̏ꍇ
		return (m_flags & ENUMPATH_FINDFILE) != 0;
	}
}

//	���݂̃X�^�b�N�g�b�v�̗v�f(�̃t�@�C����)��Ԃ��A
//	������傫��(���X�^�b�N�ɐς܂ꂽ�v�f���͏������͂���)�v�f��
//	�X�^�b�N�ɐς�
const StringBuffer
EnumSortedPath::findNext()
{
	const WIN32_FIND_DATA* pfd = this->getNextElement();
	if (pfd == NULL) return nullStr;
	return pfd->cFileName;
}

