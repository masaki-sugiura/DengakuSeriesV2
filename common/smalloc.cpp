//	$Id: smalloc.cpp,v 1.2 2002-09-10 12:33:10 sugiura Exp $
/*
 *	smalloc.cpp
 *	共有メモリ領域についての malloc, free インターフェイスの提供
 */

#include "smalloc.h"

FileMap::FileMap(const StringBuffer& name, UINT pagesize)
	: m_pagesize(pagesize)
{
	::SetLastError(0L);
	m_hFileMap = ::CreateFileMapping(
							(HANDLE)0xFFFFFFFF,
							NULL,
							PAGE_READWRITE,
							0, pagesize,
							name
						);
	if (m_hFileMap == NULL) throw InvalidObjectNameException();
	m_bAlreadyExist = (::GetLastError() == ERROR_ALREADY_EXISTS);
	m_header = ::MapViewOfFile(
							m_hFileMap,
							FILE_MAP_ALL_ACCESS,
							0, 0,
							0
						);
}

FileMap::~FileMap()
{
	::UnmapViewOfFile(m_header);
	::CloseHandle(m_hFileMap);
}

SMAPage::SMAPage(const StringBuffer& name, UINT pagesize)
	: FileMap(name,pagesize)
{
	if (!m_bAlreadyExist) {
		LPSMAHeader p = (LPSMAHeader)m_header + 1;
		p->m_info.m_size = 1;
		p->m_info.m_iptr = 2 * sizeof(SMAHeader);
		p++;
		p->m_info.m_size = pagesize/sizeof(SMAHeader) - 3;
		p->m_info.m_iptr = 1 * sizeof(SMAHeader);
		*(int*)m_header = 1 * sizeof(SMAHeader); // current freep index
	}
}

SMAPage::~SMAPage()
{
	//	no operation.
}

UINT
SMAPage::alloc(UINT size)
{
	UINT nunits = (size + sizeof(SMAHeader) - 1)/sizeof(SMAHeader) + 1;
	LPSMAHeader prevp = (LPSMAHeader)addr(*(int*)m_header), p;
	LPSMAHeader freep = prevp;
	for (p = (LPSMAHeader)addr(prevp->m_info.m_iptr);
		 p != freep;
		 prevp = p, p = (LPSMAHeader)addr(p->m_info.m_iptr)) {
		if (p->m_info.m_size >= nunits) {
			if (p->m_info.m_size == nunits)
				prevp->m_info.m_iptr = p->m_info.m_iptr;
			else {
				p->m_info.m_size -= nunits;
				p += p->m_info.m_size;
				p->m_info.m_size = nunits;
			}
			*(int*)m_header = index(prevp);
			return index((void*)(p + 1));
		}
	}
	return 0;
}

void
SMAPage::free(UINT iptr)
{
	LPSMAHeader bp = (LPSMAHeader)addr(iptr) - 1,
				p = (LPSMAHeader)addr(*(int*)m_header);
	for ( ;
		!(bp > p && bp < addr(p->m_info.m_iptr));
		p = (LPSMAHeader)addr(p->m_info.m_iptr)) {
		if (p >= addr(p->m_info.m_iptr) &&
			(bp > p || bp < addr(p->m_info.m_iptr)))
			break;
	}
	if (bp + bp->m_info.m_size == addr(p->m_info.m_iptr)) {
		bp->m_info.m_size
			+= ((LPSMAHeader)addr(p->m_info.m_iptr))->m_info.m_size;
		bp->m_info.m_iptr
			= ((LPSMAHeader)addr(p->m_info.m_iptr))->m_info.m_iptr;
	} else {
		bp->m_info.m_iptr
			= ((LPSMAHeader)addr(p->m_info.m_iptr))->m_info.m_iptr;
	}
	if (p + p->m_info.m_size == bp) {
		p->m_info.m_size += bp->m_info.m_size;
		p->m_info.m_iptr = bp->m_info.m_iptr;
	} else {
		p->m_info.m_iptr = index(bp);
	}
	*(int*)m_header = index(p);
}

struct MasterPageHeader {
	UINT m_nShareCount;
	UINT m_nPageNum;
	UINT m_nMasterIndex;
};

class SMA_AutoLock {
public:
	SMA_AutoLock(SMAlloc& sma)
		: m_sma(sma)
	{
		m_sma.lock();
	}
	~SMA_AutoLock()
	{
		m_sma.release();
	}
private:
	SMAlloc& m_sma;
};

