//	$Id: enumpath.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	enumpath.h
 *	enum*** ‚ÉŠÖ‚·‚éƒNƒ‰ƒX
 */

#ifndef DENGAKUSERIES_CLASSES_ENUMPATH
#define DENGAKUSERIES_CLASSES_ENUMPATH

#include "linklist.h"
#include "stack.h"
#include "pathname.h"

#define ENUMPATH_FINDFILE		0x00000001
#define ENUMPATH_FINDDIR		0x00000002
#define ENUMPATH_EXCLUDEDOTS	0x00000010
#define ENUMPATH_SORTREVERSE	0x00000100

#pragma pack(push,4)
typedef struct path_tree_tag {
	struct path_tree_tag* m_lower;	//	pointer to path_tree lower than this
	struct path_tree_tag* m_upper;	//	pointer to path_tree upper than this
	WIN32_FIND_DATA m_fd;
} path_tree;
#pragma pack(pop)

typedef int (*PFN_COMPARE)(path_tree*, path_tree*);

#define PTBUF_BLOCKSIZE 32
#pragma pack(push,4)
typedef struct pname_buf_tag {
	int   m_head;
	path_tree m_buf[PTBUF_BLOCKSIZE];
	pname_buf_tag() : m_head(0) {}
} pt_buf;
#pragma pack(pop)

class EnumSortedPath {
public:
	EnumSortedPath(
				const StringBuffer& dir,
				const StringBuffer& filter,
				const StringBuffer& s_order,
				DWORD flags
			);
	~EnumSortedPath();

	BOOL isValid() const
	{
		return	m_dwAttrTopDir != 0xFFFFFFFF &&
				(m_dwAttrTopDir&FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	DWORD getFlag() const { return m_flags; }

	int doEnum();
	const StringBuffer findNext();

private:
	PathName m_topdir;
	const StringBuffer m_filter;
	DWORD m_flags;
	pt_buf* m_cur_pt_buf;
	LinkList<pt_buf> m_pt_buf_list;

	DWORD m_dwAttrTopDir;
	path_tree* m_top;	//	top of the tree
	Stack<path_tree>* m_pStk;
	PFN_COMPARE m_pfnCompare[3];
	int m_ncOrder;	//	order of name comparison

	BOOL tobeAdded(const WIN32_FIND_DATA&) const;
	path_tree* allocBuf(const WIN32_FIND_DATA*);
	path_tree* addPath(path_tree**,path_tree**);	//	add path
	void pushLower(const path_tree*);
};

class InvalidArgumentException {
	//	just only to notify error
};

#endif
