//	$Id: dlltest.cpp,v 1.9 2009/09/20 13:49:01 sugiura Exp $
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

BOOL
uninitDll()
{
	if (hModule != NULL) ::FreeLibrary(hModule);
	return TRUE;
}

typedef LPCSTR (*LPFNSTRFUNC)(PVOID,...);
typedef int	(*LPFNINTFUNC)(PVOID,...);
typedef LPCSTR (*LPFNSTRFUNC_NOARG)();
typedef int (*LPFNINTFUNC_NOARG)();

#if 0
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
#endif

#include <iostream>
#include <assert.h>

using namespace std;

#define DENGAKUDLL_API extern "C" __declspec(dllimport)

DENGAKUDLL_API LPCSTR GETDIRNAME(int, LPCSTR, LPCSTR);

DENGAKUDLL_API LPCSTR GETDRIVES();
DENGAKUDLL_API LPCSTR GETLONGNAME(LPCSTR);
DENGAKUDLL_API int    SETCURDIR(LPCSTR);
DENGAKUDLL_API LPCSTR GETCURDIR();
DENGAKUDLL_API int    MKDIR(LPCSTR);
DENGAKUDLL_API int    RMDIR(LPCSTR);
DENGAKUDLL_API int    COPY(LPCSTR, LPCSTR);
DENGAKUDLL_API int    MOVE(LPCSTR, LPCSTR);
DENGAKUDLL_API int    REMOVE(LPCSTR);
DENGAKUDLL_API int    TOUCH(LPCSTR);
DENGAKUDLL_API int    RUN(LPCSTR);
DENGAKUDLL_API int    ENUMFILE(LPCSTR);
DENGAKUDLL_API int    ENUMDIR(LPCSTR);
DENGAKUDLL_API int    ENUMPATH(LPCSTR);
DENGAKUDLL_API LPCSTR FINDNEXT();
DENGAKUDLL_API LPCSTR SIZEOF(LPCSTR);
DENGAKUDLL_API LPCSTR TIMESTAMPOF(LPCSTR);
DENGAKUDLL_API LPCSTR TIMECOUNTOF(LPCSTR);
DENGAKUDLL_API LPCSTR ATTRIBUTEOF(LPCSTR);

DENGAKUDLL_API int    STRICMP(LPCSTR, LPCSTR);
DENGAKUDLL_API int    STRCOUNT(LPCSTR, LPCSTR);
DENGAKUDLL_API LPCSTR GSUB(LPCSTR, LPCSTR, LPCSTR, int);
DENGAKUDLL_API LPCSTR GETTOKEN(LPCSTR, LPCSTR);
DENGAKUDLL_API int    HASMORETOKENS();
DENGAKUDLL_API LPCSTR LTRIM(LPCSTR);
DENGAKUDLL_API LPCSTR RTRIM(LPCSTR);
DENGAKUDLL_API int    STRLEN(LPCSTR);
DENGAKUDLL_API int    STRLEN2(LPCSTR);
DENGAKUDLL_API int    STRSTR(LPCSTR, LPCSTR);
DENGAKUDLL_API int    STRSTR2(LPCSTR, LPCSTR);
DENGAKUDLL_API int    STRRSTR(LPCSTR, LPCSTR);
DENGAKUDLL_API int    STRRSTR2(LPCSTR, LPCSTR);
DENGAKUDLL_API LPCSTR LEFTSTR(LPCSTR, int);
DENGAKUDLL_API LPCSTR LEFTSTR2(LPCSTR, int);
DENGAKUDLL_API LPCSTR RIGHTSTR(LPCSTR, int);
DENGAKUDLL_API LPCSTR RIGHTSTR2(LPCSTR, int);
DENGAKUDLL_API LPCSTR MIDSTR(LPCSTR, int, int);
DENGAKUDLL_API LPCSTR MIDSTR2(LPCSTR, int, int);
DENGAKUDLL_API LPCSTR TOLOWER(LPCSTR);
DENGAKUDLL_API LPCSTR TOLOWER2(LPCSTR);
DENGAKUDLL_API LPCSTR TOUPPER(LPCSTR);
DENGAKUDLL_API LPCSTR TOUPPER2(LPCSTR);

DENGAKUDLL_API int NEWDIALOG(LPCSTR, int, LPCSTR);
DENGAKUDLL_API int NEWCONTROL(LPCSTR, LPCSTR, LPCSTR);
DENGAKUDLL_API int SHOWDIALOG(int, int);
DENGAKUDLL_API int WAITCTRLNOTIFY(int);
DENGAKUDLL_API int ENDDIALOG();

