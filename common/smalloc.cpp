//	$Id: smalloc.cpp,v 1.5 2006-06-16 15:43:57 sugiura Exp $
/*
 *	smalloc.cpp
 *	共有メモリ領域についての malloc, free インターフェイスの提供
 */

#include "smalloc.h"

FileMap::FileMap(const StringBuffer& name, UINT pagesize)
	: m_pagesize(pagesize), m_name(name)
{
	if (m_name.length() == 0 || m_name.find('\\') != -1)
		throw InvalidObjectNameException();

	::SetLastError(0L);
	m_hFileMap = ::CreateFileMapping(
							(HANDLE)0xFFFFFFFF,
							NULL,
							PAGE_READWRITE,
							0, pagesize,
							name
						);
	if (m_hFileMap == NULL) throw NoMemoryError();

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

void*
FileMap::addr(UINT iptr)
{
	return iptr < m_pagesize ? ((char*)m_header + iptr) : NULL;
}

#ifdef _DEBUG
inline void
DumpOneSMAHeader(SMAlloc* pAllocator, LPSMAHeader p)
{
	char buf[80];
	wsprintf(buf, "index: %d, next: %d, size: %d\n",
			 pAllocator->index(p), p->m_info.m_iptr, p->m_info.m_size);
	::OutputDebugString(buf);
}

void
DumpSMAHeader(SMAlloc* pAllocator, const char* strHeader, LPSMAHeader p)
{
	LPSMAHeader pend = p;
	::OutputDebugString(strHeader);
	do {
		DumpOneSMAHeader(pAllocator, p);
		p = (LPSMAHeader)pAllocator->addr(p->m_info.m_iptr);
	} while (p != pend) ;
}
#endif

SMAlloc::SMAlloc(const StringBuffer& name, UINT inipagenum)
{
	//	access control 用の mutex の作成
	StringBuffer mtxName(name);
	mtxName.append("_mutex");

	::SetLastError(0);
	
	m_hMutex = ::CreateMutex(NULL, FALSE, mtxName);
	if (m_hMutex == NULL) {
		throw InvalidObjectNameException();
	}

	BOOL bAlreadyExist = (::GetLastError() == ERROR_ALREADY_EXISTS);

	SMA_AutoLock lock(*this);

	m_pFileMap = new FileMap(name, SMA_PAGESIZE * inipagenum);

	PageHeader* pHeader = getPageHeader();

	if (!bAlreadyExist) {
		pHeader->m_nShareCount = 1; // 共有数
		initAllocator(pHeader);
	} else {
		pHeader->m_nShareCount += 1; // 共有数のインクリメント
	}
}

SMAlloc::~SMAlloc()
{
	{
		SMA_AutoLock lock(*this);

		PageHeader* pHeader = getPageHeader();

		pHeader->m_nShareCount--;

		delete m_pFileMap;
	}

	::CloseHandle(m_hMutex);
}

void
SMAlloc::initAllocator(PageHeader* pHeader)
{
	LPSMAHeader p = (LPSMAHeader)((char*)pHeader + allocate_offset());
	UINT next_offset = allocate_offset() + sizeof(SMAHeader);
	p->m_info.m_size = 1;
	p->m_info.m_iptr = next_offset;
#ifdef DEBUG_SMALLOC
	p->m_dwPreMagic  = SMA_PRE_MAGIC;
	p->m_dwPostMagic = SMA_POST_MAGIC;
#endif
	p++;
	p->m_info.m_size = (m_pFileMap->m_pagesize - next_offset) / sizeof(SMAHeader);
	p->m_info.m_iptr = allocate_offset();
#ifdef DEBUG_SMALLOC
	p->m_dwPreMagic  = SMA_PRE_MAGIC;
	p->m_dwPostMagic = SMA_POST_MAGIC;
#endif
	pHeader->m_idxFreeP = allocate_offset(); // current freep index
}

UINT
SMAlloc::alloc(UINT size)
{
	UINT nunits = (size + sizeof(SMAHeader) - 1) / sizeof(SMAHeader) + 1;

	PageHeader* pHeader = getPageHeader();

	SMA_AutoLock lock(*this);

	LPSMAHeader prevp = (LPSMAHeader)addr(pHeader->m_idxFreeP), p;
	LPSMAHeader freep = prevp;

	for (p = (LPSMAHeader)addr(prevp->m_info.m_iptr);
		 /* p != freep */;
		 prevp = p, p = (LPSMAHeader)addr(p->m_info.m_iptr)) {
		if (p->m_info.m_size >= nunits) {
			if (p->m_info.m_size == nunits) {
				prevp->m_info.m_iptr = p->m_info.m_iptr;
			} else {
				p->m_info.m_size -= nunits;
				p += p->m_info.m_size;
				p->m_info.m_size = nunits;
			}
			pHeader->m_idxFreeP = index(prevp);
#ifdef DEBUG_SMALLOC
			p->m_dwPreMagic  = SMA_PRE_MAGIC;
			p->m_dwPostMagic = SMA_POST_MAGIC;
#endif
			return index((void*)(p + 1));
		}
		if (p == freep) break;
	}

	return 0;
}

void
SMAlloc::free(UINT iptr)
{
	PageHeader* pHeader = getPageHeader();

	SMA_AutoLock lock(*this);

	LPSMAHeader bp = (LPSMAHeader)addr(iptr) - 1,
				p = (LPSMAHeader)addr(pHeader->m_idxFreeP);

#ifdef DEBUG_SMALLOC
	if (bp->m_dwPreMagic != SMA_PRE_MAGIC) {
		::MessageBox(NULL, "PreHeader is broken!!", NULL, MB_OK);
	} else if (bp->m_dwPostMagic != SMA_POST_MAGIC) {
		::MessageBox(NULL, "PostHeader is broken!!", NULL, MB_OK);
	}
#endif

	for ( ;
		!(bp > p && bp < addr(p->m_info.m_iptr));
		p = (LPSMAHeader)addr(p->m_info.m_iptr)) {
		if (p >= addr(p->m_info.m_iptr) && p < bp) {
			// bp が指す領域が現在の空き領域のどれよりも後ろにある
			break;
		}
	}

	// この時点で p < bp (< p->next)

	LPSMAHeader np = (LPSMAHeader)addr(p->m_info.m_iptr);
	if (bp + bp->m_info.m_size == np) {
		// free する領域が次の空き領域に連接
		bp->m_info.m_size += np->m_info.m_size;
		bp->m_info.m_iptr = np->m_info.m_iptr;
	} else if (bp < np) {
		// p < bp < p->next
		bp->m_info.m_iptr = index(np);
	} else {
		// p < bp (last free region)
		bp->m_info.m_iptr = p->m_info.m_iptr;
	}

	if (p + p->m_info.m_size == bp) {
		// p が指す領域が bp が指す領域と隣接
		p->m_info.m_size += bp->m_info.m_size;
		p->m_info.m_iptr = bp->m_info.m_iptr;
	} else {
		p->m_info.m_iptr = index(bp);
	}

	pHeader->m_idxFreeP = index(p);
}

void
SMAlloc::setMasterIndex(UINT index)
{
	SMA_AutoLock lock(*this);
	getPageHeader()->m_idxMaster = index;
}
