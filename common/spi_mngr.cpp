//	DengakuServer v1: Copyright(C) 1999-2000, M. Sugiura
/*
 *	spi_mngr.cpp
 *	Susie Plugin をコントロールするクラスの実装
 */

#include "spi_mngr.h"
#include "pathname.h"
#include "file.h"

BOOL
SPI_Manager::loadSPI(const StringBuffer& spiname)
{
	if (m_spi_pfns.m_hSPI != NULL ||
		(m_spi_pfns.m_hSPI = ::LoadLibrary(spiname)) == NULL) return FALSE;
	if ((m_spi_pfns.m_pfnSPIInfo
			= (PFNSPIINFO)::GetProcAddress(m_spi_pfns.m_hSPI,"GetPluginInfo"))
			== NULL ||
		(m_spi_pfns.m_pfnSPISupported
			= (PFNSPISUPPORTED)::GetProcAddress(m_spi_pfns.m_hSPI,
												"IsSupported")) == NULL ||
		(m_spi_pfns.m_pfnSPIGetPicInfo
			= (PFNSPIGETPICINFO)::GetProcAddress(m_spi_pfns.m_hSPI,
												"GetPictureInfo")) == NULL) {
		this->freeSPI();
		return FALSE;
	}
	return TRUE;
}

void
SPI_Manager::freeSPI()
{
	if (m_spi_pfns.m_hSPI != NULL) {
		::FreeLibrary(m_spi_pfns.m_hSPI);
		m_spi_pfns.m_hSPI = NULL;
	}
}

StringBuffer
SPI_Manager::getSPIInfo(int type) const
{
	TCHAR buf[MAX_PATH];
	buf[0] = '\0';
	if (m_spi_pfns.m_hSPI != NULL && m_spi_pfns.m_pfnSPIInfo != NULL) {
		(*m_spi_pfns.m_pfnSPIInfo)(type,buf,MAX_PATH);
	}
	return buf;
}

BOOL
SPI_Manager::isSupportedPic(const StringBuffer& filename) const
{
	if (m_spi_pfns.m_hSPI == NULL ||
		m_spi_pfns.m_pfnSPISupported == NULL) return FALSE;
	File fPicture(
				(LPCSTR)PathName(filename),
				GENERIC_READ,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN
			);
	if (!fPicture.isValid()) return FALSE;
	BOOL ret = FALSE;
	BYTE buf[2048];
	DWORD fsize = fPicture.readFile(buf,2048);
	if (fsize > 0) {
		if (fsize < 2048) ::ZeroMemory(buf + fsize,2048-fsize);
		ret = (*m_spi_pfns.m_pfnSPISupported)((LPCSTR)filename,(DWORD)buf);
	}
	return ret;
}

StringBuffer
SPI_Manager::getPicInfo(const StringBuffer& filename) const
{
	if (m_spi_pfns.m_hSPI == NULL ||
		m_spi_pfns.m_pfnSPIGetPicInfo == NULL) return nullStr;
	PIC_INFO pi;
	pi.hInfo = NULL;
	if ((*m_spi_pfns.m_pfnSPIGetPicInfo)(filename,0,0,&pi) != 0 &&
		(*m_spi_pfns.m_pfnSPIGetPicInfo)(filename,128,0,&pi) != 0)
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

