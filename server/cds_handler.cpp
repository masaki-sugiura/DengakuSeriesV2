//	$Id: cds_handler.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	cds_handler.cpp
 *	コモンダイアログサービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "opencomdlg.h"

LRESULT CALLBACK
OpenComDlgBaseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPOPENCOMDLGPROCARGS pocdpa;
	switch (uMsg) {
	case WM_CREATE:
		::SetWindowPos(hWnd,
						HWND_TOPMOST,
						0,0,0,0,
						SWP_NOREDRAW|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

	case WM_USER_SHOWCOMDLG:
		if (lParam == 0) break;
		pocdpa = (LPOPENCOMDLGPROCARGS)lParam;
		(*pocdpa->m_pfnComDlg)(pocdpa->m_hwndOwner,*pocdpa->m_pConvData,*pocdpa->m_pArgv);
		// through down.

	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0L);
		break;

	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

// コモンダイアログを表示するためのスレッドの実体
DWORD
OpenComDlgProc(LPVOID pParam)
{
	if (pParam == NULL) ::ExitThread((DWORD)-1);

	LPOPENCOMDLGPROCARGS pocdpa = (LPOPENCOMDLGPROCARGS)pParam;

	if (pocdpa->m_pfnComDlg == NULL ||
		pocdpa->m_pConvData == NULL ||
		pocdpa->m_pArgv == NULL)
		::ExitThread((DWORD)-1);

	WNDCLASSEX wc;
	::ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = DdeServer::gethInst();
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)OpenComDlgBaseProc;
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.lpszClassName = "DengakuServerOpenComDlgBase";
	if (::RegisterClassEx(&wc) == 0) ::ExitThread((DWORD)-1);

	RECT rct_desktop;
	::GetWindowRect(::GetDesktopWindow(),&rct_desktop);
	HWND hwndBase = ::CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
									wc.lpszClassName,
									"OpenComDlgBase",
									WS_POPUP,
									0,0,rct_desktop.right,rct_desktop.bottom,
									NULL,
									NULL,
									(HINSTANCE)wc.hInstance,
									NULL);
	DWORD ret = (DWORD)-1;
	if (hwndBase != NULL) {
		//	ダイアログの表示
		pocdpa->m_hwndOwner = hwndBase;
		::PostMessage(hwndBase, WM_USER_SHOWCOMDLG, 0, (LPARAM)pocdpa);
		//	メッセージループ
		MSG	msg;
		while (::GetMessage(&msg,NULL,0,0)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		ret = msg.wParam;
	}
	::UnregisterClass(wc.lpszClassName, (HINSTANCE)wc.hInstance);

	delete pocdpa->m_pArgv;
	delete pocdpa;

	::ExitThread(ret);

	return (DWORD)-1; // not reached
}

//	getfilename のＤＬＬとサーバの引数の違いを吸収するメソッド
void
ConvData::reorderArgv_getfilename(
	StringBuffer& argv0,
	StringBuffer& argv1) const
{
	StringBuffer tmp = argv0;
	argv0 = argv1;
	argv1 = tmp;
}

int
ConvData::do_opencomdlg(
	CmdLineParser& params,
	PFNOPENCOMDLGPROC pfnOpenComDlgProc)
{
	this->resetNotify();
	LPOPENCOMDLGPROCARGS pocdpa = new OPENCOMDLGPROCARGS;
	pocdpa->m_pConvData = this;
	pocdpa->m_pfnComDlg = pfnOpenComDlgProc;
	pocdpa->m_pArgv = new RealCmdLineParser(nullStr);
	int num = params.itemNum();
	for (int i = 0; i < num; i++)
		pocdpa->m_pArgv->addArgv(params.getArgvStr(i));
	m_pComDlgThread
		= new Thread((LPTHREAD_START_ROUTINE)OpenComDlgProc,pocdpa);
	if (m_pComDlgThread.ptr() == NULL) return FALSE;
	return m_pComDlgThread->run();
}

//	ファイル名を取得するコモンダイアログを表示
int
ConvData::On_getfilename(CmdLineParser& params)
{
	return this->do_opencomdlg(params, (PFNOPENCOMDLGPROC)GetFileNameByDlg);
}

//	フォルダ名を取得するコモンダイアログを表示
int
ConvData::On_getdirname(CmdLineParser& params)
{
	return this->do_opencomdlg(params, (PFNOPENCOMDLGPROC)GetDirNameByDlg);
}

//	色のＲＧＢ値を取得するコモンダイアログを表示
int
ConvData::On_getcolor(CmdLineParser& params)
{
	return this->do_opencomdlg(params, (PFNOPENCOMDLGPROC)GetColorByDlg);
}

//	フォントの属性を取得するコモンダイアログを表示
int
ConvData::On_getfont(CmdLineParser& params)
{
	return this->do_opencomdlg(params, (PFNOPENCOMDLGPROC)GetFontByDlg);
}

