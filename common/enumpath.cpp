//	$Id: enumpath.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	enumpath.cpp
 *	enum*** に関するクラスの実装
 */

#include "enumpath.h"
#include "tokenizer.h"

static int
compareByName(path_tree* p1, path_tree* p2)
{
	return lstrcmp(p1->m_fd.cFileName,p2->m_fd.cFileName);
}

static int
compareByExt(path_tree* p1, path_tree* p2)
{
	LPCSTR	ptr1 = lstrrchr(p1->m_fd.cFileName,'.'),
			ptr2 = lstrrchr(p2->m_fd.cFileName,'.');
	if (ptr1 == NULL) ptr1 = "";
	else ptr1++;
	if (ptr2 == NULL) ptr2 = "";
	else ptr2++;
	return lstrcmp(ptr1,ptr2);
}

static int
compareBySize(path_tree* p1, path_tree* p2)
{
	return	(p1->m_fd.nFileSizeHigh > p2->m_fd.nFileSizeHigh) ?  1 :
			(p1->m_fd.nFileSizeHigh < p2->m_fd.nFileSizeHigh) ? -1 :
			(p1->m_fd.nFileSizeLow > p2->m_fd.nFileSizeLow) ?  1 :
			(p1->m_fd.nFileSizeLow < p2->m_fd.nFileSizeLow) ? -1 :
			0;
}

static int
compareByTime(path_tree* p1, path_tree* p2)
{
	//	explorer と同じソート順序になるように逆の値を返す
	return (int)::CompareFileTime(&p2->m_fd.ftLastWriteTime,
								&p1->m_fd.ftLastWriteTime);
}

EnumSortedPath::EnumSortedPath(
	const StringBuffer& topdir,
	const StringBuffer& filter,
	const StringBuffer& s_order,
	DWORD flags)
	:	m_topdir(topdir),
		m_filter(filter),
		m_flags(flags),
		m_cur_pt_buf(NULL),
		m_top(NULL), m_pStk(NULL),
		m_ncOrder(0)
{
	m_dwAttrTopDir = m_topdir.getAttributes();
	if (m_dwAttrTopDir == 0xFFFFFFFF && lstrlen(m_topdir.getBaseName()) == 0)
		m_dwAttrTopDir = FILE_ATTRIBUTE_DIRECTORY;	//	root directory
	LPCSTR ptr = (LPCSTR)s_order;
	int findex = 0, bn = 0, be = 0, bs = 0, bt = 0;
	for (int i = 0; i < 4; i++)
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
	if (!bn || !be || !bs || !bt) throw InvalidArgumentException();
}

EnumSortedPath::~EnumSortedPath()
{
	delete m_pStk;
}

int
EnumSortedPath::doEnum()
{
	Tokenizer wcard(m_filter,";",TRUE);
	int num = 0;
	HANDLE hFindFile;
	WIN32_FIND_DATA fd;
	path_tree *ptr = NULL;
	while (wcard.hasMoreTokens()) {
		StringBuffer wcstr = wcard.getNextToken();
		if (wcstr.length() <= 0) continue;
		m_topdir.addPath(wcstr);
		hFindFile = ::FindFirstFile(m_topdir,&fd);
		m_topdir.delPath(1);
		if (hFindFile != INVALID_HANDLE_VALUE) {
			do {
				if (tobeAdded(fd)) {
					if (ptr == NULL) ptr = allocBuf(&fd);
					else ptr->m_fd = fd;
					ptr = addPath(&ptr,&m_top); // NULL if SUCCESS!!
					num++;
				}
			} while (::FindNextFile(hFindFile,&fd));
			::FindClose(hFindFile);
			m_flags |= ENUMPATH_EXCLUDEDOTS;
		}
	}
	m_pStk = new Stack<path_tree>(num);
	pushLower(m_top);
	return num;
}

BOOL
EnumSortedPath::tobeAdded(const WIN32_FIND_DATA& fd) const
{
	if (fd.dwFileAttributes == 0xFFFFFFFF) return FALSE;
	else if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0) {
		return	(m_flags & ENUMPATH_FINDDIR) != 0 &&
				((m_flags & ENUMPATH_EXCLUDEDOTS) == 0 ||
				 !IsPathNameDots(fd.cFileName));
	} else {
		return (m_flags & ENUMPATH_FINDFILE) != 0;
	}
}

path_tree*
EnumSortedPath::allocBuf(const WIN32_FIND_DATA* pfd)
{
	if (m_cur_pt_buf == NULL ||
		m_cur_pt_buf->m_head >= PTBUF_BLOCKSIZE) {
		m_cur_pt_buf = new pt_buf;
		if (m_cur_pt_buf == NULL) return NULL;
		m_pt_buf_list.addItem(m_cur_pt_buf);
	}
	path_tree* ret = m_cur_pt_buf->m_buf + m_cur_pt_buf->m_head++;
	ret->m_lower = ret->m_upper = NULL;
	::CopyMemory(&ret->m_fd,pfd,sizeof(WIN32_FIND_DATA));
	return ret;
}

path_tree*
EnumSortedPath::addPath(path_tree** ppt, path_tree** prev)
{
	path_tree* ptr;
	int comp, compName;
	while (*prev != NULL) {
		ptr = *prev;
		if (IsPathNameDots(ptr->m_fd.cFileName)) { // * == "." || * == ".."
			prev = &ptr->m_upper;
			continue;
		}
		//	file is already registered
		if ((compName = compareByName(*ppt,ptr)) == 0) return *ppt;

#define COMPARE(i) ((m_ncOrder == i) ? compName : (*m_pfnCompare[i])(*ppt,ptr))

		if ((comp = COMPARE(0)) == 0 &&
			(comp = COMPARE(1)) == 0 &&
			(comp = COMPARE(2)) == 0) comp = compName;

		prev = (comp > 0) == ((m_flags&ENUMPATH_SORTREVERSE) == 0) ?
				&ptr->m_upper : &ptr->m_lower;
	}
	*prev = *ppt;
	return NULL;	//	return NULL if "SUCCESS"
}

void
EnumSortedPath::pushLower(const path_tree* top)
{
	while (top != NULL) {
		m_pStk->push(top);
		top = top->m_lower;
	}
}

const StringBuffer
EnumSortedPath::findNext()
{
	path_tree* ptr = m_pStk->pop();
	if (ptr == NULL) return nullStr;
	if (ptr->m_upper != NULL) pushLower(ptr->m_upper);
	return ptr->m_fd.cFileName;
}

