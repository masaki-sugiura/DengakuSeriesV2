//	$Id: shobj_wrapper.h,v 1.2 2002/08/05 16:06:17 sugiura Exp $
/*
 *	shobj_wrapper.h
 *	Shell オブジェクトをラップするクラス
 */

#ifndef DENGAKUSERIES_CLASSES_SHOBJ_WRAPPER
#define DENGAKUSERIES_CLASSES_SHOBJ_WRAPPER

#include "strbuf.h"
#include "auto_ptr.h"
#include <shlobj.h>

class SHOBJ_Allocator {
public:
	SHOBJ_Allocator();
	~SHOBJ_Allocator();

	void* allocate(DWORD size);
	void  free(void*);

private:
	LPMALLOC m_lpMAlloc;
};

extern SHOBJ_Allocator g_SHOBJ_Allocator;

template<class T>
class LPSHOBJ_Wrapper {
public:
	LPSHOBJ_Wrapper(T* lpShObj = NULL)
		: m_lpShObj(lpShObj), m_bOwner(TRUE)
	{}
	LPSHOBJ_Wrapper(const LPSHOBJ_Wrapper<T>& rhs)
		: m_lpShObj(rhs.m_lpShObj),
		  m_bOwner(rhs.m_bOwner)
	{
		rhs.m_bOwner = FALSE;
	}
	virtual ~LPSHOBJ_Wrapper() {}

	LPSHOBJ_Wrapper<T>& operator=(const LPSHOBJ_Wrapper<T>& rhs)
	{
		if (this != &rhs) {
			this->invalidatePtr();
			m_lpShObj = rhs.m_lpShObj;
			m_bOwner  = rhs.m_bOwner;
			rhs.m_bOwner = FALSE;
		}
		return *this;
	}

	LPSHOBJ_Wrapper<T>& operator=(T* prhs)
	{
		this->invalidatePtr();
		m_lpShObj = prhs;
		m_bOwner  = TRUE;
		return *this;
	}

	BOOL operator==(const T* prhs) const
	{
		return m_lpShObj == prhs;
	}
	BOOL operator!=(const T* prhs) const
	{
		return !operator==(prhs);
	}

	operator T*&() { return m_lpShObj; }
	operator const T*&() const { return (const T*&)m_lpShObj; }

	T* operator->() { return m_lpShObj; }
	const T* operator->() const { return m_lpShObj; }
	T& operator*() { return *m_lpShObj; }
	const T& operator*() const { return *m_lpShObj; }

protected:
	T*   m_lpShObj;
	mutable BOOL m_bOwner;

	virtual void invalidatePtr() = 0;
};

class LPIDL_Wrapper : public LPSHOBJ_Wrapper<ITEMIDLIST> {
public:
	LPIDL_Wrapper(LPITEMIDLIST lpidl = NULL)
		: LPSHOBJ_Wrapper<ITEMIDLIST>(lpidl), m_pEnumHeader(NULL)
	{}
	LPIDL_Wrapper(const LPIDL_Wrapper& rhs)
		: LPSHOBJ_Wrapper<ITEMIDLIST>(rhs), m_pEnumHeader(NULL)
	{}
	LPIDL_Wrapper& operator=(const LPIDL_Wrapper& rhs)
	{
		this->LPSHOBJ_Wrapper<ITEMIDLIST>::operator=(rhs);
		m_pEnumHeader = NULL;
		return *this;
	}
	LPIDL_Wrapper& operator=(LPITEMIDLIST lpidl)
	{
		this->LPSHOBJ_Wrapper<ITEMIDLIST>::operator=(lpidl);
		m_pEnumHeader = NULL;
		return *this;
	}
	~LPIDL_Wrapper()
	{
		this->invalidatePtr();
	}

	DWORD getListSize() const;

	void initSequentialGet() const { m_pEnumHeader = m_lpShObj; }
	BOOL hasMoreItems() const { return m_pEnumHeader != NULL; }
	LPIDL_Wrapper getNextItem() const;

	static LPIDL_Wrapper getSpecialFolderLPIDL(int where)
	{
		LPITEMIDLIST lpidl = NULL;
		::SHGetSpecialFolderLocation(NULL, where, &lpidl);
		return lpidl;
	}

protected:
	mutable LPITEMIDLIST m_pEnumHeader;

	void invalidatePtr()
	{
		if (m_bOwner) g_SHOBJ_Allocator.free(m_lpShObj);
	}

	static LPITEMIDLIST getNextListPtr(LPITEMIDLIST pidl)
	{
		return (LPITEMIDLIST)((BYTE*)pidl + pidl->mkid.cb);
	}
};

