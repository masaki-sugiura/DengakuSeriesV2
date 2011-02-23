//	$Id: es_handler.cpp,v 1.1.1.1 2001/10/07 14:41:22 sugiura Exp $
/*
 *	es_handler.cpp
 *	環境変数サービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"

int
ConvData::On_setvar(CmdLineParser& params)
{
	return this->si_setvar(params.getArgvStr(0),params.getArgvStr(1));
}

StringBuffer
ConvData::On_getvar(CmdLineParser& params)
{
	return this->si_getvar(params.getArgvStr(0));
}

int
ConvData::On_delvar(CmdLineParser& params)
{
	return this->si_delvar(params.getArgvStr(0));
}

StringBuffer
ConvData::On_existvar(CmdLineParser& params)
{
	return this->si_existvar(params.getArgvStr(0)) != 0 ? existStr : nullStr;
}

int
ConvData::On_enumvar(CmdLineParser&)
{
	return this->si_enumvar();
}

StringBuffer
ConvData::On_nextvar(CmdLineParser& params)
{
	return this->si_nextvar();
}

StringBuffer
ConvData::On_hasmorevars(CmdLineParser&)
{
	return this->si_hasmorevars() ? existStr : nullStr;
}

