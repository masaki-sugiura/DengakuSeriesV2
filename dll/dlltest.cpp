//	$Id: dlltest.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	dlltest.cpp
 *	テスト用アプリ
 */
#include <windows.h>

HINSTANCE	hModule;

BOOL
initDll()
{
	hModule = ::LoadLibrary("DengakuDLL.dll");
	if (hModule == NULL) {
		::MessageBox(NULL,"DLL load failed.",NULL,MB_OK);
		return FALSE;
	}
	return TRUE;
}

typedef LPCSTR (*LPFNSTRFUNC)(PVOID,...);
typedef int	(*LPFNINTFUNC)(PVOID,...);
typedef LPCSTR (*LPFNSTRFUNC_NOARG)();
typedef int (*LPFNINTFUNC_NOARG)();

BOOL
showDialog(HWND hWnd)
{
	FARPROC	lpfnDialog = ::GetProcAddress(hModule,"SHOWDIALOG"),
			lpfnNewDialog = ::GetProcAddress(hModule,"NEWDIALOG"),
			lpfnNewControl = ::GetProcAddress(hModule,"NEWCONTROL"),
			lpfnSetCtrlItem = ::GetProcAddress(hModule,"SETCTRLITEM"),
			lpfnSetCtrlHeight = ::GetProcAddress(hModule,"SETCTRLHEIGHT"),
			lpfnSetCtrlNotify = ::GetProcAddress(hModule,"SETCTRLNOTIFY"),
			lpfnNewDlgPage = ::GetProcAddress(hModule,"NEWDLGPAGE");

	if (lpfnDialog == NULL		||
		lpfnNewDialog == NULL	||
		lpfnNewControl == NULL	||
		lpfnSetCtrlItem == NULL	||
		lpfnNewDlgPage == NULL) {
		::MessageBox(NULL,"some function entry point cannot resolved.",NULL,MB_OK);
		return FALSE;
	}
	(*(LPFNINTFUNC)lpfnNewDialog)((PVOID)"test dialog",50,"");
	(*(LPFNINTFUNC)lpfnNewControl)((PVOID)"tree","tree1","");
	(*(LPFNINTFUNC)lpfnSetCtrlItem)((PVOID)"","n1,parent1","root");
	(*(LPFNINTFUNC)lpfnSetCtrlItem)((PVOID)"","n2,parent2","root");
	(*(LPFNINTFUNC)lpfnSetCtrlItem)((PVOID)"","n11,child1-1","n1");
	(*(LPFNINTFUNC)lpfnSetCtrlHeight)((PVOID)"",(int)5);
	(*(LPFNINTFUNC)lpfnNewControl)((PVOID)"button","btn1","delete");
	(*(LPFNINTFUNC)lpfnSetCtrlNotify)((PVOID)"","2");
	(*(LPFNINTFUNC)lpfnNewControl)((PVOID)"okcancel","","");

	if ((*(LPFNINTFUNC)lpfnDialog)((PVOID)0,FALSE) == 0) {
		::MessageBox(NULL,"failed to do dialog.",NULL,MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL
waitNotify()
{
	static FARPROC	lpfnWaitNotify = ::GetProcAddress(hModule,"WAITCTRLNOTIFY"),
					lpfnGetCtrlState = ::GetProcAddress(hModule,"GETCTRLSTATE"),
					lpfnGetCtrlItem = ::GetProcAddress(hModule,"GETCTRLITEM"),
					lpfnDeleteCtrlItem = ::GetProcAddress(hModule,"DELETECTRLITEM");
	LPCSTR	pszRet = (*(LPFNSTRFUNC)lpfnWaitNotify)((PVOID)1);
	if (*pszRet != '\0') {
		if (*pszRet == '0' || *pszRet == '1') {
			return TRUE;
		} else if (*pszRet == '2') {
			static TCHAR	buf[80];
			lstrcpy(buf,(*(LPFNSTRFUNC)lpfnGetCtrlState)((PVOID)"tree1"));
			if (lstrlen(buf) > 0) {
				(*(LPFNINTFUNC)lpfnDeleteCtrlItem)((PVOID)"tree1",buf);
			}
		}
	}
	return FALSE;
}

BOOL
endDialog()
{
	static FARPROC	lpfnEndDialog = ::GetProcAddress(hModule,"ENDDIALOG");
	if ((*(LPFNINTFUNC_NOARG)lpfnEndDialog)() == 0) {
		::MessageBox(NULL,"failed to enddialog.",NULL,MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL
uninitDll()
{
	if (hModule != NULL) ::FreeLibrary(hModule);
	return TRUE;
}

BOOL bShowDialog = FALSE;

LRESULT CALLBACK
MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		initDll();
//		if (showDialog(hWnd)) ::PostMessage(hWnd,WM_USER+100,0,0);
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);

//	case WM_COMMAND:
//		{
//			FARPROC	lpfnFunc = ::GetProcAddress(hModule,"CALC");
//			FARPROC	lpfnFunc = ::GetProcAddress(hModule,"SETVAR");
//			if (lpfnFunc == NULL) break;
//			(*(LPFNINTFUNC)lpfnFunc)("@Name1","Value1");
//			::DestroyWindow(hWnd);
//		}
//		break;

	case WM_USER+100:
//		if (!waitNotify()) ::PostMessage(hWnd,WM_USER+100,0,0);
//		else {
//			::Sleep(2000);
		if (!showDialog(hWnd)) break;
		bShowDialog = TRUE;
		while (!waitNotify()) ;
//		{
			endDialog();
			bShowDialog = FALSE;
			::DestroyWindow(hWnd);
//		}
		break;

	case WM_DESTROY:
//		endDialog();
		::PostQuitMessage(uninitDll());
		break;

	default:
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 0;
}

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pszCmdLine, int nCmdShow)
{
//	initDll();
#if 0
	static FARPROC lpfnGetDirName = ::GetProcAddress(hModule,"GETDIRNAME");
	LPCSTR ret = (*(LPFNSTRFUNC)lpfnGetDirName)((PVOID)NULL,"フォルダの選択","C:\\hidemaru",1);
	::MessageBox(NULL,ret,NULL,MB_OK);
	return 0;
#endif

#if 1
	WNDCLASS	wc;
	wc.hInstance		=	hInstance;
	wc.hCursor			=	NULL;
	wc.hIcon			=	NULL;
	wc.lpfnWndProc		=	(WNDPROC)MainWndProc;
	wc.style			=	CS_HREDRAW|CS_VREDRAW;
	wc.lpszClassName	=	"DllTestClass32";
	wc.lpszMenuName		=	NULL;
	wc.hbrBackground	=	(HBRUSH)(COLOR_BTNFACE+1);
	wc.cbClsExtra		=	0;
	wc.cbWndExtra		=	0;

	::RegisterClass(&wc);
	HWND	hWnd = ::CreateWindow(
						wc.lpszClassName,
						"Test Dll",
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,CW_USEDEFAULT,
						CW_USEDEFAULT,CW_USEDEFAULT,
						NULL,
						NULL,
						hInstance,
						NULL);
	if (hWnd != NULL) {
		::ShowWindow(hWnd,SW_SHOW);
		::PostMessage(hWnd,WM_USER+100,0,0);
		MSG	msg;
		while (::GetMessage(&msg,NULL,0,0)) {
			if (!bShowDialog) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		return msg.wParam;
	}
	return 1;
#endif
}