SMAlloc::SMAlloc(const StringBuffer& name, UINT pagesize, UINT inipagenum)
	: m_name(name), m_page_size(pagesize)
{
	if (m_page_size > SMA_MAXPAGESIZE) {
		m_page_size = SMA_MAXPAGESIZE;
		inipagenum = (pagesize * inipagenum) / SMA_MAXPAGESIZE;
	}
	if (inipagenum > SMA_MAXPAGENUM) throw NoMemoryError();
	int len = m_name.length();
	if (len <= 0) throw InvalidObjectNameException();
	//	access control 用の mutex の作成
	m_hMutex = ::CreateMutex(NULL,FALSE,m_name.append("_mutex"));
	if (m_hMutex == NULL) throw InvalidObjectNameException();
	m_name.append("_parent"); // master page

	SMA_AutoLock lock(*this);

	//	master page の作成
	m_pmaster = new FileMap(m_name, SMA_MASTERPAGE_SIZE);
	m_name.setlength(len);
	MasterPageHeader* pHeader = (MasterPageHeader*)m_pmaster->m_header;
	if (!m_pmaster->m_bAlreadyExist) {
		pHeader->m_nShareCount = 1; // 共有数
		pHeader->m_nPageNum = inipagenum; // ページ数
		setMasterIndex(0);
	} else {
		pHeader->m_nShareCount += 1; // 共有数のインクリメント
		if (pHeader->m_nPageNum > inipagenum)
			inipagenum = pHeader->m_nPageNum;
	}
	::ZeroMemory(m_ppage_list, sizeof(LPSMAPage) * SMA_MAXPAGENUM);
	for (UINT i = 0; i < inipagenum; i++) {
		m_name.append((TCHAR)'_').append((ULONG)i);
		m_ppage_list[i] = new SMAPage(m_name, m_page_size);
		m_name.setlength(len);
	}
}

SMAlloc::~SMAlloc()
{
	SMA_AutoLock lock(*this);
	MasterPageHeader* pHeader = (MasterPageHeader*)m_pmaster->m_header;
	pHeader->m_nShareCount--;
	for (int i = 0; i < (int)SMA_MAXPAGENUM; i++) {
		if (m_ppage_list[i] == NULL) break;
		delete m_ppage_list[i];
	}
	delete m_pmaster;
	::CloseHandle(m_hMutex);
}

UINT
SMAlloc::alloc(UINT size)
{
	if (size >= m_page_size) return 0;

	int len = m_name.length();
	MasterPageHeader* pHeader = (MasterPageHeader*)m_pmaster->m_header;

	SMA_AutoLock lock(*this);

	for (UINT iptr = 0; iptr < SMA_MAXPAGENUM; iptr++) {
		if (m_ppage_list[iptr] == NULL) {
			// no memory in current alloc pages
			m_name.append((TCHAR)'_').append((ULONG)iptr);
			m_ppage_list[iptr] = new SMAPage(m_name, m_page_size);
			m_name.setlength(len);
			if (m_ppage_list[iptr] == NULL) break;
			if (pHeader->m_nPageNum < iptr + 1)
				pHeader->m_nPageNum = iptr + 1;
		}
		UINT index = m_ppage_list[iptr]->alloc(size);
		if (index != 0) {
			return makeIndex(++iptr, index);
		}
	}
	return 0;
}

void
SMAlloc::free(UINT iptr)
{
	SMA_AutoLock lock(*this);
	LPSMAPage ppage = this->getSMAPagePtr(iptr);
	if (ppage != NULL) ppage->free(getBlockIndex(iptr));
}

void*
SMAlloc::addr(UINT iptr)
{
	LPSMAPage ppage = this->getSMAPagePtr(iptr);
	return ppage != NULL ? ppage->addr(getBlockIndex(iptr)) : NULL;
}

UINT
SMAlloc::index(void* ptr) const
{
	UINT pnumber = 0, index = 0;
	for ( ; pnumber < SMA_MAXPAGENUM; pnumber++) {
		if (m_ppage_list[pnumber] == NULL) break;
		else if (m_ppage_list[pnumber]->isThisPtr(ptr)) {
			index = m_ppage_list[pnumber]->index(ptr);
			break;
		}
	}
	return index != 0 ? makeIndex(++pnumber,index) : 0;
}

UINT
SMAlloc::getMasterIndex() const
{
	return ((MasterPageHeader*)m_pmaster->m_header)->m_nMasterIndex;
}

UINT
SMAlloc::setMasterIndex(UINT index)
{
	MasterPageHeader* pHeader = (MasterPageHeader*)m_pmaster->m_header;
	SMA_AutoLock lock(*this);
	UINT oldindex = pHeader->m_nMasterIndex;
	pHeader->m_nMasterIndex = index;
	return oldindex;
}

LPSMAPage
SMAlloc::getSMAPagePtr(UINT iptr)
{
	SMA_AutoLock lock(*this);

	UINT pnumber = getPageNumber(iptr),
		 index = getBlockIndex(iptr);
	if (pnumber == 0 || index == 0) return NULL;
	if (!m_ppage_list[pnumber - 1]) {
		int len = m_name.length();

		// ローカルのポインタテーブルが共有メモリのそれに追いついていない
		// →新しく SMAPage を作成
		for (UINT i = 0; i < pnumber; i++) {
			if (m_ppage_list[i]) continue;
			m_name.append((TCHAR)'_').append((ULONG)i);
			m_ppage_list[i] = new SMAPage(m_name, m_page_size);
			m_name.setlength(len);
		}
	}

	return m_ppage_list[pnumber - 1];
}

