//	$Id: opencomdlg.cpp,v 1.2 2002-02-15 17:46:08 sugiura Exp $
/*
 *	opencomdlg.cpp
 *	コモンダイアログを表示する関数の実装
 */

#include "opencomdlg.h"
#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "seldir.h"

/*
	「ファイルを開く」ダイアログを表示する
*/
void
GetFileNameByDlg(HWND hwndOwner, ConvData& cd, CmdLineParser& argv)
{
	StringBuffer title = argv.getArgvStr(0);
	argv.delArgv(0);
	StringBuffer inipath = argv.getArgvStr(0);
	argv.delArgv(0);
	StringBuffer ret = cd.getFileNameByDlg(hwndOwner,title,inipath,argv);
	if (ret.length() <= 0) ret = errorStr;
	cd.setNotify(ret);
}

/*
	「フォルダの指定」ダイアログを表示する
*/
void
GetDirNameByDlg(HWND hwndOwner, ConvData& cd, CmdLineParser& argv)
{
	int flag = ival(argv.getArgv(2)) == 0 ? DBPA_DISABLEMODIFY : 0;
	StringBuffer ret = cd.getDirNameByDlg(hwndOwner,
									argv.getArgvStr(0),
									argv.getArgvStr(1),
									flag);
	if (ret.length() <= 0) ret = errorStr;
	cd.setNotify(ret);
}

/*
	「色の指定」ダイアログを表示する
*/
void
GetColorByDlg(HWND hwndOwner, ConvData& cd, CmdLineParser& argv)
{
	StringBuffer title = argv.getArgvStr(0);
	argv.delArgv(0);
	StringBuffer ret = cd.getColorByDlg(hwndOwner,title,argv);
	if (ret.length() <= 0) ret = errorStr;
	cd.setNotify(ret);
}

/*
	「フォントの指定」ダイアログを表示する
*/
void
GetFontByDlg(HWND hwndOwner, ConvData& cd, CmdLineParser& argv)
{
	StringBuffer title = argv.getArgvStr(0);
	argv.delArgv(0);
	StringBuffer ret = cd.getFontByDlg(hwndOwner,title,argv);
	if (ret.length() <= 0) ret = errorStr;
	cd.setNotify(ret);
}

