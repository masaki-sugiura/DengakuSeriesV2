//	$Id: pathname.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	pathname.h
 *	パス名を扱うクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_PATHNAME
#define	DENGAKUSERIES_CLASSES_PATHNAME

#include "strbuf.h"

class PathName : public StringBuffer {
public:
	PathName(const StringBuffer& name = nullStr);

	void refresh();
	void reset(const StringBuffer& name = nullStr, int head = 0, int num = -1);

	PathName& addPath(const StringBuffer&);
	PathName& delPath(int num = 1);

	void setBaseName(const StringBuffer&);
	LPCSTR getBaseName() const
	{
		LPCSTR s = lstrrchr(*this,'\\');
		return s != NULL ? (s + 1) : NULL;
	}

	DWORD getAttributes() const
	{
//		this->getFileType();
		return m_fd.dwFileAttributes;
	}

	BOOL isValid() const
	{
		return (m_fd.dwFileAttributes != 0xFFFFFFFF);
	}

	const FILETIME* getTime() const
	{
		if (!this->isValid()) return NULL;
		return &m_fd.ftLastWriteTime;
	}

	DWORD getSize() const
	{
		if (!this->isValid()) return 0L;
		return m_fd.nFileSizeLow;
	}

	BOOL isReadOnly() const
	{
		if (!this->isValid()) return FALSE;
		return (m_fd.dwFileAttributes&FILE_ATTRIBUTE_READONLY) != 0;
	}

	BOOL isDirectory() const
	{
		if (!this->isValid()) return FALSE;
		return (m_fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

protected:
	mutable WIN32_FIND_DATA	m_fd;
	mutable int m_node;

	void getFileType() const;
};

extern const StringBuffer anyPathName; // "*.*"

inline BOOL
IsPathNameDots(LPCSTR name)
{
	if (name[0] != '.') return FALSE;
	return name[1] == '\0' || (name[1] == '.' && name[2] == '\0');
}

#endif

