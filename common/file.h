//	$Id: file.h,v 1.2 2002-12-15 12:09:49 sugiura Exp $
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

	LONGLONG setFilePointer(LONGLONG, DWORD) const;
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