DENGAKUDLL_API int BRE_LOAD(LPCSTR);
DENGAKUDLL_API LPCSTR BRE_SUBST(LPCSTR, LPCSTR);
DENGAKUDLL_API LPCSTR BRE_TRANS(LPCSTR, LPCSTR);
DENGAKUDLL_API int BRE_FREE();

#define NO_OUTPUT
#define PROF_FILE
//#define PROF_STR

static void
SHOW_FINDNEXT_RESULTS()
{
#ifndef NO_OUTPUT
	cout << "result of FINDNEXT():\n";
#endif
	for (;;) {
		LPCSTR str = FINDNEXT();
		if (lstrlen(str) == 0) break;
#ifndef NO_OUTPUT
		cout << str << '\t';
		cout << SIZEOF("") << '\t';
		cout << TIMESTAMPOF("") << '\t';
		cout << TIMECOUNTOF("") << '\t';
		cout << ATTRIBUTEOF("") << '\n';
#endif
	}
}

static void
assert_str(const char* expr_str, const char* ret_str, const char* valid,
		   const char* file, int line)
{
	if (lstrcmp(ret_str, valid) != 0) {
		cerr << "assertion failed: " << file << ":(" << line << ")\n";
		cerr << "result of " << expr_str << " should be [" << valid << "], "
			 << "but result is [" << ret_str << "]\n";
		abort();
	}
	cout << "result of " << expr_str << ": [" << ret_str << "]\n";
}

static void
assert_num(const char* expr_str, const char* op_str, int ret_num, int valid, bool check,
		   const char* file, int line)
{
	if (!check) {
		cerr << "assertion failed: " << file << ":(" << line << ")\n";
		cerr << "result of " << expr_str << " should be " << op_str << " [" << valid << "], "
			 << "but result is [" << ret_num << "]\n";
		abort();
	}
	cout << "result of " << expr_str << ": [" << ret_num << "]\n";
}

