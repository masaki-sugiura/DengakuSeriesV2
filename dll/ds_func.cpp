//	$Id: ds_func.cpp,v 1.13 2009-09-20 13:49:01 sugiura Exp $
/*
 *	ds_func.cpp
 *	ダイアログ操作関数
 */

#include "DengakuDLL.h"
#include "cmdline.h"

//#define	OUTPUT_INOUT_TIME

#ifdef OUTPUT_INOUT_TIME
class OutputTickCount
{
public:
	OutputTickCount(LPCSTR pszFuncName)
		:	m_pszFuncName(pszFuncName)
	{
		Output("Enter");
	}
	~OutputTickCount()
	{
		Output("Leave");
	}

private:
	LPCSTR m_pszFuncName;

	void Output(LPCSTR pszHeader)
	{
		CHAR szBuffer[80];
		
		wsprintf(szBuffer, "%s %s: %d\n", pszHeader, m_pszFuncName, ::GetTickCount());

		::OutputDebugString(szBuffer);
	}
};
#define	OUTPUT_TICK_COUNT(s)	OutputTickCount tc(s)
#else
#define	OUTPUT_TICK_COUNT(s)	//	nothing to do.
#endif

//	ダイアログからの通知コードの到着を待つ
DENGAKUDLL_API LPCSTR
WAITCTRLNOTIFY(HIDEDLL_NUMTYPE num)
{
	OUTPUT_TICK_COUNT("WAITCTRLNOTIFY");
	try {
		if (num < 0) num = 0;
		g_strBuffer = nullStr;
		if (g_pSessionInstance->getDlgFrame().getUserDlg() != NULL)
			g_pSessionInstance->getNotify(g_strBuffer,(DWORD)num);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ダイアログの表示
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWDIALOG(HIDEDLL_NUMTYPE num1, HIDEDLL_NUMTYPE num2)
{
	OUTPUT_TICK_COUNT("SHOWDIALOG");
	try {
		return g_pSessionInstance->si_showdialog((HWND)num1,(BOOL)num2);
	} catch (...) {
		return 0;
	}
}

//	ダイアログの破棄
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENDDIALOG()
{
	OUTPUT_TICK_COUNT("ENDDIALOG");
	try {
		return g_pSessionInstance->si_enddialog();
	} catch (...) {
		return 0;
	}
}

//	コントロールの状態を設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSTATE(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("SETCTRLSTATE");
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlstate(str1,argv);
	} catch (...) {
		return 0;
	}
}

//	コントロールのテキストを設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSTRING(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("SETCTRLSTRING");
	try {
		return g_pSessionInstance->si_setctrlstring(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	コントロールの IME の状態を設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLIMESTATE(LPCSTR str1, HIDEDLL_NUMTYPE num2)
{
	OUTPUT_TICK_COUNT("SETCTRLIMESTATE");
	try {
		return g_pSessionInstance->si_setctrlimestate(str1,num2);
	} catch (...) {
		return 0;
	}
}

//	コントロールのソートの状態を設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSORT(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("SETCTRLSORT");
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlsort(str1, argv);
	} catch (...) {
		return 0;
	}
}

//	コントロールの項目の内容を設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGECTRLITEM(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	OUTPUT_TICK_COUNT("CHANGECTRLITEM");
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_changectrlitem(str1,argv,str3);
	} catch (...) {
		return 0;
	}
}

//	コントロールの項目を追加する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLITEM(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	OUTPUT_TICK_COUNT("SETCTRLITEM");
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlitem(str1,argv,str3);
	} catch (...) {
		return 0;
	}
}

//	コントロールの項目を削除する
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELETECTRLITEM(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("DELETECTRLITEM");
	try {
		return g_pSessionInstance->si_deletectrlitem(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	コントロールの項目を全削除する
DENGAKUDLL_API HIDEDLL_NUMTYPE
RESETCTRLITEM(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("RESETCTRLITEM");
	try {
		return g_pSessionInstance->si_resetctrlitem(str1);
	} catch (...) {
		return 0;
	}
}

//	コントロールの有効・無効を切りかえる
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENABLECTRL(HIDEDLL_NUMTYPE num, LPCSTR str)
{
	OUTPUT_TICK_COUNT("ENABLECTRL");
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_enablectrl(num,argv);
	} catch (...) {
		return 0;
	}
}

//	コントロールのフォントを設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLFONT(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("SETCTRLFONT");
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlfont(str1,argv);
	} catch (...) {
		return 0;
	}
}

