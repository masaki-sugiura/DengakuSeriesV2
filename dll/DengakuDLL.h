//	$Id: DengakuDLL.h,v 1.2 2002/01/16 16:31:04 sugiura Exp $
/*
 *	DengakuDLL.h
 *	メインヘッダーファイル
 */

#ifndef DENGAKUDLL_DLLMAIN
#define DENGAKUDLL_DLLMAIN

#include "session.h"

#ifdef DENGAKUDLL_EXPORTS

#ifdef __cplusplus
#define DENGAKUDLL_API extern "C" __declspec(dllexport)
#else
#define DENGAKUDLL_API __declspec(dllexport)
#endif /* __cplusplus */

#else

#ifdef __cplusplus
#define DENGAKUDLL_API extern "C" __declspec(dllimport)
#else
#define DENGAKUDLL_API __declspec(dllimport)
#endif /* __cplusplus */

#endif /* DENGAKUDLL_EXPORTS */

//	int 版と double 版の両方でコードを再利用するためのトリック
#ifndef HIDEDLL_DOUBLE
typedef int HIDEDLL_NUMTYPE;
#else
typedef double HIDEDLL_NUMTYPE;
#endif

#define HIDEDLL_TRUE  static_cast<HIDEDLL_NUMTYPE>(TRUE)
#define HIDEDLL_FALSE static_cast<HIDEDLL_NUMTYPE>(FALSE)

//	DLL では唯一のセッションインスタンスへのポインタ
extern Auto_Ptr<SessionInstance> g_pSessionInstance;
//	LPSTR を返す関数が使用する文字列バッファへのポインタ
extern StringBuffer g_strBuffer;
//	DLL のバージョン文字列
extern const StringBuffer g_versionStr;

#endif
