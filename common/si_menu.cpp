//	$Id: si_menu.cpp,v 1.8 2011-01-07 16:08:38 sugiura Exp $
/*
 *	si_menu.cpp
 *	メニュー表示関数
 */

#include "session.h"
#include "cmdline.h"
#include "menu.h"
#include "str_tbl.h"

//	ダミーウィンドウのウィンドウクラス名
LPCSTR pszClassName = "DengakuDLLWndClass";

const StringBuffer errorStr  = "!";
const StringBuffer cancelStr = "0";

//	ポップアップメニューの親ウィンドウのコールバック関数
LRESULT CALLBACK
UserMenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE) {
		lParam = (LPARAM)((LPCREATESTRUCT)lParam)->lpCreateParams;
		::SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)lParam);
		//	ウィンドウ全体を覆うようにサイズ変更
		RECT rct;
		::GetWindowRect(::GetDesktopWindow(),&rct);
		::SetWindowPos(hWnd,HWND_TOPMOST,0,0,rct.right,rct.bottom,
						SWP_HIDEWINDOW|SWP_NOREDRAW);
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	LPSHOWMENUPROCARGS
		lpsmpa = reinterpret_cast<LPSHOWMENUPROCARGS>(
									::GetWindowLongPtr(hWnd,GWLP_USERDATA));

	switch (uMsg) {
	case WM_USER_SHOWUSERMENU:
		{
			//	ポップアップメニューの表示
			if (lpsmpa->m_bShowAtCursor) {
				POINT pt;
				::GetCursorPos(&pt);
				lpsmpa->m_mnPosInfo.x += pt.x;
				lpsmpa->m_mnPosInfo.y += pt.y;
			}
			lpsmpa->m_wFirstShow = TRUE; // WM_INITMENUPOPUP 参照
			HWND hwndTop = ::GetForegroundWindow();
			if (hwndTop != NULL) ::UpdateWindow(hwndTop);
			//	ウィンドウを可視化(透明だけど(^^;)
			::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,
						SWP_SHOWWINDOW|SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE);
			::SetForegroundWindow(hWnd);
			//	メニューの表示
			lpsmpa->m_wSelectedID
				= (WORD)::TrackPopupMenu(lpsmpa->m_hMenu,
										TPM_RETURNCMD,
										lpsmpa->m_mnPosInfo.x,
										lpsmpa->m_mnPosInfo.y,
										0,hWnd,NULL);
			//	画面のクリア
			::InvalidateRect(hWnd,NULL,FALSE);
			::UpdateWindow(hWnd);
			//	ウィンドウを隠す
			::SetWindowPos(hWnd,HWND_TOP,0,0,0,0,
						SWP_HIDEWINDOW|SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE);
			::DestroyWindow(hWnd);
			::SetForegroundWindow(hwndTop);
		}
		break;

	case WM_INITMENUPOPUP:
		//	メニューが表示される直前にメニュー項目を構築
		if (lpsmpa != NULL) {
			lpsmpa->m_pSessionInstance->getUserMenu()
					.createMenu(reinterpret_cast<HMENU>(wParam));
			if (lpsmpa->m_wFirstShow) {
				// メニューが表示されて最初の項目を選択状態にする
				// (下カーソルキーのエミュレーション(苦笑))
				::PostMessage(hWnd, WM_KEYDOWN, VK_DOWN, 1);
				::PostMessage(hWnd, WM_KEYUP, VK_DOWN, 1);
				lpsmpa->m_wFirstShow = FALSE;
			}
		}
		break;

	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		//	メニューハンドルの削除
		if (lpsmpa != NULL)
			lpsmpa->m_pSessionInstance->getUserMenu().deleteMenu();
		::PostQuitMessage(0L);
		break;

	default:
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 0;
}

