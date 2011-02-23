//	$Id: enumpath.h,v 1.3 2002/02/15 17:46:08 sugiura Exp $
/*
 *	enumpath.h
 *	enum*** ‚ÉŠÖ‚·‚éƒNƒ‰ƒX
 */

#ifndef DENGAKUSERIES_CLASSES_ENUMPATH
#define DENGAKUSERIES_CLASSES_ENUMPATH

#include "sorter.h"
#include "pathname.h"

#define ENUMPATH_FINDFILE		0x00000001
#define ENUMPATH_FINDDIR		0x00000002
#define ENUMPATH_EXCLUDEDOTS	0x00000010
#define ENUMPATH_SORTREVERSE	0x00000100

typedef int (*PFN_COMPARE)(const WIN32_FIND_DATA&, const WIN32_FIND_DATA&);

class PathComparator : public Comparator<WIN32_FIND_DATA> {
public:
	PathComparator(const StringBuffer& sbOrder, BOOL bReverse);

	int doCompare(const WIN32_FIND_DATA& lhs, const WIN32_FIND_DATA& rhs) const;

private:
	BOOL m_bReverse;
	int m_ncOrder;
	PFN_COMPARE m_pfnCompare[3];
};

class EnumSortedPath : public Sorter<WIN32_FIND_DATA> {
public:
	EnumSortedPath(
				const StringBuffer& dir,
				const StringBuffer& filter,
				const StringBuffer& s_order,
				DWORD flags
			);

	BOOL isValid() const
	{
		return	m_dwAttrTopDir != 0xFFFFFFFF &&
				(m_dwAttrTopDir&FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
	StringBuffer getNext();

	DWORD getFlag() const { return m_flags; }

	int doEnum();

private:
	PathName m_topdir;
	const StringBuffer m_filter;
	DWORD m_flags;

	DWORD m_dwAttrTopDir;

	BOOL tobeAdded(const WIN32_FIND_DATA&) const;
};

class InvalidArgumentException {
	//	just only to notify error
};

#endif
