//	$Id: smalloc.h,v 1.2 2002-09-10 12:33:10 sugiura Exp $
/*
 *	smalloc.h
 *	共有メモリ領域についての malloc, free インターフェイスの提供
 */

#ifndef DENGAKUSERIES_CLASSES_SMALLOC
#define DENGAKUSERIES_CLASSES_SMALLOC

#include "strbuf.h"

// allocation unit
typedef union SMAHeader_tag {
	struct {
		UINT m_iptr; // index of next header
		UINT m_size; // size of block
	} m_info;
	ULONG m_align;
} SMAHeader, *LPSMAHeader;


class FileMap {
public:
	FileMap(const StringBuffer& name, UINT pagesize);
	virtual ~FileMap();

	void* m_header;
	UINT m_pagesize;
	BOOL m_bAlreadyExist;

private:
	HANDLE m_hFileMap;

	FileMap(const FileMap&);
	FileMap& operator=(const FileMap&);
};

typedef FileMap* LPFileMap;

// page manager
class SMAPage : protected FileMap {
public:
	SMAPage(const StringBuffer& name, UINT pagesize);
	~SMAPage();

	UINT alloc(UINT size);
	void free(UINT iptr);

	BOOL isThisPtr(void* ptr) const
	{
		return	(ptr >= (LPSMAHeader)m_header + 2) &&
				(ptr < (LPSMAHeader)m_header + m_pagesize);
	}

	void* addr(UINT index) const { return (void*)((BYTE*)m_header + index); }
	UINT index(void* ptr) const { return (BYTE*)ptr - (BYTE*)m_header; }
};

typedef SMAPage* LPSMAPage;

#define SMA_PAGENUMBER_OFFSET 20
#define SMA_PAGENUMBER_MASK   0xFFF00000
#define SMA_BLOCKINDEX_MASK   0x000FFFFF
#define SMA_MAXPAGESIZE       SMA_BLOCKINDEX_MASK
#define SMA_MAXPAGENUM        (((UINT)SMA_PAGENUMBER_MASK) >> SMA_PAGENUMBER_OFFSET)
#define SMA_MASTERPAGE_SIZE   (4 * sizeof(UINT))

class SMAlloc {
public:
	SMAlloc(const StringBuffer& name, UINT pagesize, UINT inipagenum = 1);
	~SMAlloc();

	UINT alloc(UINT size);
	void free(UINT iptr);

	void* addr(UINT iptr);
	UINT index(void* ptr) const;

	UINT getMasterIndex() const;
	UINT setMasterIndex(UINT index);

	void lock() const { ::WaitForSingleObject(m_hMutex,INFINITE); }
	void release() const { ::ReleaseMutex(m_hMutex); }

	UINT sharedNum() const { return *(UINT*)m_pmaster->m_header; }
	BOOL isShared() const { return sharedNum() > 1; }

private:
	StringBuffer m_name;
	UINT m_page_size;
	HANDLE m_hMutex;
	LPFileMap m_pmaster;
	LPSMAPage m_ppage_list[SMA_MAXPAGENUM];

	SMAlloc(const SMAlloc&);
	SMAlloc& operator=(const SMAlloc&);

	LPSMAPage getSMAPagePtr(UINT iptr);
	UINT getPageNumber(UINT iptr) const
	{
		return (iptr >> SMA_PAGENUMBER_OFFSET);
	}
	UINT getBlockIndex(UINT iptr) const
	{
		return (iptr & SMA_BLOCKINDEX_MASK);
	}
	UINT makeIndex(UINT pnumber, UINT bindex) const
	{
		return (pnumber << SMA_PAGENUMBER_OFFSET) | bindex;
	}
};

//	メモリ不足の時に投げられる例外クラス
class NoMemoryError {
};

//	共有メモリの名前が不正だった場合に投げられる例外クラス
class InvalidObjectNameException {
};

#endif
