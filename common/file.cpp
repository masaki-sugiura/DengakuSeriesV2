//	$Id: file.cpp,v 1.2 2002-12-15 12:09:49 sugiura Exp $
/*
 *	file.cpp
 *	ƒtƒ@ƒCƒ‹‚Ì“Ç‚Ý‘‚«•‘®«‘€ì
 */

#include "file.h"

File::File(LPCSTR filename, DWORD accessmode, DWORD openmode, DWORD flag)
{
	m_hFile = ::CreateFile(filename, accessmode, 0, NULL, openmode, flag, NULL);
}

File::~File()
{
	if (this->isValid()) ::CloseHandle(m_hFile);
}

LONGLONG
File::setFilePointer(LONGLONG size, DWORD seekfrom) const
{
	if (!this->isValid()) return -1;
	LONG lSizeHigh = (LONG)((size >> 32) & 0xFFFFFFFF);
	DWORD dwSizeLow = ::SetFilePointer(m_hFile, (LONG)size, &lSizeHigh, seekfrom);
	if (dwSizeLow == 0xFFFFFFFF && ::GetLastError() != NOERROR)
		return -1;
	return dwSizeLow | ((LONGLONG)lSizeHigh << 32);
}

DWORD
File::readFile(BYTE *pbuf, DWORD size) const
{
	if (pbuf == NULL || !this->isValid()) return 0;
	DWORD dwReadByte;
	if (!::ReadFile(m_hFile, pbuf, size, &dwReadByte, NULL)) return 0;
	return dwReadByte;
}

DWORD
File::writeFile(const BYTE *pbuf, DWORD size) const
{
	if (pbuf == NULL || !this->isValid()) return 0;
	DWORD dwWriteByte;
	if (!::WriteFile(m_hFile, pbuf, size, &dwWriteByte, NULL)) return 0;
	return dwWriteByte;
}

BOOL
File::setFileTime(
	const FILETIME *ftCreate,
	const FILETIME *ftLastAccess,
	const FILETIME *ftLastWrite) const
{
	if (!this->isValid()) return FALSE;
	return ::SetFileTime(m_hFile, ftCreate, ftLastAccess, ftLastWrite);
}

BOOL
File::getFileTime(
	FILETIME *ftCreate,
	FILETIME *ftLastAccess,
	FILETIME *ftLastWrite) const
{
	if (!this->isValid()) return FALSE;
	return ::GetFileTime(m_hFile, ftCreate, ftLastAccess, ftLastWrite);
}

