//	$Id: main.cpp,v 1.3 2002-02-20 16:48:40 sugiura Exp $
/*
 *	main.cpp
 *	田楽サーバ本体
 */

#include "resource.h"
#include "common.h"
#include "ddeserv.h"
#include "ddesinfo.h"
#include "misc.h"

#include <commctrl.h>

/*
    コマンドライン解釈＆DDEServerInfo構造体へのデータの格納を実行する関数

	サポートしているコマンドラインオプションは以下の通り
	    /h                  :   アイコンを隠す
	    /e                  :   マクロサーバ互換モード
	    /s ServiceName      :   基本サービス名を ServiceName に変える
	    /l [LogFileName]    :   サーバのログを取得

    ※実行ファイル名が macserv.exe だった場合、または基本サービス名が
      "HideMacroServer" だった場合、マクロサーバ互換モードが有効になる。
*/
static BOOL
ParseCmdLine(DDEServerInfo& dsi)
{
//	Windows プログラムでも __argc, __argv でコンソールプログラムのように
//	コマンドラインを取得できる…らしい(^^;。
//	で、長い(^^;ので別名を定義
#ifdef __GNUC__
extern int _argc;
extern char** _argv;
#define ac _argc
#define av _argv
#else
#define	ac __argc
#define	av __argv
#endif
//	不正なパラメータのチェック
	if (ac < 1) return FALSE;

//	フルパスの実行ファイル名を取得
	dsi.m_sbModuleName.resize(MAX_PATH);
	::GetModuleFileName(dsi.m_hInst,dsi.m_sbModuleName.getBufPtr(),MAX_PATH);
	if (dsi.m_sbModuleName.length() <= 0) return FALSE;

//	実行ファイル名が "macserv.exe" ならマクロサーバ互換モードをＯＮにする
	LPCSTR ptr = lstrrchr((LPCSTR)dsi.m_sbModuleName,'\\');
	if (ptr != NULL && lstrcmpi(++ptr,GetServerString(SVR_STR_MSEXENAME)) == 0)
		dsi.m_bStrictEmulation = TRUE;

//	オプションの解析
	//	/s, /l オプションの次の文字列を格納するバッファへのポインタ
	StringBuffer* ppopt = NULL;
	for (char** pptr = av; *pptr != NULL; pptr++) {
		switch (**pptr) {
		case '/':	//	オプション引数("/" も "-" も使用可能)
		case '-':
			switch (*++*pptr) {
			case 'h':	//	アイコンを隠す
				dsi.m_bHideIcon = TRUE;
				ppopt = NULL;
				break;
			case 's':	//	基本サービス名の設定
				if (!dsi.m_bStrictEmulation) ppopt = &dsi.m_sbServiceName;
				break;
			case 'l':	//	サーバのログを取る
				if (!dsi.m_psbServerLogFileName) {
					try {
						dsi.m_psbServerLogFileName
							= new StringBuffer(nullStr);
					} catch (exception&) {
						//	no operation.
					}
				}
				ppopt = dsi.m_psbServerLogFileName;
				break;
			case 'e':	//	マクロサーバ互換モードをＯＮ
				dsi.m_bStrictEmulation = TRUE;
				ppopt = NULL;
				break;
			default:	//	解釈不能なオプション
				ppopt = NULL;
			}
			//	/s, /l オプション指定時に "/sServicename" の様な
			//	書き方をしている場合への対処
			if (ppopt != NULL && *++*pptr) {
				*ppopt = *pptr;
				ppopt = NULL;
			}
			break;

		default:	//	"/?"("-?") でない文字列の場合
			if (ppopt != NULL) {
				//	直前が "/s" または "/l" だった場合への対処
				*ppopt = *pptr;
				ppopt = NULL;
			}
		}
	}

	//	基本サービス名の設定
	if (dsi.m_bStrictEmulation)
		//	マクロサーバの基本サービス名(="HideMacroServer")
		dsi.m_sbServiceName = GetServerString(SVR_STR_MSSERVICENAME);
	else if (dsi.m_sbServiceName.length() <= 0)
		//	デフォルトの基本サービス名(="DengakuServer")に設定
		dsi.m_sbServiceName = GetServerString(SVR_STR_DSSERVICENAME);

	//	デフォルトのログファイル名の設定
	if (dsi.m_psbServerLogFileName != NULL &&
		dsi.m_psbServerLogFileName->length() <= 0) {
		StringBuffer& lfn = *dsi.m_psbServerLogFileName;
		lfn = dsi.m_sbModuleName;
		int sep = lfn.rfind((TCHAR)'\\');
		lfn.setlength(++sep);
		lfn.append(dsi.m_sbServiceName).append(".log");
	}

	//	ミューテックスオブジェクトの名前の生成
	dsi.m_sbMutexName = GetServerString(SVR_STR_DSMUTEXNAME);
	dsi.m_sbMutexName.append(dsi.m_sbServiceName);

	//	メインウィンドウのタイトルの生成
	dsi.m_sbWndTitle = GetServerString(SVR_STR_SERVERNAME);
	dsi.m_sbWndTitle.append(" - ").append(dsi.m_sbServiceName);

	return TRUE;
}

