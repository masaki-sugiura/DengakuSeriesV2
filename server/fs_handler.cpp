//	$Id: fs_handler.cpp,v 1.2 2002-02-17 08:00:41 sugiura Exp $
/*
 *	fs_handler.cpp
 *	ファイルサービスの実装
 */

#include <exception>
#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"
#include "pathname.h"

int
ConvData::On_setcurdir(CmdLineParser& params)
{
	//	注意：
	//	setcurdir コマンドはサーバが内部的に持っている
	//	各ドライブ毎のカレントフォルダのリスト(DirList)の内容を更新するだけで
	//	実際に SetCurrentDir() API を発行していない。
	return this->si_setcurdir(params.getArgvStr(0));
}

int
ConvData::On_copy(CmdLineParser& params)
{
	if (DdeServer::isStrictEmulation()) {
		//	マクロサーバ互換モード
		//	copy orgfilename destfilename の形を仮定する。
		if (params.itemNum() != 2) return FALSE;
		PathName file, dest;
		if (!this->getPathName(params.getArgvStr(1),file,TRUE) ||
			!this->getPathName(params.getArgvStr(2),dest,FALSE))
			return FALSE;
		return ::CopyFile(file,dest,FALSE);
	}

	return this->si_copy(params);
}

int
ConvData::On_appendcopy(CmdLineParser& params)
{
	return this->si_appendcopy(params.getArgvStr(0),params.getArgvStr(1),
								FALSE);
}

int
ConvData::On_appendcopytext(CmdLineParser& params)
{
	return this->si_appendcopy(params.getArgvStr(0),params.getArgvStr(1),
								TRUE);
}

int
ConvData::On_rename(CmdLineParser& params)
{
	return this->si_rename(params.getArgvStr(0),params.getArgvStr(1));
}

int
ConvData::On_delete(CmdLineParser& params)
{
	return this->si_delete(params.getArgvStr(0));
}

int
ConvData::On_remove(CmdLineParser& params)
{
	return this->si_remove(params);
}

int
ConvData::On_move(CmdLineParser& params)
{
	return this->si_move(params);
}

int
ConvData::On_mkdir(CmdLineParser& params)
{
	return this->si_mkdir(params);
}

int
ConvData::On_rmdir(CmdLineParser& params)
{
	return this->si_rmdir(params);
}

int
ConvData::On_setattribute(CmdLineParser& params)
{
	return this->si_setattribute(params);
}

int
ConvData::On_touch(CmdLineParser& params)
{
	return this->si_touch(params);
}

StringBuffer
ConvData::On_fileopresult(CmdLineParser&)
{
	return this->si_fileopresult();
}

int
ConvData::On_run(CmdLineParser& params)
{
	return this->si_run(params, SW_SHOW);
}

int
ConvData::On_iconrun(CmdLineParser& params)
{
	return this->si_run(params, SW_SHOWMINNOACTIVE);
}

int
ConvData::On_maxrun(CmdLineParser& params)
{
	return this->si_run(params, SW_SHOWMAXIMIZED);
}

int
ConvData::On_hiderun(CmdLineParser& params)
{
	return this->si_run(params, SW_HIDE);
}

int
ConvData::On_enumpath(CmdLineParser& params)
{
	return this->si_enumpath(params);
}

int
ConvData::On_enumfile(CmdLineParser& params)
{
	return this->si_enumfile(params);
}

int
ConvData::On_enumdir(CmdLineParser& params)
{
	return this->si_enumdir(params);
}

StringBuffer
ConvData::On_getcurdir(CmdLineParser&)
{
	return this->si_getcurdir();
}

StringBuffer
ConvData::On_isreadonly(CmdLineParser& params)
{
	return this->si_isreadonly(params);
}

StringBuffer
ConvData::On_sizeof(CmdLineParser& params)
{
	return this->si_sizeof(params);
}

StringBuffer
ConvData::On_timestampof(CmdLineParser& params)
{
	return this->si_timestampof(params);
}

StringBuffer
ConvData::On_timecountof(CmdLineParser& params)
{
	return this->si_timecountof(params);
}

StringBuffer
ConvData::On_attributeof(CmdLineParser& params)
{
	return this->si_attributeof(params);
}

StringBuffer
ConvData::On_existfile(CmdLineParser& params)
{
	StringBuffer ret = this->si_getlongname(params.getArgvStr(0),TRUE);
	if (ret.length() <= 0) return nullStr;
	if (DdeServer::isStrictEmulation()) return existStr;
	return ret;
}

StringBuffer
ConvData::On_findnext(CmdLineParser&)
{
	return this->si_findnext();
}

StringBuffer
ConvData::On_getdrives(CmdLineParser&)
{
	return this->si_getdrives();
}

StringBuffer
ConvData::On_getlongname(CmdLineParser& params)
{
	return this->si_getlongname(params.getArgvStr(0),FALSE);
}

