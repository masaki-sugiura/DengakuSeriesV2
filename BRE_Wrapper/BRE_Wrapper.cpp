//	$Id: BRE_Wrapper.cpp,v 1.1 2002-01-16 15:49:15 sugiura Exp $
/*
 *	bs_func.cpp
 *	BRegexp に関するクラス
 */

#include "BRE_Wrapper.h"
#include "BRegExp_Mngr.h"
#include "strutils.h"

#include <commctrl.h>

//	DLL のインスタンスハンドル
Auto_Ptr<BRegExp_Manager> g_pBRegExp_Manager = NULL;

//	LPSTR を返す関数が使用する文字列バッファ
StringBuffer g_strBuffer;

HINSTANCE g_hInstance;

static DWORD
make_dword_from_pos(const StringBuffer& pos)
{
	int sep = pos.find(':');
	if (sep == -1 || sep == 0 || sep >= pos.length() - 1)
		return (DWORD)-1;
	return make_dword(ival(pos.extract(0, sep)),
					  ival(pos.extract(sep + 1, -1)));
}

//	DLL エントリポイント
extern "C" BOOL APIENTRY
DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (g_pBRegExp_Manager.ptr() != NULL) {
			//	有り得ないと思うけど一応
			return FALSE;
		}
		g_hInstance = hInstance;
		try {
			//	LPSTR を返す関数のために文字列バッファを確保
			g_pBRegExp_Manager = new BRegExp_Manager(BREGEXP_FILENAME);
		} catch (...) {
			return FALSE;
		}
		//	よく忘れるので(^^;最初に呼び出しておく
		::InitCommonControls();
		break;

	case DLL_PROCESS_DETACH:
		if (g_pBRegExp_Manager.ptr() == NULL) {
			//	有り得ないと思うけど一応
			return FALSE;
		}
		//	セッションインスタンスの削除
		g_pBRegExp_Manager = NULL;
		g_hInstance = NULL;
		break;

/*
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
*/
    }
    return TRUE;
}

BRE_WRAPPER_API LPCSTR
BRE_ERRORMSG()
{
	g_strBuffer = g_pBRegExp_Manager->getErrorMessage();
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_VERSION()
{
	g_strBuffer = g_pBRegExp_Manager->bRegexpversion();
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_MATCH(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = make_result(g_pBRegExp_Manager->bMatch(pattern,str));
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_SUBST(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = g_pBRegExp_Manager->bSubst(pattern,str);
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_TRANS(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = g_pBRegExp_Manager->bTrans(pattern,str);
	return g_strBuffer;
}

BRE_WRAPPER_API int
BRE_SPLIT(LPCSTR pattern, LPCSTR str, int limit)
{
	return g_pBRegExp_Manager->bSplit(pattern, str, limit);
}

BRE_WRAPPER_API LPCSTR
BRE_GETNEXT()
{
	g_strBuffer = make_result(g_pBRegExp_Manager->getNextResult());
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_POSTOSTR(LPCSTR pstr)
{
	DWORD pos = make_dword_from_pos(pstr);
	if (pos == -1) return nullStr;
	g_strBuffer = g_pBRegExp_Manager->posToString(pos);
	return g_strBuffer;
}

BRE_WRAPPER_API int
BRE_HASMORERESULTS()
{
	return (int)g_pBRegExp_Manager->hasMoreResults();
}

