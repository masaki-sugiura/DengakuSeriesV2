//	$Id: fs_func.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
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
	return g_pSessionInstance->si_setcurdir(RemoveQuote(str));
}

//	UNIX like cp
DENGAKUDLL_API HIDEDLL_NUMTYPE
COPY(LPCSTR strOrg, LPCSTR strDest)
{
	RealCmdLineParser argv(strOrg);
	argv.addArgv(RemoveQuote(strDest));
	return g_pSessionInstance->si_copy(argv);
}

//	str1 の末尾に str2 を追加
DENGAKUDLL_API HIDEDLL_NUMTYPE
APPENDCOPY(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_appendcopy(RemoveQuote(str1),
											RemoveQuote(str2),FALSE);
}

//	str1 の末尾に str2 を追加(EOF は除く)
DENGAKUDLL_API HIDEDLL_NUMTYPE
APPENDCOPYTEXT(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_appendcopy(RemoveQuote(str1),
											RemoveQuote(str2),TRUE);
}

//	ファイルの削除
DENGAKUDLL_API HIDEDLL_NUMTYPE
REMOVE(LPCSTR str)
{
	RealCmdLineParser argv(str);	//	const CmdLineParser& ではないので
	return g_pSessionInstance->si_remove(argv);
}

//	ファイルの移動(名前変更)
DENGAKUDLL_API HIDEDLL_NUMTYPE
MOVE(LPCSTR strOrg, LPCSTR strDest)
{
	RealCmdLineParser argv(strOrg);
	argv.addArgv(RemoveQuote(strDest));
	return g_pSessionInstance->si_move(argv);
}

//	ディレクトリの作成
DENGAKUDLL_API HIDEDLL_NUMTYPE
MKDIR(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_mkdir(argv);
}

//	ディレクトリの削除
DENGAKUDLL_API HIDEDLL_NUMTYPE
RMDIR(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_rmdir(argv);
}

//	属性値の変更
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETATTRIBUTE(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_setattribute(argv);
}

//	タイムスタンプの変更
DENGAKUDLL_API HIDEDLL_NUMTYPE
TOUCH(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_touch(argv);
}

//	普通に run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
RUN(LPSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_run(argv,SW_SHOW);
}

//	アイコン状態で run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
ICONRUN(LPSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_run(argv,SW_SHOWMINNOACTIVE);
}

//	最大化状態で run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
MAXRUN(LPSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_run(argv,SW_SHOWMAXIMIZED);
}

//	不可視状態で run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
HIDERUN(LPSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_run(argv,SW_HIDE);
}

//	ファイル・ディレクトリの再帰的列挙
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMPATH(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_enumpath(argv);
}

//	ファイルの列挙
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMFILE(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_enumfile(argv);
}

//	ディレクトリの列挙
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMDIR(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_enumdir(argv);
}

//	カレントディレクトリを返す
DENGAKUDLL_API LPCSTR
GETCURDIR()
{
	*g_pStrBuffer = g_pSessionInstance->si_getcurdir();
	return *g_pStrBuffer;
}

//	ファイルが読取専用かどうかを返す
DENGAKUDLL_API LPCSTR
ISREADONLY(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_isreadonly(argv);
	return *g_pStrBuffer;
}

//	ファイルのサイズを返す
DENGAKUDLL_API LPCSTR
SIZEOF(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_sizeof(argv);
	return *g_pStrBuffer;
}

//	タイムスタンプを返す
DENGAKUDLL_API LPCSTR
TIMESTAMPOF(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_timestampof(argv);
	return *g_pStrBuffer;
}

//	タイムスタンプをあるルールで数値にしたものを返す
DENGAKUDLL_API LPCSTR
TIMECOUNTOF(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_timecountof(argv);
	return *g_pStrBuffer;
}

//	ファイルの属性値を返す
DENGAKUDLL_API LPCSTR
ATTRIBUTEOF(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_attributeof(argv);
	return *g_pStrBuffer;
}

DENGAKUDLL_API LPCSTR
EXISTFILE(LPSTR str)
{
	*g_pStrBuffer = g_pSessionInstance->si_getlongname(RemoveQuote(str),TRUE);
	return *g_pStrBuffer;
}

//	ENUM***() で取得したファイル・ディレクトリ名を返す
DENGAKUDLL_API LPCSTR
FINDNEXT()
{
	*g_pStrBuffer = g_pSessionInstance->si_findnext();
	return *g_pStrBuffer;
}

//	現在使用可能なドライブを返す
DENGAKUDLL_API LPCSTR
GETDRIVES()
{
	*g_pStrBuffer = g_pSessionInstance->si_getdrives();
	return *g_pStrBuffer;
}

//	ファイル名をロングファイル名に変換する
DENGAKUDLL_API LPCSTR
GETLONGNAME(LPCSTR str)
{
	*g_pStrBuffer = g_pSessionInstance->si_getlongname(RemoveQuote(str),FALSE);
	return *g_pStrBuffer;
}

