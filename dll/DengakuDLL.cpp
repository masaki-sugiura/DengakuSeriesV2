//	$Id: DengakuDLL.cpp,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	DengakuDLL.cpp
 *	DLL エントリポイントの定義
 */

#include "DengakuDLL.h"
#include <commctrl.h>

//	DLL のインスタンスハンドル
SessionInstance* g_pSessionInstance;

//	LPSTR を返す関数が使用する文字列バッファのポインタ
StringBuffer* g_pStrBuffer;

//	DLL のバージョン文字列
const StringBuffer g_versionStr = "2.00β";

//	DLL エントリポイント
extern "C" BOOL APIENTRY
DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (g_pSessionInstance != NULL || g_pStrBuffer != NULL) {
			//	有り得ないと思うけど一応
			return FALSE;
		}
		try {
			//	DLL では唯一のセッションインスタンスの構築
			g_pSessionInstance = new SessionInstance(hInstance);

			//	LPSTR を返す関数のために文字列バッファを確保
			g_pStrBuffer = new StringBuffer(nullStr);
		} catch (exception&) {
			return FALSE;
		}
		//	よく忘れるので(^^;最初に呼び出しておく
		::InitCommonControls();
		break;

	case DLL_PROCESS_DETACH:
		if (g_pSessionInstance == NULL || g_pStrBuffer == NULL) {
			//	有り得ないと思うけど一応
			return FALSE;
		}
		//	文字列バッファの削除
		delete g_pStrBuffer;
		g_pStrBuffer = NULL;
		//	セッションインスタンスの削除
		delete g_pSessionInstance;
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