template<class T>
class LPSHOBJ_Wrapper_Released : public LPSHOBJ_Wrapper<T> {
public:
	LPSHOBJ_Wrapper_Released(T* lpShObj = NULL)
		: LPSHOBJ_Wrapper<T>(lpShObj)
	{}
	LPSHOBJ_Wrapper_Released(const LPSHOBJ_Wrapper_Released<T>& rhs)
		: LPSHOBJ_Wrapper<T>(rhs)
	{}
	LPSHOBJ_Wrapper_Released<T>&
		operator=(const LPSHOBJ_Wrapper_Released<T>& rhs)
	{
		this->LPSHOBJ_Wrapper<T>::operator=(rhs);
		return *this;
	}
	LPSHOBJ_Wrapper_Released<T>&
		operator=(T* prhs)
	{
		this->LPSHOBJ_Wrapper<T>::operator=(prhs);
		return *this;
	}
	~LPSHOBJ_Wrapper_Released()
	{
		this->invalidatePtr();
	}

protected:
	void invalidatePtr()
	{
		if (m_bOwner && m_lpShObj != NULL) {
			m_lpShObj->Release();
		}
	}
};

typedef LPSHOBJ_Wrapper_Released<IExtractIcon> LPEI_Wrapper;
typedef LPSHOBJ_Wrapper_Released<IEnumIDList>  LPEIDL_Wrapper;

class LPSF_Wrapper : public LPSHOBJ_Wrapper_Released<IShellFolder> {
public:
	LPSF_Wrapper(IShellFolder* lpsf = NULL)
		: LPSHOBJ_Wrapper_Released<IShellFolder>(lpsf),
		  m_LPEIDL_Wrapper(NULL)
	{
	}
	LPSF_Wrapper(const LPSF_Wrapper& rhs)
		: LPSHOBJ_Wrapper_Released<IShellFolder>(rhs),
		  m_LPEIDL_Wrapper(NULL)
	{}
	LPSF_Wrapper& operator=(const LPSF_Wrapper& rhs)
	{
		this->LPSHOBJ_Wrapper_Released<IShellFolder>::operator=(rhs);
		m_LPEIDL_Wrapper = NULL;
		return *this;
	}
	LPSF_Wrapper& operator=(LPSHELLFOLDER prhs)
	{
		this->LPSHOBJ_Wrapper_Released<IShellFolder>::operator=(prhs);
		m_LPEIDL_Wrapper = NULL;
		return *this;
	}

	static LPSF_Wrapper getDesktopFolder();

	LPEI_Wrapper getLPEI(const LPIDL_Wrapper& lpidlFolder) const
	{
		IExtractIcon* lpeIcon = NULL;
		m_lpShObj->GetUIObjectOf(NULL,
								 1,
								 &(LPCITEMIDLIST&)lpidlFolder,
								 IID_IExtractIcon,
								 NULL,
								 (void**)&lpeIcon);
		return lpeIcon;
	}

	void initSequentialGet(DWORD flag, HWND hwndOwner = NULL) const;

	LPIDL_Wrapper getNextSubFolderLPIDL() const
	{
		if (m_LPEIDL_Wrapper != NULL) {
			LPITEMIDLIST lpidl = NULL;
			DWORD num;
			if (m_LPEIDL_Wrapper->Next(1, &lpidl, &num) == NOERROR)
				return lpidl;
		}
		return NULL;
	}

	StringBuffer getDisplayNameOf(const LPIDL_Wrapper& lpidlFolder, DWORD flag) const;
	LPSF_Wrapper bindTo(const LPIDL_Wrapper& lpidl) const
	{
		LPSHELLFOLDER lpShellFolder = NULL;
		m_lpShObj->BindToObject(lpidl, NULL,
								IID_IShellFolder,
								(void**)&lpShellFolder);
		return lpShellFolder;
	}

	LPIDL_Wrapper parseDisplayName(const StringBuffer& name) const
	{
		WCHAR wname[MAX_PATH];
		::ZeroMemory(wname, sizeof(WCHAR) * MAX_PATH);
		name.toUnicode(wname);

		ULONG len;
		LPITEMIDLIST lpidl = NULL;
		m_lpShObj->ParseDisplayName(NULL, NULL, wname, &len, &lpidl, NULL);

		return lpidl;
	}

	LPIDL_Wrapper setNameOf(const StringBuffer& newname,
							const LPIDL_Wrapper& pidl,
							DWORD flag) const
	{
		WCHAR wname[MAX_PATH];
		::ZeroMemory(wname, sizeof(WCHAR) * MAX_PATH);
		newname.toUnicode(wname);
		LPITEMIDLIST pidlNew = NULL;
		m_lpShObj->SetNameOf(NULL, pidl, wname, flag, &pidlNew);
		return pidlNew;
	}

private:
	mutable LPEIDL_Wrapper m_LPEIDL_Wrapper;
};

#endif
