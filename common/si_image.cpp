//	$Id: si_image.cpp,v 1.3 2002-02-15 17:46:08 sugiura Exp $
/*
 *	si_image.cpp
 *	‰æ‘œ‚ÌŠÇ—‚ÉŠÖ‚·‚éƒNƒ‰ƒX
 */

#include "session.h"
#include "cmdline.h"
#include "pathname.h"
#include "spi_mngr.h"

int
SessionInstance::si_loadspi(const StringBuffer& filename)
{
	PathName pnspi;
	if (!m_DirList.getPathName(filename,pnspi,TRUE)) return FALSE;
	try {
		m_pSPIManager = new SPI_Manager(pnspi);
	} catch (...) {
		return FALSE;
	}
	return TRUE;
}

int
SessionInstance::si_freespi()
{
	if (m_pSPIManager.ptr() == NULL) return FALSE;
	m_pSPIManager = NULL;
	return TRUE;
}

StringBuffer
SessionInstance::si_getspiinfo(int id)
{
	if (m_pSPIManager.ptr() == NULL) return nullStr;
	return m_pSPIManager->getSPIInfo(id);
}

int
SessionInstance::si_issupportedpic(const StringBuffer& filename)
{
	if (m_pSPIManager.ptr() == NULL) return FALSE;
	PathName pnpic;
	if (!m_DirList.getPathName(filename,pnpic,TRUE) ||
		!m_pSPIManager->isSupportedPic(pnpic)) return FALSE;
	return TRUE;
}

StringBuffer
SessionInstance::si_getpicinfo(const StringBuffer& filename)
{
	if (m_pSPIManager.ptr() == NULL) return nullStr;
	PathName pnpic;
	if (!m_DirList.getPathName(filename,pnpic,TRUE)) return nullStr;
	return m_pSPIManager->getPicInfo(pnpic);
}

