//	$Id: DengakuDLL.h,v 1.2 2002/01/16 16:31:04 sugiura Exp $
/*
 *	DengakuDLL.h
 *	メインヘッダーファイル
 */

#ifndef DENGAKUDLL_DLLMAIN
#define DENGAKUDLL_DLLMAIN

#include "session.h"

//#ifdef _WIN64
#if 0
#define DLL_EXPORT
#define	DLL_IMPORT
#else
#define DLL_EXPORT	__declspec(dllexport)
#define	DLL_IMPORT	__declspec(dllimport)
#endif

#ifdef DENGAKUDLL_EXPORTS

#ifdef __cplusplus
#define DENGAKUDLL_API extern "C" DLL_EXPORT
#else
#define DENGAKUDLL_API DLL_EXPORT
#endif /* __cplusplus */

#else

#ifdef __cplusplus
#define DENGAKUDLL_API extern "C" DLL_IMPORT
#else
#define DENGAKUDLL_API DLL_IMPORT
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
