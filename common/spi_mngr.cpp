//	DengakuServer v1: Copyright(C) 1999-2000, M. Sugiura
/*
 *	spi_mngr.cpp
 *	Susie Plugin をコントロールするクラスの実装
 */

#include "spi_mngr.h"
#include "pathname.h"
#include "file.h"

SPI_Manager::SPI_Manager(const StringBuffer& spiname)
{
	if ((m_hSPI = ::LoadLibrary(spiname)) == NULL ||
		(m_pfnSPIInfo = (PFNSPIINFO)::GetProcAddress(m_hSPI, "GetPluginInfo")) == NULL ||
		(m_pfnSPISupported = (PFNSPISUPPORTED)::GetProcAddress(m_hSPI, "IsSupported")) == NULL ||
		(m_pfnSPIGetPicInfo = (PFNSPIGETPICINFO)::GetProcAddress(m_hSPI, "GetPictureInfo")) == NULL)
		throw SPINotFoundException();
}

SPI_Manager::~SPI_Manager()
{
	// DLL アンロードの順序が不定な場合への対処
	if (!::IsBadCodePtr((FARPROC)m_pfnSPIInfo))
		::FreeLibrary(m_hSPI);
}

StringBuffer
SPI_Manager::getSPIInfo(int type) const
{
	TCHAR buf[MAX_PATH];
	buf[0] = '\0';
	(*m_pfnSPIInfo)(type, buf, MAX_PATH);
	return buf;
}

BOOL
SPI_Manager::isSupportedPic(const StringBuffer& filename) const
{
	File fPicture(
				(LPCSTR)PathName(filename),
				GENERIC_READ,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN
			);
	if (!fPicture.isValid()) return FALSE;

#define SPISUPPORTED_BUF_SIZE 2048

	BOOL ret = FALSE;
	Array<BYTE> buf(SPISUPPORTED_BUF_SIZE);
	DWORD fsize = fPicture.readFile(buf, SPISUPPORTED_BUF_SIZE);
	if (fsize > 0) {
		if (fsize < SPISUPPORTED_BUF_SIZE) buf.zero(fsize, -1);
		ret = (*m_pfnSPISupported)((LPCSTR)filename, (DWORD)(BYTE*)buf);
	}
	return ret;
}

StringBuffer
SPI_Manager::getPicInfo(const StringBuffer& filename) const
{
	PIC_INFO pi;
	pi.hInfo = NULL;
	if ((*m_pfnSPIGetPicInfo)(filename,0,0,&pi) != 0 &&
		(*m_pfnSPIGetPicInfo)(filename,128,0,&pi) != 0)
		return nullStr;
	StringBuffer ret(80);
	ret.append((DWORD)pi.left).append(',')
		.append((DWORD)pi.top).append(',')
		.append((DWORD)pi.width).append(',')
		.append((DWORD)pi.height).append(',')
		.append((WORD)pi.x_density).append(',')
		.append((WORD)pi.y_density).append(',')
		.append((WORD)pi.colorDepth);
	if (pi.hInfo != NULL) {
		ret.append(',').append((LPCSTR)::GlobalLock(pi.hInfo));
		::GlobalUnlock(pi.hInfo);
		::GlobalFree(pi.hInfo);
	}
	return ret;
}