static void
CREATE_FILE(LPCSTR filename)
{
	HANDLE hFile = ::CreateFile(filename, GENERIC_WRITE, 0, NULL,
								OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	FILETIME ft;
	::GetSystemTimeAsFileTime(&ft);
	::SetFileTime(hFile, NULL, NULL, &ft);
	::CloseHandle(hFile);
}

#define ASSERT(expr) \
	do { \
		if (!(expr)) { \
			cerr << "assertion failed: " << __FILE__ << ":(" << __LINE__ << ")\n"; \
			abort(); \
		} \
	} while (0)

#ifndef NO_OUTPUT

#define ASSERT_STR(EXPR,VALID) assert_str(#EXPR, EXPR, VALID, __FILE__, __LINE__)
#define ASSERT_NUM(EXPR,OP,VALID) \
	do { \
		int ret_num = EXPR; \
		assert_num(#EXPR, #OP, ret_num, VALID, ret_num OP VALID, \
				   __FILE__, __LINE__); \
	} while (0)

#else

#define ASSERT_STR(EXPR,VALID) EXPR
#define ASSERT_NUM(EXPR,OP,VALID) EXPR

#endif

#include <string>
using std::string;

void enumtest(const char* parent)
{
	int n = ENUMDIR((string(parent) + "\\*.*").c_str());
	if (n == 0) {
		MessageBox(NULL, NULL, "Error: 検索失敗", MB_OK);
	} else if (n == 1) {
		const char* child = FINDNEXT();
		while (strlen(child) != 0) {
//			MessageBox("$$parent = [" + $$parent + "]\n$$child = [" + $$child + "]");
			child = FINDNEXT();
		}
	} else {
//		message("Error: 不明");
	}
}


#ifdef _WINDOWS
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pszCmdLine, int nCmdShow)
#else
int main(int ac, char** av)
#endif
{
//	initDll();
#if 1
#if 0
	static FARPROC lpfnGetDirName = ::GetProcAddress(hModule,"GETDIRNAME");
	LPCSTR ret = (*(LPFNSTRFUNC)lpfnGetDirName)((PVOID)NULL,"フォルダの選択","C:\\hidemaru",1);
	::MessageBox(NULL,ret,NULL,MB_OK);
	return 0;

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

//	ASSERT(initDll());
#ifdef PROF_FILE

#ifndef NO_OUTPUT
	cout << "result of GETDRIVES(): "
		 << GETDRIVES()
		 << '\n';

	cout << "result of GETLONGNAME(C:\\PROGRA~1\\WINDOW~2): "
		 << GETLONGNAME("C:\\PROGRA~1\\WINDOW~2")
		 << '\n';
#else
	static TCHAR buf[MAX_PATH];
	LPCSTR str = GETDRIVES();
	lstrcpy(buf, str);
	str = GETLONGNAME("C:\\PROGRA~1\\WINDOW~2");
	lstrcpy(buf, str);
#endif

	RUN("\"D:\\Program Files\\CHOCOA\\license.txt\"");

	ASSERT_NUM(SETCURDIR("C:\\usertemp"), ==, 1);
	ASSERT_STR(GETCURDIR(), "C:\\usertemp");

#ifndef NO_OUTPUT
	cout << "result of TOUCH(foo1.txt): "
		 << TOUCH("foo1.txt")
		 << '\n';
#else
	TOUCH("foo1.txt");
#endif

#if 0
	ASSERT_NUM(MKDIR("foodir1\\foodir2"), ==, 0);
	ASSERT_NUM(MKDIR("-p foodir1\\foodir2"), ==, 1);

	ASSERT_NUM(COPY("foo1.txt", "foo2.txt"), ==, 1);
	ASSERT_NUM(COPY("foo2.txt", "foodir1"), ==, 1);
	ASSERT_NUM(COPY("foodir1", "foodir2"), ==, 0);
	ASSERT_NUM(COPY("-r foodir1", "foodir2"), ==, 1);

	ASSERT_NUM(MOVE("foo1.txt", "foo3.txt"), ==, 1);
	ASSERT_NUM(MOVE("foo3.txt", "foodir1"), ==, 1);
	ASSERT_NUM(MOVE("foodir2", "foodir3"), ==, 1);
#endif

#if 0
//	ASSERT_NUM(ENUMFILE("C:\\*.*"), ==, 1);
//	SHOW_FINDNEXT_RESULTS();
	ASSERT_NUM(ENUMFILE("C:\\temp\\*.txt"), ==, 1);
	SHOW_FINDNEXT_RESULTS();

//	ASSERT_NUM(ENUMDIR("C:\\*.*"), ==, 1);
//	SHOW_FINDNEXT_RESULTS();
	ASSERT_NUM(ENUMDIR("C:\\temp\\*.*"), ==, 1);
	SHOW_FINDNEXT_RESULTS();

//	ASSERT_NUM(ENUMPATH("-f C:\\*.doc"), ==, 1);
//	SHOW_FINDNEXT_RESULTS();
	ASSERT_NUM(ENUMPATH("C:\\temp\\*.txt"), ==, 1);
	SHOW_FINDNEXT_RESULTS();
#endif

#if 0
	enumtest("C:\\temp\\work\\00_Folder");
	enumtest("C:\\temp\\work\\01 Folder");
	enumtest("C:\\temp\\work\\02,Folder");
#endif

#if 0
	ASSERT_NUM(RMDIR("foodir1"), ==, 0);
	ASSERT_NUM(RMDIR("foodir1\\foodir2"), ==, 1);

	ASSERT_NUM(REMOVE("foodir1"), ==, 0);
	ASSERT_NUM(REMOVE("foodir1\\foo?.txt"), ==, 1);
	ASSERT_NUM(REMOVE("-r foodir?"), ==, 1);
	ASSERT_NUM(REMOVE("foo?.txt"), ==, 1);
#endif

#endif

#ifdef PROF_STR

	for (int i = 0; i < 1000; i++) {

	// STRICMP
	ASSERT_NUM(STRICMP("", ""), ==, 0);
	ASSERT_NUM(STRICMP("a", ""), >, 0);
	ASSERT_NUM(STRICMP("", "a"), <, 0);
	ASSERT_NUM(STRICMP("a", "a"), ==, 0);
	ASSERT_NUM(STRICMP("Ａ", "A"), !=, 0);

	// STRCOUNT
	ASSERT_NUM(STRCOUNT("", ""), ==, 0);
	ASSERT_NUM(STRCOUNT("a", "a"), ==, 1);
	ASSERT_NUM(STRCOUNT("表", "\\"), ==, 0);
	ASSERT_NUM(STRCOUNT("aaaaa", "aa"), ==, 2);
	ASSERT_NUM(STRCOUNT("a", "aa"), ==, 0);

	// GSUB
	ASSERT_STR(GSUB("", "", "a", -1), "");
	ASSERT_STR(GSUB("", "a", "b", -1), "");
	ASSERT_STR(GSUB("aaaaa","aa","b",-1), "bba");
	ASSERT_STR(GSUB("aaaaa","aa","b",1), "baaa");
	ASSERT_STR(GSUB("aaaaa","aa","b",0), "aaaaa");
	ASSERT_STR(GSUB("aaa","a","bb",2), "bbbba");

	// GETTOKEN
	ASSERT_STR(GETTOKEN(":b::c:",":"), "");
	ASSERT_STR(GETTOKEN("",":"), "b");
	ASSERT_STR(GETTOKEN("",":"), "");
	ASSERT_STR(GETTOKEN("",":"), "c");
	ASSERT_NUM(HASMORETOKENS(), ==, 1);
	ASSERT_STR(GETTOKEN("",":"), "");
	ASSERT_NUM(HASMORETOKENS(), ==, 0);
	ASSERT_STR(GETTOKEN("",":"), "");
	ASSERT_NUM(HASMORETOKENS(), ==, 0);

	// LTRIM
	ASSERT_STR(LTRIM("  a "), "a ");
	ASSERT_STR(LTRIM("　a "), "　a ");
	ASSERT_STR(LTRIM("  "), "");

	// RTRIM
	ASSERT_STR(RTRIM("  a "), "  a");
	ASSERT_STR(RTRIM(" a　"), " a　");
	ASSERT_STR(RTRIM("  "), "");

	// STRLEN
	ASSERT_NUM(STRLEN(""), ==, 0);
	ASSERT_NUM(STRLEN("a"), ==, 1);
	ASSERT_NUM(STRLEN("あ"), ==, 2);
	ASSERT_NUM(STRLEN("あa"), ==, 3);

	// STRLEN2
	ASSERT_NUM(STRLEN2(""), ==, 0);
	ASSERT_NUM(STRLEN2("a"), ==, 1);
	ASSERT_NUM(STRLEN2("あ"), ==, 1);
	ASSERT_NUM(STRLEN2("あa"), ==, 2);

	// STRSTR
	ASSERT_NUM(STRSTR("",""), ==, -1);
	ASSERT_NUM(STRSTR("","a"), ==, -1);
	ASSERT_NUM(STRSTR("a","a"), ==, 0);
	ASSERT_NUM(STRSTR("あa","a"), ==, 2);

	// STRSTR2
	ASSERT_NUM(STRSTR2("",""), ==, -1);
	ASSERT_NUM(STRSTR2("","a"), ==, -1);
	ASSERT_NUM(STRSTR2("a","a"), ==, 0);
	ASSERT_NUM(STRSTR2("あa","a"), ==, 1);

	// STRRSTR
	ASSERT_NUM(STRRSTR("",""), ==, -1);
	ASSERT_NUM(STRRSTR("","a"), ==, -1);
	ASSERT_NUM(STRRSTR("a","a"), ==, 0);
	ASSERT_NUM(STRRSTR("あaa","a"), ==, 3);

	// STRRSTR2
	ASSERT_NUM(STRRSTR2("",""), ==, -1);
	ASSERT_NUM(STRRSTR2("","a"), ==, -1);
	ASSERT_NUM(STRRSTR2("a","a"), ==, 0);
	ASSERT_NUM(STRRSTR2("あaa","a"), ==, 2);

	// LEFTSTR
	ASSERT_STR(LEFTSTR("",1), "");
	ASSERT_STR(LEFTSTR("aa",0), "");
	ASSERT_STR(LEFTSTR("aa",1), "a");
	ASSERT_STR(LEFTSTR("aa",2), "aa");
	ASSERT_STR(LEFTSTR("aa",3), "aa");
	ASSERT_STR(LEFTSTR("あa",2), "あ");

	// LEFTSTR2
	ASSERT_STR(LEFTSTR2("",1), "");
	ASSERT_STR(LEFTSTR2("aa",0), "");
	ASSERT_STR(LEFTSTR2("aa",1), "a");
	ASSERT_STR(LEFTSTR2("aa",2), "aa");
	ASSERT_STR(LEFTSTR2("aa",3), "aa");
	ASSERT_STR(LEFTSTR2("あa",2), "あa");

	// RIGHTSTR
	ASSERT_STR(RIGHTSTR("",1), "");
	ASSERT_STR(RIGHTSTR("aa",0), "");
	ASSERT_STR(RIGHTSTR("aa",1), "a");
	ASSERT_STR(RIGHTSTR("aa",2), "aa");
	ASSERT_STR(RIGHTSTR("aa",3), "aa");
	ASSERT_STR(RIGHTSTR("aあ",2), "あ");

	// RIGHTSTR2
	ASSERT_STR(RIGHTSTR2("",1), "");
	ASSERT_STR(RIGHTSTR2("aa",0), "");
	ASSERT_STR(RIGHTSTR2("aa",1), "a");
	ASSERT_STR(RIGHTSTR2("aa",2), "aa");
	ASSERT_STR(RIGHTSTR2("aa",3), "aa");
	ASSERT_STR(RIGHTSTR2("aあ",2), "aあ");

	// MIDSTR
	ASSERT_STR(MIDSTR("",0,0), "");
	ASSERT_STR(MIDSTR("",0,1), "");
	ASSERT_STR(MIDSTR("",1,0), "");
	ASSERT_STR(MIDSTR("",1,1), "");
	ASSERT_STR(MIDSTR("aa",0,1), "a");
	ASSERT_STR(MIDSTR("aa",1,1), "a");
	ASSERT_STR(MIDSTR("aa",1,2), "a");
	ASSERT_STR(MIDSTR("aa",2,1), "");
	ASSERT_STR(MIDSTR("あaa",2,1), "a");
	ASSERT_STR(MIDSTR("あaa",3,1), "a");

	// MIDSTR2
	ASSERT_STR(MIDSTR2("",0,0), "");
	ASSERT_STR(MIDSTR2("",0,1), "");
	ASSERT_STR(MIDSTR2("",1,0), "");
	ASSERT_STR(MIDSTR2("",1,1), "");
	ASSERT_STR(MIDSTR2("aa",0,1), "a");
	ASSERT_STR(MIDSTR2("aa",1,1), "a");
	ASSERT_STR(MIDSTR2("aa",1,2), "a");
	ASSERT_STR(MIDSTR2("aa",2,1), "");
	ASSERT_STR(MIDSTR2("あaa",2,1), "a");
	ASSERT_STR(MIDSTR2("あaa",3,1), "");

	// TOLOWER
	ASSERT_STR(TOLOWER(""), "");
	ASSERT_STR(TOLOWER("aa"), "aa");
	ASSERT_STR(TOLOWER("Aa"), "aa");
	ASSERT_STR(TOLOWER("AＡ"), "aＡ");

	// TOLOWER2
	ASSERT_STR(TOLOWER2(""), "");
	ASSERT_STR(TOLOWER2("aa"), "aa");
	ASSERT_STR(TOLOWER2("Aa"), "aa");
	ASSERT_STR(TOLOWER2("AＡ"), "aａ");

	// TOUPPER
	ASSERT_STR(TOUPPER(""), "");
	ASSERT_STR(TOUPPER("AA"), "AA");
	ASSERT_STR(TOUPPER("Aa"), "AA");
	ASSERT_STR(TOUPPER("aａ"), "Aａ");

	// TOUPPER2
	ASSERT_STR(TOUPPER2(""), "");
	ASSERT_STR(TOUPPER2("AA"), "AA");
	ASSERT_STR(TOUPPER2("Aa"), "AA");
	ASSERT_STR(TOUPPER2("aａ"), "AＡ");

	} // for

#endif
//	ASSERT(uninitDll());
#endif

#if 0
	NEWDIALOG("Dialog Test", 40, "");
	NEWCONTROL("text","","ダイアログテスト");
//	NEWCONTROL("edit","","");
	SHOWDIALOG(0, 1);
	WAITCTRLNOTIFY(1000);
	ENDDIALOG();
#endif

//	LPCSTR pszDirName = GETDIRNAME(0, "test", "\\\\topquark\\smbshare");
//	MessageBox(NULL, pszDirName, NULL, MB_OK);

#if 0
	BRE_LOAD("C:\\Program Files\\hidemaru\\bregexp.dll");

	BRE_SUBST("s/ひ/に/gk", "にゃあ");
	BRE_TRANS("tr/ひ/に/gk", "にゃあ");

	BRE_FREE();
#endif

	int	nRet = ENUMFILE("/o n c:\\temp\\*.*");

	return 0;
}


