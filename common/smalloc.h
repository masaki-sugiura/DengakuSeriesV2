//	$Id: smalloc.h,v 1.3 2002-09-26 13:13:24 sugiura Exp $
/*
 *	smalloc.h
 *	共有メモリ領域についての malloc, free インターフェイスの提供
 */

#ifndef DENGAKUSERIES_CLASSES_SMALLOC
#define DENGAKUSERIES_CLASSES_SMALLOC

#include "strbuf.h"

#define SMA_PAGESIZE     0x01000000  // 16MB

// allocation unit
union SMAHeader {
	struct {
		UINT m_iptr; // index of next header
		UINT m_size; // size of block
	} m_info;
	ULONG m_align;
};

typedef union SMAHeader* LPSMAHeader;

struct PageHeader {
	UINT m_nShareCount;
	UINT m_idxMaster;
	UINT m_idxFreeP;
};

static inline int
allocate_offset()
{
	return ((sizeof(PageHeader) + sizeof(SMAHeader) - 1) / sizeof(SMAHeader))
			* sizeof(SMAHeader);
}

class FileMap {
public:
	FileMap(const StringBuffer& name, UINT pagesize);
	virtual ~FileMap();

	void* m_header;
	UINT m_pagesize;
	BOOL m_bAlreadyExist;
	const StringBuffer m_name;

	void* addr(UINT iptr);

	UINT index(void* ptr) const
	{
		return isThisPtr(ptr) ? ((char*)ptr - (char*)m_header) : 0;
	}

	BOOL isThisPtr(void* ptr) const
	{
		return	(ptr >= (char*)m_header + allocate_offset()) &&
				(ptr <  (char*)m_header + m_pagesize);
	}

private:
	HANDLE m_hFileMap;

	FileMap(const FileMap&);
	FileMap& operator=(const FileMap&);
};

typedef FileMap* LPFileMap;

class SMAlloc {
public:
	SMAlloc(const StringBuffer& name, UINT inipagenum = 1);
	~SMAlloc();

	UINT alloc(UINT size);
	void free(UINT iptr);

	void* addr(UINT iptr)
	{
		return m_pFileMap->addr(iptr);
	}

	UINT index(void* ptr)
	{
		return m_pFileMap->index(ptr);
	}

	BOOL isThisPtr(void* ptr)
	{
		return m_pFileMap->isThisPtr(ptr);
	}

	void lock() const { ::WaitForSingleObject(m_hMutex, INFINITE); }
	void release() const { ::ReleaseMutex(m_hMutex); }

	UINT sharedNum() { return getPageHeader()->m_nShareCount; }
	BOOL isShared() { return sharedNum() > 1; }

	UINT getMasterIndex() { return getPageHeader()->m_idxMaster; }
	void setMasterIndex(UINT index);

private:
	HANDLE m_hMutex;
	FileMap* m_pFileMap;

	PageHeader* getPageHeader() { return (PageHeader*)m_pFileMap->m_header; }

	void initAllocator(PageHeader* pHeader);
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

//	メモリ不足の時に投げられる例外クラス
class NoMemoryError {
};

//	共有メモリの名前が不正だった場合に投げられる例外クラス
class InvalidObjectNameException {
};

#endif
