//	$Id: ss_handler.cpp,v 1.2 2003-02-15 18:37:02 sugiura Exp $
/*
 *	ss_handler.cpp
 *	文字列サービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"

StringBuffer
ConvData::On_atoi(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(this->si_atoi(params.getArgvStr(0)));
}

StringBuffer
ConvData::On_itoa(CmdLineParser& params)
{
	return this->si_itoa(ival(params.getArgv(0)),ival(params.getArgv(1)));
}

StringBuffer
ConvData::On_stricmp(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(
			this->si_stricmp(params.getArgvStr(0),
							params.getArgvStr(1)));
}

StringBuffer
ConvData::On_strcount(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(
			this->si_strcount(params.getArgvStr(0),
								params.getArgvStr(1)));
}

StringBuffer
ConvData::On_gsub(CmdLineParser& params)
{
	return this->si_gsub(params.getArgvStr(0),
						params.getArgvStr(1),
						params.getArgvStr(2),
						ival(params.getArgv(3)));
}

StringBuffer
ConvData::On_reverse(CmdLineParser& params)
{
	return this->si_reverse(params.getArgvStr(0));
}

StringBuffer
ConvData::On_tolower(CmdLineParser& params)
{
	return this->si_tolower(params.getArgvStr(0));
}

StringBuffer
ConvData::On_toupper(CmdLineParser& params)
{
	return this->si_toupper(params.getArgvStr(0));
}

StringBuffer
ConvData::On_strstr(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(
				this->si_strstr(params.getArgvStr(0),
								params.getArgvStr(1)));
}

StringBuffer
ConvData::On_strrstr(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(
				this->si_strrstr(params.getArgvStr(0),
								params.getArgvStr(1)));
}

StringBuffer
ConvData::On_strlen(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(
				this->si_strlen(params.getArgvStr(0)));
}

StringBuffer
ConvData::On_leftstr(CmdLineParser& params)
{
	return this->si_leftstr(params.getArgvStr(0),ival(params.getArgv(1)));
}

StringBuffer
ConvData::On_midstr(CmdLineParser& params)
{
	return this->si_midstr(params.getArgvStr(0),
							ival(params.getArgv(1)),
							ival(params.getArgv(2)));
}

StringBuffer
ConvData::On_rightstr(CmdLineParser& params)
{
	return this->si_rightstr(params.getArgvStr(0),ival(params.getArgv(1)));
}

StringBuffer
ConvData::On_reverse2(CmdLineParser& params)
{
	return this->si_reverse2(params.getArgvStr(0));
}

StringBuffer
ConvData::On_tolower2(CmdLineParser& params)
{
	return this->si_tolower2(params.getArgvStr(0));
}

StringBuffer
ConvData::On_toupper2(CmdLineParser& params)
{
	return this->si_toupper2(params.getArgvStr(0));
}

StringBuffer
ConvData::On_strstr2(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(
				this->si_strstr2(params.getArgvStr(0),
								params.getArgvStr(1)));
}

StringBuffer
ConvData::On_strrstr2(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(
				this->si_strrstr2(params.getArgvStr(0),
								params.getArgvStr(1)));
}

StringBuffer
ConvData::On_strlen2(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(
				this->si_strlen2(params.getArgvStr(0)));
}

StringBuffer
ConvData::On_leftstr2(CmdLineParser& params)
{
	return this->si_leftstr2(params.getArgvStr(0),ival(params.getArgv(1)));
}

StringBuffer
ConvData::On_midstr2(CmdLineParser& params)
{
	return this->si_midstr2(params.getArgvStr(0),
							ival(params.getArgv(1)),
							ival(params.getArgv(2)));
}

StringBuffer
ConvData::On_rightstr2(CmdLineParser& params)
{
	return this->si_rightstr2(params.getArgvStr(0),
							ival(params.getArgv(1)));
}

StringBuffer
ConvData::On_ltrim(CmdLineParser& params)
{
	return this->si_ltrim(params.getArgvStr(0));
}

StringBuffer
ConvData::On_rtrim(CmdLineParser& params)
{
	return this->si_rtrim(params.getArgvStr(0));
}

StringBuffer
ConvData::On_gettoken(CmdLineParser& params)
{
	return this->si_gettoken(params.getArgvStr(0),params.getArgvStr(1));
}

StringBuffer
ConvData::On_hasmoretokens(CmdLineParser& params)
{
	return StringBuffer(nullStr).append(this->si_hasmoretokens());
}

