//	$Id: is_handler.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	is_handler.cpp
 *	Susie Plugin サービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"

int
ConvData::On_loadspi(CmdLineParser& params)
{
	return this->si_loadspi(params.getArgvStr(0));
}

int
ConvData::On_freespi(CmdLineParser&)
{
	return this->si_freespi();
}

StringBuffer
ConvData::On_getspiinfo(CmdLineParser& params)
{
	return this->si_getspiinfo(ival(params.getArgvStr(0)));
}

StringBuffer
ConvData::On_issupportedpic(CmdLineParser& params)
{
	return this->si_issupportedpic(params.getArgvStr(0)) ? existStr : nullStr;
}

StringBuffer
ConvData::On_getpicinfo(CmdLineParser& params)
{
	return this->si_getpicinfo(params.getArgvStr(0));
}