//	メインウィンドウを初期化する関数
static BOOL
InitMainWindow(DDEServerInfo* pdsi)
{
	//	ウィンドウタイトルの登録
	::SetWindowText(pdsi->m_hwndMain,pdsi->m_sbWndTitle);

	//	アプリ名、バージョン情報
	StringBuffer name(GetServerString(SVR_STR_SERVERNAME),-1,16);
	name.append(" Ver.").append(GetServerString(SVR_STR_VERSION));
	::SetDlgItemText(pdsi->m_hwndMain,IDC_APPNAME,name);

	//	タスクトレイにアイコンを登録する構造体の初期化
	NOTIFYICONDATA* lptnid = &pdsi->m_NotifyIconData;
	lptnid->cbSize = sizeof(NOTIFYICONDATA);
	lptnid->hWnd = pdsi->m_hwndMain;
	lptnid->uID = 1;
	lptnid->uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	lptnid->uCallbackMessage = WM_USER_MENU;	//	MainWndProc への MSG
	lptnid->hIcon = (HICON)::LoadImage(pdsi->m_hInst,
										MAKEINTRESOURCE(IDI_ICON),
										IMAGE_ICON,
										16,16,0);
	lstrcpy(lptnid->szTip,pdsi->m_sbWndTitle);

	//	ウィンドウを画面中央に移動
	CenteringWindow(pdsi->m_hwndMain, ::GetDesktopWindow(),
					SWP_NOACTIVATE|SWP_NOREDRAW|SWP_NOSIZE|SWP_HIDEWINDOW);
	::SetWindowPos(pdsi->m_hwndMain, HWND_TOPMOST, 0, 0, 0, 0,
					SWP_NOACTIVATE|SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE|SWP_HIDEWINDOW);

	return TRUE;
}


