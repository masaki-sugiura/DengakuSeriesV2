//	$Id: hs_handler.cpp,v 1.3 2009-09-20 13:49:01 sugiura Exp $
/*
 *	bs_handler.cpp
 *	BRegExp サービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"

int
ConvData::On_hmjre_load(CmdLineParser& params)
{
	return this->si_hmjre_load(params.getArgvStr(0));
}

int
ConvData::On_hmjre_free(CmdLineParser& params)
{
	return this->si_hmjre_free();
}

StringBuffer
ConvData::On_hmjre_get_version(CmdLineParser& params)
{
	return this->si_hmjre_get_version();
}

StringBuffer
ConvData::On_hmjre_match(CmdLineParser& params)
{
	return this->si_hmjre_match(params.getArgvStr(0),
								params.getArgvStr(1),
								ival(params.getArgvStr(2)),
								ival(params.getArgvStr(3)),
								ival(params.getArgvStr(4)));
}

StringBuffer
ConvData::On_hmjre_get_tag_position(CmdLineParser& params)
{
	return this->si_hmjre_get_tag_position(ival(params.getArgvStr(0)));
}

StringBuffer
ConvData::On_hmjre_get_match_string(CmdLineParser& params)
{
	return this->si_hmjre_get_match_string(params.getArgvStr(0),
										   params.getArgvStr(1));
}

int
ConvData::On_hmjre_find_regular(CmdLineParser& params)
{
	return this->si_hmjre_find_regular(params.getArgvStr(0), params.getArgvStr(1), ival(params.getArgvStr(2)));
}

int
ConvData::On_hmjre_find_regular_no_case_sense(CmdLineParser& params)
{
	return this->si_hmjre_find_regular_no_case_sense(params.getArgvStr(0), params.getArgvStr(1), ival(params.getArgvStr(2)));
}

StringBuffer
ConvData::On_hmjre_replace_regular(CmdLineParser& params)
{
	return this->si_hmjre_replace_regular(params.getArgvStr(0), params.getArgvStr(1), ival(params.getArgvStr(2)), params.getArgvStr(3), ival(params.getArgvStr(4)));
}

StringBuffer
ConvData::On_hmjre_replace_regular_no_case_sense(CmdLineParser& params)
{
	return this->si_hmjre_replace_regular_no_case_sense(params.getArgvStr(0), params.getArgvStr(1), ival(params.getArgvStr(2)), params.getArgvStr(3), ival(params.getArgvStr(4)));
}

int
ConvData::On_hmjre_get_last_match_length(CmdLineParser&)
{
	return this->si_hmjre_get_last_match_length();
}

int
ConvData::On_hmjre_get_last_match_tag_position(CmdLineParser& params)
{
	return this->si_hmjre_get_last_match_tag_position(ival(params.getArgvStr(0)));
}

int
ConvData::On_hmjre_get_last_match_tag_length(CmdLineParser& params)
{
	return this->si_hmjre_get_last_match_tag_length(ival(params.getArgvStr(0)));
}

int
ConvData::On_hmjre_env_changed(CmdLineParser&)
{
	return this->si_hmjre_env_changed();
}

int
ConvData::On_hmjre_option_dialog(CmdLineParser& params)
{
	return this->si_hmjre_option_dialog(ival(params.getArgvStr(0)), ival(params.getArgvStr(1)));
}
