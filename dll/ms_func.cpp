//	$Id: ms_func.cpp,v 1.5 2003-07-06 16:27:46 sugiura Exp $
/*
 *	ms_func.cpp
 *	メニュー表示関数
 */

#include "DengakuDLL.h"

static LPCSTR
show_menu(LPCSTR menuname, HIDEDLL_NUMTYPE hWndHavingCaret,
		  HIDEDLL_NUMTYPE x_offset, HIDEDLL_NUMTYPE y_offset)
{
	try {
		POINT pt;
		if (hWndHavingCaret != 0) {
			::GetCaretPos(&pt);
			::ClientToScreen((HWND)hWndHavingCaret, &pt);
			pt.x += x_offset;
			pt.y += y_offset;
		} else {
			pt.x = x_offset;
			pt.y = y_offset;
		}

		//	showMenuThread() 内でも同じコードがあるが、
		//	効いてないようなのでここにも入れる...何で？
		HWND hwndTop = ::GetForegroundWindow();
		if (hwndTop != NULL) ::UpdateWindow(hwndTop);

		//	メニューを表示するスレッドの開始
		//	メニューの表示を別スレッドのウィンドウに任せる理由：
		//	＊）このスレッド内でメニューを表示させると
		//		秀丸のウィンドウをクリックした時点で
		//		「マクロを終了しますか？」とでる。
		//		これは透明ウィンドウを（このスレッドで）作成しても同様。
		//		マクロ動作中の秀丸はメッセージループ内(DispatchMessage に渡す前)で
		//		WM_LBUTTONDOWN をフックしているものと思われる。
		if (!g_pSessionInstance->showMenuThread(menuname,
												hWndHavingCaret == 0,
												pt)) {
			return errorStr;
		}

		//	スレッドが停止するまで待つ
		g_pSessionInstance->waitMenuThread();

		//	返り値の取得
		g_strBuffer = nullStr;
		g_pSessionInstance->getNotify(g_strBuffer,INFINITE);

		//	スレッドの破棄
		g_pSessionInstance->stopMenuThread();

		if (hwndTop != NULL) ::SetForegroundWindow(hwndTop);

		return g_strBuffer;

	} catch (...) {
		return "";
	}
}

//	ポップアップメニューを表示し選択結果を返す
DENGAKUDLL_API LPCSTR
MENU(LPCSTR menuname, HIDEDLL_NUMTYPE hWndHavingCaret)
{
	return show_menu(menuname, hWndHavingCaret, 0, 0);
}

//	ポップアップメニューを表示し選択結果を返す
DENGAKUDLL_API LPCSTR
MENUEX(LPCSTR menuname, HIDEDLL_NUMTYPE hWndHavingCaret,
	   HIDEDLL_NUMTYPE x_offset, HIDEDLL_NUMTYPE y_offset)
{
	return show_menu(menuname, hWndHavingCaret, x_offset, y_offset);
}

//	新規メニューの構築
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWMENU(LPCSTR menuname)
{
	try {
		return g_pSessionInstance->si_newmenu(menuname,TRUE);
	} catch (...) {
		return 0;
	}
}

//	メニュー項目の追加(属性値あり)
DENGAKUDLL_API HIDEDLL_NUMTYPE
ADDMENUITEMEX(LPCSTR menuname, LPCSTR title, LPCSTR name, LPCSTR opt)
{
	try {
		return g_pSessionInstance->si_addmenuitem(menuname,title,name,opt);
	} catch (...) {
		return 0;
	}
}

//	メニュー項目の追加(属性値なし)
DENGAKUDLL_API HIDEDLL_NUMTYPE
ADDMENUITEM(LPCSTR menuname, LPCSTR title, LPCSTR name)
{
	try {
		return g_pSessionInstance->si_addmenuitem(menuname,title,name,nullStr);
	} catch (...) {
		return 0;
	}
}

//	メニュー項目の変更(属性値あり)
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGEMENUITEMEX(LPCSTR name, HIDEDLL_NUMTYPE index, LPCSTR str, LPCSTR opt)
{
	try {
		return g_pSessionInstance->si_changemenuitem(name,index,str,opt);
	} catch (...) {
		return 0;
	}
}

//	メニュー項目の変更(属性値なし)
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGEMENUITEM(LPCSTR name, HIDEDLL_NUMTYPE index, LPCSTR str)
{
	try {
		return g_pSessionInstance->si_changemenuitem(name,index,str,nullStr);
	} catch (...) {
		return 0;
	}
}

//	メニュー項目を削除
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELETEMENUITEM(LPCSTR menuname, HIDEDLL_NUMTYPE index)
{
	try {
		return g_pSessionInstance->si_deletemenuitem(menuname,index);
	} catch (...) {
		return 0;
	}
}