/*
	メインウィンドウのウィンドウプロシージャ

	やっていること：
		・アイコンの登録(抹消)
		・DDE サーバの開始/終了
		・アイコンからのメッセージ(WM_LBUTTONDBLCLK, WM_RBUTTONDOWN)の処理
		・メニューからのメッセージの処理

*/
BOOL CALLBACK
MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DDEServerInfo*
		pdsi = reinterpret_cast<DDEServerInfo*>(::GetWindowLong(hWnd,
																DWL_USER));

	switch (uMsg) {
	case WM_INITDIALOG:	//	メインウィンドウの初期化処理
		//	lParam : サーバ情報の入った構造体へのポインタ
		::SetWindowLong(hWnd,DWL_USER,lParam);
		pdsi = reinterpret_cast<DDEServerInfo*>(lParam);

		//	ウィンドウハンドルの登録
		pdsi->m_hwndMain = hWnd;

		//	メインウィンドウの初期化本体
		if (!InitMainWindow(pdsi)) {
			ErrorMessageBox(MSG_ERR_START);
			::DestroyWindow(hWnd);
			return TRUE;
		}

		//	タスクトレイにアイコンを登録
		if (!pdsi->m_bHideIcon)
			::Shell_NotifyIcon(NIM_ADD,&pdsi->m_NotifyIconData);

		//	DDE サーバの開始
		if (!DdeServer::init(*pdsi)) {
			ErrorMessageBox(MSG_ERR_START);
			::DestroyWindow(hWnd);
		}
		return FALSE;


	case WM_COMMAND:	//	コントロール/メニューからのメッセージ処理
		switch (LOWORD(wParam)) {
		case IDOK:	//	「ＯＫ」ボタンが押された
			//	メインウィンドウを再び隠す
			::ShowWindow(hWnd,SW_HIDE);
			break;

		case IDM_ABOUT:	//	メニューの「田楽サーバについて」が選択された
			//	基本サービス名を表示
			::SetDlgItemText(hWnd,IDC_SERVICENAME,pdsi->m_sbServiceName);
			//	現在確立している DDE 通信の数を表示
			::SetDlgItemInt(hWnd,IDC_CONVNUM,DdeServer::getConvNum(),FALSE);
			//	メインウィンドウを表示
			::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,
							SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
			break;

		case IDM_HELP:		//	メニューの「ヘルプ」が選択された
		case IDC_SHOWHELP:	//	「ヘルプ」ボタンが押された
			{
				//	ヘルプファイル名の生成
				StringBuffer buf(pdsi->m_sbModuleName);
				buf.setlength(buf.length()-3);
				buf.append("hlp");
				//	ヘルプの呼出し
				::WinHelp(hWnd,buf,HELP_FINDER,0L);
			}
			break;

		case IDM_QUIT:	//	メニューの「終了」が選択された
		case IDC_CLOSE:	//	「終了」ボタンが押された
			//	終了ボタン/メニュー項目を選択
			::DestroyWindow(hWnd);
			break;

		}
		break;

	case WM_CLOSE:	//	終了メッセージ
		::DestroyWindow(hWnd);
		break;

	case WM_DESTROY:	//	サーバの終了
		if (!pdsi->m_bHideIcon)
			::Shell_NotifyIcon(NIM_DELETE,&pdsi->m_NotifyIconData);
		if (!DdeServer::uninit()) ErrorMessageBox(MSG_ERR_STOP);
		::PostQuitMessage(0);
		break;

	case WM_USER_MENU:	//	タスクトレイアイコンからのメッセージ
		switch (LOWORD(lParam)) {
		case WM_LBUTTONDBLCLK:	//	左ダブルクリック
			//	メインウィンドウを表示
			::PostMessage(hWnd,WM_COMMAND,IDM_ABOUT,0L);
			break;

		case WM_RBUTTONDOWN:	//	右クリック
			{
				//	メニューを表示
				POINT pt;
				::SetForegroundWindow(hWnd);
				::GetCursorPos(&pt);
				::TrackPopupMenu(
							::GetSubMenu(
									::LoadMenu(pdsi->m_hInst,
												MAKEINTRESOURCE(IDR_MENU)),
									0),
							TPM_RIGHTALIGN|TPM_RIGHTBUTTON,
							pt.x, pt.y, 0, hWnd, NULL);
			}
			break;
		}
		break;

	case WM_USER_DDE_STOP:	//	DDE 通信/サーバの終了
		if (DdeServer::isActive()) {
			//	(ConvData*)lParam != NULL : 特定の通信の終了
			//					   = NULL : 全通信の終了
			DdeServer::deleteConvData(reinterpret_cast<ConvData*>(lParam));
			if (!lParam) ::DestroyWindow(hWnd);	//	サーバを終了
		}
		break;

	case WM_USER_SHOWICON:	//	タスクトレイのアイコンの表示/非表示の切替
		if (static_cast<WPARAM>(pdsi->m_bHideIcon) != wParam) {
			pdsi->m_bHideIcon = wParam;
			::Shell_NotifyIcon(pdsi->m_bHideIcon ? NIM_DELETE : NIM_ADD,
								&pdsi->m_NotifyIconData);
		}
		break;

	case WM_USER_NOTIFYPOSTADV:	//	このプロセス内で DdePostAdvise() を呼ぶ
		DdeServer::notifyPostAdvise((ConvData*)lParam);
		break;

	default:
		return FALSE;

	}
	return TRUE;
}

#ifdef __GNUC__

