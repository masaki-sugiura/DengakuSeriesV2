//	$Id: crs_handler.cpp,v 1.1 2002-02-15 17:46:08 sugiura Exp $
/*
 *	crs_handler.cpp
 *	カラーテーブルサービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"

int
ConvData::On_clr_setname(CmdLineParser& params)
{
	if (params.itemNum() < 2) return FALSE;
	return this->si_setcolorref(params.getArgvStr(0), params.getArgvStr(1));
}

StringBuffer
ConvData::On_clr_getname(CmdLineParser& params)
{
	if (params.itemNum() < 1) return nullStr;
	return this->si_getcolorname(params.getArgvStr(0));
}

StringBuffer
ConvData::On_clr_getrgb(CmdLineParser& params)
{
	if (params.itemNum() < 1) return nullStr;
	return this->si_getcolorref(params.getArgvStr(0));
}

int
ConvData::On_clr_load(CmdLineParser& params)
{
	if (params.itemNum() < 1) return FALSE;
	return this->si_loadcolorrefs(params.getArgvStr(0));
}

int
ConvData::On_clr_save(CmdLineParser& params)
{
	if (params.itemNum() < 1) return FALSE;
	return this->si_savecolorrefs(params.getArgvStr(0));
}

