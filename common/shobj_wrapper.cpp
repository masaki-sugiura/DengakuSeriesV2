//	$Id: shobj_wrapper.cpp,v 1.1 2002-02-15 17:46:08 sugiura Exp $
/*
 *	shobj_wrapper.cpp
 *	Shell オブジェクトをラップするクラスの実装
 */

#include "shobj_wrapper.h"

SHOBJ_Allocator g_SHOBJ_Allocator;

SHOBJ_Allocator::SHOBJ_Allocator()
{
	::SHGetMalloc(&m_lpMAlloc);
}

SHOBJ_Allocator::~SHOBJ_Allocator()
{
	m_lpMAlloc->Release();
}

void*
SHOBJ_Allocator::allocate(DWORD size)
{
	return m_lpMAlloc->Alloc(size);
}

void
SHOBJ_Allocator::free(void* ptr)
{
	m_lpMAlloc->Free(ptr);
}

//	terminator の長さは除いた ITEMIDLIST 配列の長さを返す
DWORD
LPIDL_Wrapper::getListSize() const
{
	if (m_lpShObj == NULL) return 0;

	DWORD cb = 0;
	for (LPITEMIDLIST pidOne = m_lpShObj;
		pidOne->mkid.cb != 0;
		pidOne = getNextListPtr(pidOne)) {
		cb += pidOne->mkid.cb;
	}

	return cb;
}

LPIDL_Wrapper
LPIDL_Wrapper::getNextItem() const
{
	if (m_pEnumHeader == NULL) return NULL;
	int cb = m_pEnumHeader->mkid.cb;
	LPITEMIDLIST
		newIDL = (LPITEMIDLIST)g_SHOBJ_Allocator.allocate(cb + sizeof(USHORT));
	if (newIDL == NULL) return NULL;
	::CopyMemory(newIDL, m_pEnumHeader, cb);
	*((USHORT*)(((LPBYTE)newIDL) + cb)) = 0;
	m_pEnumHeader = getNextListPtr(m_pEnumHeader);
	if (m_pEnumHeader->mkid.cb == 0) m_pEnumHeader = NULL;
	return newIDL;
}

LPSF_Wrapper
LPSF_Wrapper::getDesktopFolder()
{
	LPSHELLFOLDER lpsfDesktop = NULL;

	::SHGetDesktopFolder(&lpsfDesktop);

	return lpsfDesktop;
}

StringBuffer
LPSF_Wrapper::getDisplayNameOf(const LPIDL_Wrapper& lpidlFolder, DWORD flag) const
{
	STRRET strRet;
	m_lpShObj->GetDisplayNameOf(lpidlFolder, flag, &strRet);

	switch (strRet.uType) {
	case STRRET_OFFSET:
		return (LPCSTR)(LPCITEMIDLIST)lpidlFolder + strRet.uOffset;

	case STRRET_WSTR:
		{
			TCHAR buf[MAX_PATH];
			buf[0] = '\0';
			::WideCharToMultiByte(CP_ACP,
								WC_COMPOSITECHECK,
								strRet.pOleStr,-1,
								buf,MAX_PATH,
								0,NULL);
			g_SHOBJ_Allocator.free(strRet.pOleStr);
			return buf;
		}

	case STRRET_CSTR:
		return strRet.cStr;

	default:
		return nullStr;
	}
}

