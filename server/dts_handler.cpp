//	$Id: dts_handler.cpp,v 1.1 2002/08/05 16:06:17 sugiura Exp $
/*
 *	dts_handler.cpp
 *	日付･時刻サービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"

StringBuffer
ConvData::On_dt_format(CmdLineParser& argv)
{
	if (argv.itemNum() < 2) return nullStr;
	const StringBuffer& fmt = argv.getArgvStr(0);
	int time = ival(argv.getArgv(1));
	return this->si_datetime_format(fmt, time);
}

StringBuffer
ConvData::On_dt_gettime(CmdLineParser&)
{
	return StringBuffer().append(this->si_datetime_gettime());
}

StringBuffer
ConvData::On_dt_filetime_to_time(CmdLineParser& argv)
{
	int time;
	if (argv.itemNum() < 1)
		time = this->si_datetime_gettime();
	else
		time = this->si_datetime_filetime(ival(argv.getArgv(0)));
	return StringBuffer().append(time);
}