DWORD WINAPI
ShowMenuProc(LPVOID pThreadArgs)
{
	if (pThreadArgs == NULL) {
		::ExitThread(1);	//	有り得ないはず！
	}

	LPSHOWMENUPROCARGS psmpa = (LPSHOWMENUPROCARGS)pThreadArgs;

	WNDCLASS wc;
	::ZeroMemory(&wc,sizeof(WNDCLASS));
	wc.hInstance		=	psmpa->m_pSessionInstance->getInstanceHandle();
	wc.lpszClassName	=	pszClassName;
	wc.style			=	CS_CLASSDC|CS_DBLCLKS;
	wc.lpfnWndProc		=	(WNDPROC)UserMenuProc;
	wc.hbrBackground	=	(HBRUSH)/*(COLOR_BTNFACE+1)*/ NULL;
	::RegisterClass(&wc);

	psmpa->m_wSelectedID = MENURESULT_ERR_ID_MASK; // error

	DWORD ret = (DWORD)-1;
	//	(透明な)ダミーウィンドウの作成
	HWND hwndDummy = ::CreateWindowEx(WS_EX_TRANSPARENT /* | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE */,
									pszClassName,
									"DengakuDLL DummyWindow",
									WS_POPUP,
									0,0,		//	値はダミー
									100,100,	//	値はダミー
									NULL,
									NULL,
									(HINSTANCE)wc.hInstance,
									psmpa);
	if (hwndDummy != NULL) {
		//	メニューの表示
		::PostMessage(hwndDummy,WM_USER_SHOWUSERMENU,0,0);
		//	メッセージループ
		MSG	msg;
		while (::GetMessage(&msg,NULL,0,0)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		ret = msg.wParam;
	}
	::UnregisterClass(pszClassName, (HINSTANCE)wc.hInstance);

	psmpa->m_pSessionInstance->sendMenuResult(psmpa);

	::ExitThread(ret);
	return (DWORD)-1;	//	有り得ない！！
}

BOOL
SessionInstance::showMenuThread(const StringBuffer& menuname,
								BOOL bShowAtCursor,
								POINT pt)
{
	if (menuname.length() <= 0 || m_pMenuThread.ptr() != NULL) return FALSE;

	//	メニュー表示スレッドに渡す引数構造体
	LPSHOWMENUPROCARGS psmpa = new SHOWMENUPROCARGS;
	if (psmpa == NULL) return FALSE;
	::ZeroMemory(psmpa, sizeof(SHOWMENUPROCARGS));
	psmpa->m_pSessionInstance = this;

	//	名前で指定されたメニュー(とそのサブメニュー)を構築
	psmpa->m_hMenu = m_UserMenu.precreateMenuRecursive(menuname);
	if (psmpa->m_hMenu == NULL) {
		delete psmpa;
		return FALSE;
	}
	psmpa->m_bShowAtCursor = bShowAtCursor;
	psmpa->m_mnPosInfo = pt;

	//	メニューを表示するスレッドの開始
	//	メニューの表示を別スレッドのウィンドウに任せる理由：
	//	＊）このスレッド内でメニューを表示させると
	//	秀丸のウィンドウをクリックした時点で「マクロを終了しますか？」とでる。
	//	これは透明ウィンドウを（このスレッドで）作成しても同様。
	//	マクロ動作中の秀丸はメッセージループ内(DispatchMessage に渡す前)で
	//	WM_LBUTTONDOWN をフックしているものと思われる。
	m_pMenuThread
		= new Thread((LPTHREAD_START_ROUTINE)ShowMenuProc,(LPVOID)psmpa);
	if (m_pMenuThread.ptr() == NULL) return FALSE;
	return m_pMenuThread->run();
}

void
SessionInstance::sendMenuResult(LPSHOWMENUPROCARGS psmpa)
{
	if (m_pMenuThread.ptr() == NULL ||
		psmpa == NULL ||
		(psmpa->m_wSelectedID&MENURESULT_ERR_ID_MASK) != 0) {
		this->setNotify(errorStr);
		return;
	}

	//	LOBYTE(id)	:	項目ID(1～)
	//	HIBYTE(id)	:	(index of Menu in MenuList) + 1.
	WORD id = psmpa->m_wSelectedID;
	if (id == 0) {	//	キャンセル
		this->setNotify(cancelStr);
		return;
	}
	//	エラーでない
	int	index = (int)HIBYTE(id) - 1;
	Menu* menu = psmpa->m_pSessionInstance->m_UserMenu.getItemByIndex(index);
	if (menu == NULL) {
		this->setNotify(errorStr);
		return;
	}
	//	返り値は "menuname:index"
	this->setNotify(StringBuffer(menu->getName())
								.append((TCHAR)':')
								.append((int)(UINT)LOBYTE(id)));
}

DWORD
SessionInstance::waitMenuThread(DWORD waittime)
{
	if (m_pMenuThread.ptr() == NULL) return THREAD_INVALID_EXITCODE;
	//	スレッドが停止するまで待つ
	return m_pMenuThread->wait(waittime);
}

DWORD
SessionInstance::stopMenuThread(DWORD waittime)
{
	if (m_pMenuThread.ptr() == NULL) return THREAD_INVALID_EXITCODE;
	DWORD exitcode = m_pMenuThread->getExitCode();
	LPSHOWMENUPROCARGS psmpa = (LPSHOWMENUPROCARGS)m_pMenuThread->getProcArg();
	// clean up the background window of the menu
	HWND hwndBack = ::WindowFromPoint(psmpa->m_mnPosInfo);
	if (hwndBack != NULL) {
		::InvalidateRect(hwndBack,NULL,FALSE);
		::UpdateWindow(hwndBack);
//		::SetForegroundWindow(hwndBack);
	}
	delete psmpa;
	m_pMenuThread = NULL; // Thread を破壊
	return exitcode;
}

int
SessionInstance::si_newmenu(const StringBuffer& name, BOOL bExtended)
{
	if (name.length() <= 0) {
		return FALSE;
	}
	//	新しいメニューの割り当て
	return m_UserMenu.addMenu(new Menu(name, bExtended)) > 0;
}

int
SessionInstance::si_addmenuitem(
	const StringBuffer& name,
	const StringBuffer& title,
	const StringBuffer& submenu,
	const StringBuffer& opt)
{
	//	処理対象のメニューを検索
	Menu* menu = m_UserMenu.getMenu(name);
	if (menu == NULL) return FALSE;
	if (title.length() <= 0) {
		//	セパレータの挿入
		return menu->addItem(new MenuItem(MENUTYPE_SEPARATOR)) > 0;
	} else if (submenu.length() <= 0) {
		//	通常の項目の挿入
		return menu->addItem(new MenuItem(MENUTYPE_ITEM,title,nullStr,opt)) > 0;
	} else {
		//	サブメニューを持つ項目の挿入
		return menu->addItem(new MenuItem(MENUTYPE_POPUP,title,submenu,opt)) > 0;
	}
}

int
SessionInstance::si_changemenuitem(
	const StringBuffer& name,
	int id,
	const StringBuffer& text,
	const StringBuffer& state)
{
	//	処理対象のメニューを検索
	Menu* menu = m_UserMenu.getMenu(name);
	if (menu == NULL) return FALSE;
	//	処理対象のメニュー項目を検索
	MenuItem* item = menu->getItemByIndex(id - 1);
	if (item == NULL) return FALSE;
	item->setText(text);	//	項目テキストを更新
	item->setState(state);	//	項目の状態を更新
	return TRUE;
}

int
SessionInstance::si_deletemenuitem(const StringBuffer& name, int id)
{
	//	処理対象のメニューを検索
	Menu* menu = m_UserMenu.getMenu(name);
	if (menu == NULL) return FALSE;
	if (id <= 0) {
		//	全ての項目を削除
		while (menu->delItemByIndex(0) > 0)
			/* no operation */;
	} else {
		//	指定項目を削除
		menu->delItemByIndex(id - 1);
	}
	return TRUE;
}

