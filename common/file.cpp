//	$Id: file.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	file.cpp
 *	ƒtƒ@ƒCƒ‹‚Ì“Ç‚Ý‘‚«•‘®«‘€ì
 */

#include "file.h"

File::File(LPCSTR filename, DWORD accessmode, DWORD openmode, DWORD flag)
{
	m_hFile = ::CreateFile(filename,accessmode,0,NULL,openmode,flag,NULL);
}

File::~File()
{
	if (this->isValid()) ::CloseHandle(m_hFile);
}

DWORD
File::setFilePointer(LONG sizelow, PLONG lpsizehigh, DWORD seekfrom) const
{
	if (!this->isValid()) return 0xFFFFFFFF;
	return ::SetFilePointer(m_hFile,sizelow,lpsizehigh,seekfrom);
}

DWORD
File::readFile(BYTE *pbuf, DWORD size) const
{
	if (pbuf == NULL || !this->isValid()) return 0;
	DWORD dwReadByte;
	if (!::ReadFile(m_hFile,pbuf,size,&dwReadByte,NULL)) return 0;
	return dwReadByte;
}

DWORD
File::writeFile(const BYTE *pbuf, DWORD size) const
{
	if (pbuf == NULL || !this->isValid()) return 0;
	DWORD dwWriteByte;
	if (!::WriteFile(m_hFile,pbuf,size,&dwWriteByte,NULL)) return 0;
	return dwWriteByte;
}

BOOL
File::setFileTime(
	const FILETIME *ftCreate,
	const FILETIME *ftLastAccess,
	const FILETIME *ftLastWrite) const
{
	if (!this->isValid()) return FALSE;
	return ::SetFileTime(m_hFile,ftCreate,ftLastAccess,ftLastWrite);
}

BOOL
File::getFileTime(
	FILETIME *ftCreate,
	FILETIME *ftLastAccess,
	FILETIME *ftLastWrite) const
{
	if (!this->isValid()) return FALSE;
	return ::GetFileTime(m_hFile,ftCreate,ftLastAccess,ftLastWrite);
}