static LPCSTR
	pszClassName = "DengakuServerClass",
	pszCaption = "田楽サーバについて",
	pszFontName = "ＭＳ Ｐゴシック",
	pszOK = "OK",
	pszHelp = "ヘルプ",
	pszQuit = "終了",
	pszTitle = "田楽 (でんがく) サーバ  Ver.2.00β",
	pszCopyRight = "Copyright(C) 1998-2002, M. Sugiura (sugiura@ceres.dti.ne.jp)",
	pszLabelConnect = "現在の DDE 通信",
	pszLabelService = "サービス名：",
	pszLabelNumber  = "通信数：";

#define CLASSNAME_SIZE (19 * sizeof(WORD))
#define CAPTION_SIZE   (10 * sizeof(WORD))
#define FONTNAME_SIZE  ( 9 * sizeof(WORD))
#define OK_SIZE        ( 3 * sizeof(WORD))
#define HELP_SIZE      ( 4 * sizeof(WORD))
#define QUIT_SIZE      ( 3 * sizeof(WORD))
#define TITLE_SIZE     (25 * sizeof(WORD))
#define COPYRIGHT_SIZE (61 * sizeof(WORD))
#define CONNECT_SIZE   (11 * sizeof(WORD))
#define SERVICE_SIZE   ( 7 * sizeof(WORD))
#define NUMBER_SIZE    ( 5 * sizeof(WORD))

#define DLGTEMPL_SIZE 1024 * 256

#define IDC_STATIC (WORD)(-1)

static LPWSTR
AddDlgItem(
	LPWSTR pwStr,
	DWORD style,
	WORD x, WORD y, WORD cx, WORD cy, WORD id,
	WORD cls,
	LPCSTR title)
{
	LPDLGITEMTEMPLATE pdit = (LPDLGITEMTEMPLATE)pwStr;
	pdit->style = style;
	pdit->dwExtendedStyle = 0;
	pdit->x = x;
	pdit->y = y;
	pdit->cx = cx;
	pdit->cy = cy;
	pdit->id = id;
	pwStr += sizeof(DLGITEMTEMPLATE) / sizeof(WORD);
	*pwStr++ = 0xFFFF;
	*pwStr++ = cls;
	pwStr += StringBuffer(title).toUnicode(pwStr);
	if ((((DWORD)pwStr) & 0x3) != 0) *pwStr++ = 0;
	return pwStr;
}

static HWND
CreateMainDialog(HINSTANCE hInst, LPARAM lParam)
{
	Array<BYTE> pTemplate(DLGTEMPLSIZE);
	LPDLGTEMPLATE pdt = (LPDLGTEMPLATE)(BYTE*)pTemplate;

	pdt->style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
	pdt->dwExtendedStyle = 0;
	pdt->cdit = 10;
	pdt->x = 0;
	pdt->y = 0;
	pdt->cx = 229;
	pdt->cy = 109;
	LPWSTR pwStr = (LPWSTR)(pTemplate + sizeof(DLGTEMPLATE));
	*pwStr++ = 0; // no menu name
	pwStr += StringBuffer(pszClassName).toUnicode(pwStr);
	pwStr += StringBuffer(pszCaption).toUnicode(pwStr);
	*pwStr++ = 9;
	pwStr += StringBuffer(pszFontName).toUnicode(pwStr);
	if ((((DWORD)pwStr) & 0x3) != 0) *pwStr++ = 0;

	// OK button
	pwStr = AddDlgItem(pwStr,
						BS_DEFPUSHBUTTON | BS_PUSHBUTTON |
							WS_CHILD | WS_TABSTOP | WS_GROUP,
						57, 88, 50, 14, IDOK, 0x0080,
						pszOK);
	// Help button
	pwStr = AddDlgItem(pwStr,
						BS_PUSHBUTTON | WS_CHILD | WS_TABSTOP | WS_GROUP,
						114, 88, 50, 14, IDC_SHOWHELP, 0x0080,
						pszHelp);
	// Quit button
	pwStr = AddDlgItem(pwStr,
						BS_PUSHBUTTON | WS_CHILD | WS_TABSTOP | WS_GROUP,
						171, 88, 50, 14, IDC_CLOSE, 0x0080,
						pszQuit);
	// Title text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						33, 13, 108, 8, IDC_APPNAME, 0x0082,
						pszTitle);
	// Copyright text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						33, 23, 187, 8, IDC_STATIC, 0x0082,
						pszCopyRight);
	// Connect groupbox
	pwStr = AddDlgItem(pwStr,
						BS_GROUPBOX | WS_CHILD | WS_GROUP,
						7, 39, 215, 47, IDC_STATIC, 0x0080,
						pszLabelConnect);
	// Service title text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						13, 52, 41, 8, IDC_STATIC, 0x0082,
						pszLabelService);
	// Number title text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						26, 65, 28, 8, IDC_STATIC, 0x0082,
						pszLabelNumber);
	// Service text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						57, 52, 128, 8, IDC_SERVICENAME, 0x0082,
						"");
	// Number text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						57, 65, 128, 8, IDC_CONVNUM, 0x0082,
						"");

	HWND hDlg = ::CreateDialogIndirectParam(hInst,
											(LPCDLGTEMPLATE)pTemplate,
											NULL,
											(DLGPROC)MainWndProc,
											lParam);

	return hDlg;
}

