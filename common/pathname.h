//	$Id: pathname.h,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	pathname.h
 *	パス名を扱うクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_PATHNAME
#define	DENGAKUSERIES_CLASSES_PATHNAME

#include "strutils.h"
#include "strbuf.h"

#define	ISPATHINVALID			0x80000000	/* 無効なパス名 */

#define	ISPATHSPECIFIED			0x00000001	/* ...dir1\dir2\foo */
#define	ISPATHFROMROOT			0x00000002	/* [header]\dir1\dir2\foo */
#define	ISDRIVESPECIFIED		0x00000010	/* X:... */
#define	ISSERVERSPECIFIED		0x00000020	/* \\computer... */
#define	ISSHARENAMESPECIFIED	0x00000040	/* \\computer\share... */

// #define	ISPATHLOCAL				0x00000100	/* X:\.. 型のパス */
#define	ISPATHREMOTE			0x00000200	/* \\computer\share\.. 型のパス */


inline int
DRIVENUM(TCHAR ch)
{
	return IsCharLowerCase(ch) ? (ch - 'a' + 1) : (ch - 'A' + 1);
}

inline BOOL
ISDRIVELETTER(TCHAR ch)
{
	return IsCharAlphabet(ch);
}


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

	//	与えられたパスの種類を返す
	static int getPathType(LPCSTR, LPCSTR* pphead = NULL);

protected:
	mutable WIN32_FIND_DATA	m_fd;
	mutable int m_node;
	mutable int m_type;

	BOOL isRootPath() const;

	void getFileType() const;
};

extern const StringBuffer anyPathName; // "*.*"

inline int
IsPathNameDots(LPCSTR name)
{
	if (name[0] == '.') {
		if (name[1] == '\0') return 1;
		if (name[1] == '.' && name[2] == '\0') return 2;
	}
	return 0;
}

inline BOOL
IsPathEndWithSep(const StringBuffer& path)
{
	return path.charAt(-1) == '\\' &&
		   (path.length() <= 1 || !IsCharLeadByte(path.charAt(-2)));
}

#endif

