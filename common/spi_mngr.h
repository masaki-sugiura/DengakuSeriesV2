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

class SPI_Manager {
public:
	SPI_Manager(const StringBuffer&);
	~SPI_Manager();

	StringBuffer getSPIInfo(int) const;
	BOOL isSupportedPic(const StringBuffer&) const;
	StringBuffer getPicInfo(const StringBuffer&) const;

private:
	HMODULE m_hSPI;
	PFNSPIINFO m_pfnSPIInfo;
	PFNSPISUPPORTED m_pfnSPISupported;
	PFNSPIGETPICINFO m_pfnSPIGetPicInfo;
};

class SPINotFoundException {};

#endif
