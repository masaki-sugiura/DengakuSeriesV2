//	$Id: si_image.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	si_image.cpp
 *	âÊëúÇÃä«óùÇ…ä÷Ç∑ÇÈÉNÉâÉX
 */

#include "session.h"
#include "cmdline.h"
#include "spi_mngr.h"

int
SessionInstance::si_loadspi(const StringBuffer& filename)
{
	PathName pnspi;
	if (!m_DirList.getPathName(filename,pnspi,TRUE)) return FALSE;
	return m_SPIManager.loadSPI(pnspi);
}

int
SessionInstance::si_freespi()
{
	if (!m_SPIManager.isLoaded()) return FALSE;
	m_SPIManager.freeSPI();
	return TRUE;
}

StringBuffer
SessionInstance::si_getspiinfo(int id)
{
	if (!m_SPIManager.isLoaded()) return nullStr;
	return m_SPIManager.getSPIInfo(id);
}

int
SessionInstance::si_issupportedpic(const StringBuffer& filename)
{
	if (!m_SPIManager.isLoaded()) return FALSE;
	PathName pnpic;
	if (!m_DirList.getPathName(filename,pnpic,TRUE) ||
		!m_SPIManager.isSupportedPic(pnpic)) return FALSE;
	return TRUE;
}

StringBuffer
SessionInstance::si_getpicinfo(const StringBuffer& filename)
{
	if (!m_SPIManager.isLoaded()) return nullStr;
	PathName pnpic;
	if (!m_DirList.getPathName(filename,pnpic,TRUE)) return nullStr;
	return m_SPIManager.getPicInfo(pnpic);
}