//	コントロール中のフォーカスを得る項目の変更
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLFOCUSEDITEM(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("SETCTRLFOCUSEDITEM");
	try {
		return g_pSessionInstance->si_setctrlfocuseditem(str1, str2);
	} catch (...) {
		return 0;
	}
}

//	メッセージボックスを表示する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWMESSAGE(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("SHOWMESSAGE");
	try {
		return g_pSessionInstance->si_showmessage(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	質問に答えるメッセージボックスを表示する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWQUESTION(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	OUTPUT_TICK_COUNT("SHOWQUESTION");
	try {
		return g_pSessionInstance->si_showquestion(str1,str2,str3);
	} catch (...) {
		return 0;
	}
}

//	ダイアログの現在の内容をデータファイルに保存する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SAVEDIALOG(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("SAVEDIALOG");
	try {
		return g_pSessionInstance->si_savedialog(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	保存したダイアログのデータファイルからデータを読み込む
DENGAKUDLL_API HIDEDLL_NUMTYPE
LOADDIALOG(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("LOADDIALOG");
	try {
		return g_pSessionInstance->si_loaddialog(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	ダイアログのタイトルを設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETDLGTITLE(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("SETDLGTITLE");
	try {
		return g_pSessionInstance->si_setdlgtitle(str1);
	} catch (...) {
		return 0;
	}
}

//	ダイアログのタイトルを返す
DENGAKUDLL_API LPCSTR
GETDLGTITLE()
{
	OUTPUT_TICK_COUNT("GETDLGTITLE");
	try {
		g_strBuffer = g_pSessionInstance->si_getdlgtitle();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ダイアログ全体のIMEの状態を設定する
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETDLGIMESTATE(HIDEDLL_NUMTYPE num1)
{
	OUTPUT_TICK_COUNT("SETDLGIMESTATE");
	try {
		return g_pSessionInstance->si_setdlgimestate(num1);
	} catch (...) {
		return 0;
	}
}

//	ダイアログ全体のIMEの状態を返す
DENGAKUDLL_API LPCSTR
GETDLGIMESTATE()
{
	OUTPUT_TICK_COUNT("GETDLGIMESTATE");
	try {
		g_strBuffer = g_pSessionInstance->si_getdlgimestate();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	コントロールの状態を返す
DENGAKUDLL_API LPCSTR
GETCTRLSTATE(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("GETCTRLSTATE");
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlstate(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	コントロールのテキストを返す
DENGAKUDLL_API LPCSTR
GETCTRLSTRING(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("GETCTRLSTRING");
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlstring(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	コントロールの IME の状態を返す
DENGAKUDLL_API LPCSTR
GETCTRLIMESTATE(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("GETCTRLIMESTATE");
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlimestate(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	コントロールのソートの状態を返す
DENGAKUDLL_API LPCSTR
GETCTRLSORT(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("GETCTRLSORT");
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlsort(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	コントロールのフォント情報を返す
DENGAKUDLL_API LPCSTR
GETCTRLFONT(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("GETCTRLFONT");
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlfont(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	コントロールの項目テキストを返す
DENGAKUDLL_API LPCSTR
GETCTRLITEM(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("GETCTRLITEM");
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlitem(str1,str2);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	コントロール中のフォーカスを得ている項目を返す
DENGAKUDLL_API LPCSTR
GETCTRLFOCUSEDITEM(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("GETCTRLFOCUSEDITEM");
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlfocuseditem(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ダイアログデータファイルのシグネチャを返す
DENGAKUDLL_API LPCSTR
GETDLGSIGNATURE(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("GETDLGSIGNATURE");
	try {
		g_strBuffer = g_pSessionInstance->si_getdlgsignature(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	新規ダイアログテンプレートの作成
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWDIALOG(LPCSTR str1, HIDEDLL_NUMTYPE num1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("NEWDIALOG");
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_newdialog(str1,(WORD)num1,argv);
	} catch (...) {
		return 0;
	}
}

//	新規子ダイアログテンプレートの作成
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWDLGPAGE(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	OUTPUT_TICK_COUNT("NEWDLGPAGE");
	try {
		return g_pSessionInstance->si_newdlgpage(str1,(WORD)num1);
	} catch (...) {
		return 0;
	}
}

//	変更対象となる子ダイアログの選択
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCURDLGPAGE(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("SETCURDLGPAGE");
	try {
		return g_pSessionInstance->si_setcurdlgpage(str1);
	} catch (...) {
		return 0;
	}
}

//	フォーカスを得るコントロールの変更
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETFOCUSEDCTRL(LPCSTR str1)
{
	OUTPUT_TICK_COUNT("SETFOCUSEDCTRL");
	try {
		return g_pSessionInstance->si_setfocusedctrl(str1);
	} catch (...) {
		return 0;
	}
}

//	現在変更対象となっている子ダイアログの名前を返す
DENGAKUDLL_API LPCSTR
GETCURDLGPAGE()
{
	OUTPUT_TICK_COUNT("GETCURDLGPAGE");
	try {
		g_strBuffer = g_pSessionInstance->si_getcurdlgpage();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	現在フォーカスを得ているコントロールの名前を返す
DENGAKUDLL_API LPCSTR
GETFOCUSEDCTRL()
{
	OUTPUT_TICK_COUNT("GETFOCUSEDCTRL");
	try {
		g_strBuffer = g_pSessionInstance->si_getfocusedctrl();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	新規コントロールの作成
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWCONTROL(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	OUTPUT_TICK_COUNT("NEWCONTROL");
	try {
		return g_pSessionInstance->si_newcontrol(str1,str2,str3);
	} catch (...) {
		return 0;
	}
}

//	コントロールの幅を設定
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLWIDTH(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	OUTPUT_TICK_COUNT("SETCTRLWIDTH");
	try {
		return g_pSessionInstance->si_setctrlwidth(str1,(WORD)num1);
	} catch (...) {
		return 0;
	}
}

//	コントロールの高さを設定
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLHEIGHT(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	OUTPUT_TICK_COUNT("SETCTRLHEIGHT");
	try {
		return g_pSessionInstance->si_setctrlheight(str1,(WORD)num1);
	} catch (...) {
		return 0;
	}
}

//	コントロールの通知コードを設定
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLNOTIFY(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("SETCTRLNOTIFY");
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlnotify(str1,argv);
	} catch (...) {
		return 0;
	}
}

//	コントロールの拡張プロパティを設定
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLEXPROPERTY(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	OUTPUT_TICK_COUNT("SETCTRLEXPROPERTY");
	try {
		return g_pSessionInstance->si_setctrlexproperty(str1, str2, str3);
	} catch (...) {
		return 0;
	}
}

//	コントロールの拡張プロパティを設定
DENGAKUDLL_API LPCSTR
GETCTRLEXPROPERTY(LPCSTR str1, LPCSTR str2)
{
	OUTPUT_TICK_COUNT("GETCTRLEXPROPERTY");
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlexproperty(str1, str2);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	新しい列の開始
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWCOLUMN(HIDEDLL_NUMTYPE num1)
{
	OUTPUT_TICK_COUNT("NEWCOLUMN");
	try {
		return g_pSessionInstance->si_newcolumn((WORD)num1);
	} catch (...) {
		return 0;
	}
}

//	新しいページの開始
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWPAGE(HIDEDLL_NUMTYPE num1)
{
	OUTPUT_TICK_COUNT("NEWPAGE");
	try {
		return g_pSessionInstance->si_newpage((WORD)num1);
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API LPCSTR
GETDLGPOS()
{
	OUTPUT_TICK_COUNT("GETDLGPOS");
	try {
		g_strBuffer = g_pSessionInstance->si_getdlgpos();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
SETDLGPOS(HIDEDLL_NUMTYPE x, HIDEDLL_NUMTYPE y, LPCSTR pszOrigin, LPCSTR pszUnit)
{
	OUTPUT_TICK_COUNT("SETDLGPOS");
	try {
		return g_pSessionInstance->si_setdlgpos(x, y, pszOrigin, pszUnit);
	} catch (...) {
		return 0;
	}
}

DENGAKUDLL_API LPCSTR
GETDLGSIZE()
{
	OUTPUT_TICK_COUNT("GETDLGSIZE");
	try {
		g_strBuffer = g_pSessionInstance->si_getdlgsize();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