#endif // __GNUC__

static void
CloseStdHandle(ULONG hStd)
{
	HANDLE hStdHandle;
	if ((hStdHandle = ::GetStdHandle(hStd)) != NULL) {
		::CloseHandle(hStdHandle);
		::SetStdHandle(hStd,NULL);
	}
}

int APIENTRY
WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine,
	int nCmdShow)
{
	int	ret_code = 0;	//	リターンコード

	try {

	//	サーバ情報を格納するクラスの生成
	Auto_Ptr<DDEServerInfo>	pDdeSI(new DDEServerInfo(hInstance));

	//	コマンドラインの解釈＆サービス名etc.の取得
	if (!ParseCmdLine(*pDdeSI)) return 1;

	//	複数のインスタンスの起動を抑止するため Mutex オブジェクトを作成。
	//	但し、Mutex の名前は基本サービス名毎に固有なので、
	//	別の基本サービス名を持つインスタンスは複数起動できる。
	HANDLE hMutex = ::CreateMutex(NULL,FALSE,pDdeSI->m_sbMutexName);
	if (::GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND hwndPrev = ::FindWindow(GetServerString(SVR_STR_DSCLASSNAME),
									 pDdeSI->m_sbWndTitle);
		//	タスクトレイのアイコンの状態を変更
		if (hwndPrev != NULL)
			::SendMessage(hwndPrev, WM_USER_SHOWICON, pDdeSI->m_bHideIcon, 0);

		//	このプロセスは終了
		::CloseHandle(hMutex);
		return 0;
	}

	//	この処理をしないと親プロセスが各ファイルを
	//	リダイレクトしている場合に DOS アプリが正常に起動しない。
	//	理由はイマイチよく分からない＆対処療法的だが
	//	…まぁしょうがなかろう(^^;。
	CloseStdHandle(STD_INPUT_HANDLE);
	CloseStdHandle(STD_OUTPUT_HANDLE);
	CloseStdHandle(STD_ERROR_HANDLE);

	//	コモンコントロールライブラリの初期化
	::InitCommonControls();

	//	(お約束(^^;の)メインウィンドウのウィンドウクラスの登録
	WNDCLASS wc;
	::ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style = CS_CLASSDC|CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)DefDlgProc;	//	メインウィンドウは
	wc.cbWndExtra = DLGWINDOWEXTRA;			//	ダイアログ
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
	wc.lpszClassName = GetServerString(SVR_STR_DSCLASSNAME);

	HWND hwndDlg;
	if (::RegisterClass(&wc) &&
#ifdef __GNUC__
		(hwndDlg = CreateMainDialog(hInstance, (LPARAM)pDdeSI.ptr()))) {
#else
		(hwndDlg = ::CreateDialogParam(hInstance,
										MAKEINTRESOURCE(IDD_ABOUT),
										NULL,
										(DLGPROC)MainWndProc,
										(LPARAM)pDdeSI.ptr()))) {
#endif
		//	これまたお約束のメッセージループ
		MSG msg;
		while (::GetMessage(&msg,NULL,0,0)) {
			//	メインウィンドウ(ダイアログ)へのメッセージ配送
			if (!::IsDialogMessage(hwndDlg,&msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		ret_code = msg.wParam;
	} else {
		ErrorMessageBox(MSG_ERR_START);
		ret_code = 1;
	}

	//	後始末
	::CloseHandle(hMutex);

	} catch (...) {
		return -1;
	}

	return ret_code;
}

