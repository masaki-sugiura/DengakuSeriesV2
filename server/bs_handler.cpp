//	$Id: bs_handler.cpp,v 1.1 2002-01-16 15:57:23 sugiura Exp $
/*
 *	bs_handler.cpp
 *	BRegExp サービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"

int
ConvData::On_bre_load(CmdLineParser& params)
{
	return this->si_bregexp_load(params.getArgvStr(0));
}

int
ConvData::On_bre_free(CmdLineParser& params)
{
	return this->si_bregexp_free();
}

StringBuffer
ConvData::On_bre_errormsg(CmdLineParser& params)
{
	return this->si_bregexp_errormsg();
}

StringBuffer
ConvData::On_bre_version(CmdLineParser& params)
{
	return this->si_bregexp_version();
}

StringBuffer
ConvData::On_bre_match(CmdLineParser& params)
{
	return this->si_bregexp_match(params.getArgvStr(0), params.getArgvStr(1));
}

StringBuffer
ConvData::On_bre_subst(CmdLineParser& params)
{
	return this->si_bregexp_subst(params.getArgvStr(0), params.getArgvStr(1));
}

StringBuffer
ConvData::On_bre_trans(CmdLineParser& params)
{
	return this->si_bregexp_trans(params.getArgvStr(0), params.getArgvStr(1));
}

StringBuffer
ConvData::On_bre_split(CmdLineParser& params)
{
	return StringBuffer(8).append(this->si_bregexp_split(
										params.getArgvStr(0),
										params.getArgvStr(1),
										ival(params.getArgv(2))));
}

StringBuffer
ConvData::On_bre_getnext(CmdLineParser& params)
{
	return this->si_bregexp_getnext();
}

StringBuffer
ConvData::On_bre_hasmoreresults(CmdLineParser& params)
{
	return StringBuffer(8).append(this->si_bregexp_hasmoreresults());
}

