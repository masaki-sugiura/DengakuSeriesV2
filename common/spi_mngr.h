//	DengakuServer v1: Copyright(C) 1999-2000, M. Sugiura
/*
 *	spi_mngr.h
 *	Susie Plugin をコントロールするクラス
 */

#ifndef DENGAKUSERIES_CLASSES_SPI_MANAGER
#define DENGAKUSERIES_CLASSES_SPI_MANAGER

#include "strbuf.h"

#pragma pack(push,pic_info,8)
typedef struct {
	LONG	left, top;
	LONG	width;
	LONG	height;
	WORD	x_density;
	WORD	y_density;
	SHORT	colorDepth;
	HLOCAL	hInfo;
} PIC_INFO;
#pragma pack(pop,pic_info)

typedef int (__stdcall *PFNSPIINFO)(int,LPSTR,int);
typedef int (__stdcall *PFNSPISUPPORTED)(LPCSTR,DWORD);
typedef int (__stdcall *PFNSPIGETPICINFO)(LPCSTR,LONG,UINT,PIC_INFO*);

typedef struct {
	HMODULE m_hSPI;
	PFNSPIINFO m_pfnSPIInfo;
	PFNSPISUPPORTED m_pfnSPISupported;
	PFNSPIGETPICINFO m_pfnSPIGetPicInfo;
} SPI_PFNS;

class SPI_Manager {
public:
	SPI_Manager() { m_spi_pfns.m_hSPI = NULL; }
	~SPI_Manager()
	{ if (m_spi_pfns.m_hSPI != NULL) ::FreeLibrary(m_spi_pfns.m_hSPI); }

	BOOL isLoaded() const { return m_spi_pfns.m_hSPI != NULL; }

	BOOL loadSPI(const StringBuffer&);
	void freeSPI();

	StringBuffer getSPIInfo(int) const;
	BOOL isSupportedPic(const StringBuffer&) const;
	StringBuffer getPicInfo(const StringBuffer&) const;

private:
	SPI_PFNS m_spi_pfns;
};

#endif
