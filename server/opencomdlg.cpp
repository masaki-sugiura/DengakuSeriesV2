//	$Id: opencomdlg.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	opencomdlg.cpp
 *	�R�����_�C�A���O��\������֐��̎���
 */

#include "opencomdlg.h"
#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "seldir.h"

/*
	�u�t�@�C�����J���v�_�C�A���O��\������
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
	�u�t�H���_�̎w��v�_�C�A���O��\������
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
	�u�F�̎w��v�_�C�A���O��\������
*/
void
GetColorByDlg(HWND hwndOwner, ConvData& cd, CmdLineParser& argv)
{
	StringBuffer ret = cd.getColorByDlg(hwndOwner,
									argv.getArgvStr(0),
									argv.getArgvStr(1));
	if (ret.length() <= 0) ret = errorStr;
	cd.setNotify(ret);
}

/*
	�u�t�H���g�̎w��v�_�C�A���O��\������
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

