//	$Id: ms_func.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	ms_func.cpp
 *	メニュー表示関数
 */

#include "DengakuDLL.h"

//	ポップアップメニューを表示し選択結果を返す
DENGAKUDLL_API LPCSTR
MENU(LPCSTR menuname, HIDEDLL_NUMTYPE hWndHavingCaret)
{
	POINT pt;
	POINT* pAt = NULL;
	if (hWndHavingCaret != 0) {
		pAt = &pt;
		::GetCaretPos(pAt);
		::ClientToScreen((HWND)hWndHavingCaret,pAt);
	}

	//	showMenuThread() 内でも同じコードがあるが、
	//	効いてないようなのでここにも入れる...何で？
	HWND hwndTop = ::GetForegroundWindow();

	//	メニューを表示するスレッドの開始
	//	メニューの表示を別スレッドのウィンドウに任せる理由：
	//	＊）このスレッド内でメニューを表示させると
	//		秀丸のウィンドウをクリックした時点で
	//		「マクロを終了しますか？」とでる。
	//		これは透明ウィンドウを（このスレッドで）作成しても同様。
	//		マクロ動作中の秀丸はメッセージループ内(DispatchMessage に渡す前)で
	//		WM_LBUTTONDOWN をフックしているものと思われる。
	if (!g_pSessionInstance->showMenuThread(menuname,pAt)) return errorStr;

	//	スレッドが停止するまで待つ
	g_pSessionInstance->waitMenuThread();

	//	返り値の取得
	*g_pStrBuffer = nullStr;
	g_pSessionInstance->getNotify(*g_pStrBuffer,INFINITE);

	//	スレッドの破棄
	g_pSessionInstance->stopMenuThread();

	::SetForegroundWindow(hwndTop);

	return *g_pStrBuffer;
}

//	新規メニューの構築
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWMENU(LPCSTR menuname)
{
	return g_pSessionInstance->si_newmenu(menuname,TRUE);
}

//	メニュー項目の追加(属性値あり)
DENGAKUDLL_API HIDEDLL_NUMTYPE
ADDMENUITEMEX(LPCSTR menuname, LPCSTR title, LPCSTR name, LPCSTR opt)
{
	return g_pSessionInstance->si_addmenuitem(menuname,title,name,opt);
}

//	メニュー項目の追加(属性値なし)
DENGAKUDLL_API HIDEDLL_NUMTYPE
ADDMENUITEM(LPCSTR menuname, LPCSTR title, LPCSTR name)
{
	return g_pSessionInstance->si_addmenuitem(menuname,title,name,nullStr);
}

//	メニュー項目の変更(属性値あり)
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGEMENUITEMEX(LPCSTR name, HIDEDLL_NUMTYPE index, LPCSTR str, LPCSTR opt)
{
	return g_pSessionInstance->si_changemenuitem(name,index,str,opt);
}

//	メニュー項目の変更(属性値なし)
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGEMENUITEM(LPCSTR name, HIDEDLL_NUMTYPE index, LPCSTR str)
{
	return g_pSessionInstance->si_changemenuitem(name,index,str,nullStr);
}

//	メニュー項目を削除
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELETEMENUITEM(LPCSTR menuname, HIDEDLL_NUMTYPE index)
{
	return g_pSessionInstance->si_deletemenuitem(menuname,index);
}

