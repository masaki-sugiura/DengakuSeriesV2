//	$Id: ds_func.cpp,v 1.2 2002-01-16 16:31:04 sugiura Exp $
/*
 *	ds_func.cpp
 *	ダイアログ操作関数
 */

#include "DengakuDLL.h"
#include "cmdline.h"

//	ダイアログからの通知コードの到着を待つ
DENGAKUDLL_API LPCSTR
WAITCTRLNOTIFY(HIDEDLL_NUMTYPE num)
{
	if (num < 0) num = 0;
	g_strBuffer = nullStr;
	if (g_pSessionInstance->getDlgFrame().getUserDlg() != NULL)
		g_pSessionInstance->getNotify(g_strBuffer,(DWORD)num);
	return g_strBuffer;
}

//	ダイアログの表示
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWDIALOG(HIDEDLL_NUMTYPE num1, HIDEDLL_NUMTYPE num2)
{
	return g_pSessionInstance->si_showdialog((HWND)num1,(BOOL)num2);
}

//	ダイアログの破棄
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENDDIALOG()
{
	return g_pSessionInstance->si_enddialog();
}

//	コントロールの状態を設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSTATE(LPCSTR str1, LPCSTR str2)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_setctrlstate(str1,argv);
}

//	コントロールのテキストを設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSTRING(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_setctrlstring(str1,str2);
}

//	コントロールの IME の状態を設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLIMESTATE(LPCSTR str1, HIDEDLL_NUMTYPE num2)
{
	return g_pSessionInstance->si_setctrlimestate(str1,num2);
}

//	コントロールの項目の内容を設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGECTRLITEM(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_changectrlitem(str1,argv,str3);
}

//	コントロールの項目を追加する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLITEM(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_setctrlitem(str1,argv,str3);
}

//	コントロールの項目を削除する
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELETECTRLITEM(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_deletectrlitem(str1,str2);
}

//	コントロールの項目を全削除する
DENGAKUDLL_API HIDEDLL_NUMTYPE
RESETCTRLITEM(LPCSTR str1)
{
	return g_pSessionInstance->si_resetctrlitem(str1);
}

//	コントロールの有効・無効を切りかえる
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENABLECTRL(HIDEDLL_NUMTYPE num, LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_enablectrl(num,argv);
}

//	コントロールのフォントを設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLFONT(LPCSTR str1, LPCSTR str2)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_setctrlfont(str1,argv);
}

//	メッセージボックスを表示する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWMESSAGE(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_showmessage(str1,str2);
}

//	質問に答えるメッセージボックスを表示する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWQUESTION(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	return g_pSessionInstance->si_showquestion(str1,str2,str3);
}

//	ダイアログの現在の内容をデータファイルに保存する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SAVEDIALOG(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_savedialog(str1,str2);
}

//	保存したダイアログのデータファイルからデータを読み込む
DENGAKUDLL_API HIDEDLL_NUMTYPE
LOADDIALOG(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_loaddialog(str1,str2);
}

//	ダイアログのタイトルを設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETDLGTITLE(LPCSTR str1)
{
	return g_pSessionInstance->si_setdlgtitle(str1);
}

//	ダイアログのタイトルを返す
DENGAKUDLL_API LPCSTR
GETDLGTITLE()
{
	g_strBuffer = g_pSessionInstance->si_getdlgtitle();
	return g_strBuffer;
}

//	コントロールの状態を返す
DENGAKUDLL_API LPCSTR
GETCTRLSTATE(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getctrlstate(str1);
	return g_strBuffer;
}

//	コントロールのテキストを返す
DENGAKUDLL_API LPCSTR
GETCTRLSTRING(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getctrlstring(str1);
	return g_strBuffer;
}

//	コントロールの IME の状態を返す
DENGAKUDLL_API LPCSTR
GETCTRLIMESTATE(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getctrlimestate(str1);
	return g_strBuffer;
}

//	コントロールのフォント情報を返す
DENGAKUDLL_API LPCSTR
GETCTRLFONT(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getctrlfont(str1);
	return g_strBuffer;
}

//	コントロールの項目テキストを返す
DENGAKUDLL_API LPCSTR
GETCTRLITEM(LPCSTR str1, LPCSTR str2)
{
	g_strBuffer = g_pSessionInstance->si_getctrlitem(str1,str2);
	return g_strBuffer;
}

//	ダイアログデータファイルのシグネチャを返す
DENGAKUDLL_API LPCSTR
GETDLGSIGNATURE(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getdlgsignature(str1);
	return g_strBuffer;
}

//	新規ダイアログテンプレートの作成
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWDIALOG(LPCSTR str1, HIDEDLL_NUMTYPE num1, LPCSTR str2)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_newdialog(str1,(WORD)num1,argv);
}

//	新規子ダイアログテンプレートの作成
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWDLGPAGE(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_newdlgpage(str1,(WORD)num1);
}

//	変更対象となる子ダイアログの選択
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCURDLGPAGE(LPCSTR str1)
{
	return g_pSessionInstance->si_setcurdlgpage(str1);
}

//	現在変更対象となっている子ダイアログの名前を返す
DENGAKUDLL_API LPCSTR
GETCURDLGPAGE()
{
	g_strBuffer = g_pSessionInstance->si_getcurdlgpage();
	return g_strBuffer;
}

//	新規コントロールの作成
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWCONTROL(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	return g_pSessionInstance->si_newcontrol(str1,str2,str3);
}

//	コントロールの幅を設定
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLWIDTH(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_setctrlwidth(str1,(WORD)num1);
}

//	コントロールの高さを設定
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLHEIGHT(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_setctrlheight(str1,(WORD)num1);
}

//	コントロールの通知コードを設定
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLNOTIFY(LPCSTR str1, LPCSTR str2)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_setctrlnotify(str1,argv);
}

//	新しい列の開始
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWCOLUMN(HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_newcolumn((WORD)num1);
}

//	新しいページの開始
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWPAGE(HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_newpage((WORD)num1);
}

