//	$Id: DengakuDLL.cpp,v 1.25 2003-03-11 15:47:30 sugiura Exp $
/*
 *	DengakuDLL.cpp
 *	DLL エントリポイントの定義
 */

#include "DengakuDLL.h"
#include "auto_ptr.h"

#include <commctrl.h>

//	DLL のインスタンスハンドル
Auto_Ptr<SessionInstance> g_pSessionInstance(NULL);

//	LPSTR を返す関数が使用する文字列バッファのポインタ
StringBuffer g_strBuffer;

//	DLL のバージョン文字列
const StringBuffer g_versionStr = "2.51";

//	DLL エントリポイント
extern "C" BOOL APIENTRY
DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (g_pSessionInstance.ptr() != NULL) {
			//	有り得ないと思うけど一応
			return FALSE;
		}
		try {
			//	DLL では唯一のセッションインスタンスの構築
			g_pSessionInstance = new SessionInstance(hInstance);
		} catch (...) {
			return FALSE;
		}
		//	よく忘れるので(^^;最初に呼び出しておく
		::InitCommonControls();
		break;

	case DLL_PROCESS_DETACH:
		if (g_pSessionInstance.ptr() == NULL) {
			//	有り得ないと思うけど一応
			return FALSE;
		}
		//	セッションインスタンスの削除
		g_pSessionInstance = NULL;
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

