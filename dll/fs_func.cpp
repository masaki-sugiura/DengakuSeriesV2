//	$Id: fs_func.cpp,v 1.5 2004-11-16 17:03:51 sugiura Exp $
/*
 *	fs_func.cpp
 *	ファイルサービスの関数
 */

#include "DengakuDLL.h"
#include "cmdline.h"

//	カレントディレクトリの変更
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCURDIR(LPCSTR str)
{
	try {
		return g_pSessionInstance->si_setcurdir(RemoveQuote(str));
	} catch (...) {
		return 0;
	}
}

//	UNIX like cp
DENGAKUDLL_API HIDEDLL_NUMTYPE
COPY(LPCSTR strOrg, LPCSTR strDest)
{
	try {
		RealCmdLineParser argv(strOrg);
		argv.addArgv(RemoveQuote(strDest));
		return g_pSessionInstance->si_copy(argv);
	} catch (...) {
		return 0;
	}
}

//	str1 の末尾に str2 を追加
DENGAKUDLL_API HIDEDLL_NUMTYPE
APPENDCOPY(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_appendcopy(RemoveQuote(str1),
												RemoveQuote(str2),FALSE);
	} catch (...) {
		return 0;
	}
}

//	str1 の末尾に str2 を追加(EOF は除く)
DENGAKUDLL_API HIDEDLL_NUMTYPE
APPENDCOPYTEXT(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_appendcopy(RemoveQuote(str1),
												RemoveQuote(str2),TRUE);
	} catch (...) {
		return 0;
	}
}

//	ファイルの削除
DENGAKUDLL_API HIDEDLL_NUMTYPE
REMOVE(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);	//	const CmdLineParser& ではないので
		return g_pSessionInstance->si_remove(argv);
	} catch (...) {
		return 0;
	}
}

//	ファイルの移動(名前変更)
DENGAKUDLL_API HIDEDLL_NUMTYPE
MOVE(LPCSTR strOrg, LPCSTR strDest)
{
	try {
		RealCmdLineParser argv(strOrg);
		argv.addArgv(RemoveQuote(strDest));
		return g_pSessionInstance->si_move(argv);
	} catch (...) {
		return 0;
	}
}

//	ディレクトリの作成
DENGAKUDLL_API HIDEDLL_NUMTYPE
MKDIR(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_mkdir(argv);
	} catch (...) {
		return 0;
	}
}

//	ディレクトリの削除
DENGAKUDLL_API HIDEDLL_NUMTYPE
RMDIR(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_rmdir(argv);
	} catch (...) {
		return 0;
	}
}

//	属性値の変更
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETATTRIBUTE(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_setattribute(argv);
	} catch (...) {
		return 0;
	}
}

//	タイムスタンプの変更
DENGAKUDLL_API HIDEDLL_NUMTYPE
TOUCH(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_touch(argv);
	} catch (...) {
		return 0;
	}
}

//	ファイル操作系関数の結果(-n オプション指定時のみ)を返す
DENGAKUDLL_API LPCSTR
FILEOPRESULT()
{
	try {
		g_strBuffer = g_pSessionInstance->si_fileopresult();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	普通に run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
RUN(LPSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_run(argv,SW_SHOW);
	} catch (...) {
		return 0;
	}
}

//	アイコン状態で run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
ICONRUN(LPSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_run(argv,SW_SHOWMINNOACTIVE);
	} catch (...) {
		return 0;
	}
}

//	最大化状態で run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
MAXRUN(LPSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_run(argv,SW_SHOWMAXIMIZED);
	} catch (...) {
		return 0;
	}
}

//	不可視状態で run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
HIDERUN(LPSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_run(argv,SW_HIDE);
	} catch (...) {
		return 0;
	}
}

//	ファイル・ディレクトリの再帰的列挙
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMPATH(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_enumpath(argv);
	} catch (...) {
		return 0;
	}
}

//	ファイルの列挙
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMFILE(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_enumfile(argv);
	} catch (...) {
		return 0;
	}
}

//	ディレクトリの列挙
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMDIR(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_enumdir(argv);
	} catch (...) {
		return 0;
	}
}

//	カレントディレクトリを返す
DENGAKUDLL_API LPCSTR
GETCURDIR()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getcurdir();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ファイルが読取専用かどうかを返す
DENGAKUDLL_API LPCSTR
ISREADONLY(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_isreadonly(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ファイルのサイズを返す
DENGAKUDLL_API LPCSTR
SIZEOF(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_sizeof(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	タイムスタンプを返す
DENGAKUDLL_API LPCSTR
TIMESTAMPOF(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_timestampof(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	タイムスタンプをあるルールで数値にしたものを返す
DENGAKUDLL_API LPCSTR
TIMECOUNTOF(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_timecountof(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ファイルの属性値を返す
DENGAKUDLL_API LPCSTR
ATTRIBUTEOF(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_attributeof(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
EXISTFILE(LPSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getlongname(RemoveQuote(str),TRUE);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ENUM***() で取得したファイル・ディレクトリ名を返す
DENGAKUDLL_API LPCSTR
FINDNEXT()
{
	try {
		g_strBuffer = g_pSessionInstance->si_findnext();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	現在使用可能なドライブを返す
DENGAKUDLL_API LPCSTR
GETDRIVES()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getdrives();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ファイル名をロングファイル名に変換する
DENGAKUDLL_API LPCSTR
GETLONGNAME(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getlongname(RemoveQuote(str),FALSE);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ファイル名をDOSファイル名に変換する
DENGAKUDLL_API LPCSTR
GETSHORTNAME(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getshortname(RemoveQuote(str),FALSE);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

