//	$Id: file.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	file.h
 *	ƒtƒ@ƒCƒ‹‚Ì“Ç‚İ‘‚«•‘®«‘€ì
 */

#ifndef	DENGAKUSERIES_CLASSES_FILE
#define	DENGAKUSERIES_CLASSES_FILE

#include <windows.h>

class File {
public:
	File(LPCSTR filename, DWORD accessmode, DWORD openmode, DWORD flag);
	~File();

	BOOL isValid() const { return m_hFile != INVALID_HANDLE_VALUE; }

	DWORD setFilePointer(LONG, PLONG, DWORD) const;
	DWORD readFile(BYTE*, DWORD) const;
	DWORD writeFile(const BYTE*, DWORD) const;
	void flushFileBuffers() const
	{
		if (this->isValid()) ::FlushFileBuffers(m_hFile);
	}

	BOOL setFileTime(const FILETIME*, const FILETIME*, const FILETIME*) const;
	BOOL getFileTime(FILETIME*,FILETIME*,FILETIME*) const;

private:
	HANDLE m_hFile;

	File(const File&);
	File& operator=(const File&);
};

#endif
