//	$Id: DengakuDLL.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	DengakuDLL.h
 *	メインヘッダーファイル
 */

#ifndef DENGAKUDLL_DLLMAIN
#define DENGAKUDLL_DLLMAIN

#include "session.h"

// 以下の ifdef ブロックは DLL から簡単にエクスポートさせるマクロを作成する標準的な方法です。 
// この DLL 内のすべてのファイルはコマンドラインで定義された DENGAKUDLL_EXPORTS シンボル
// でコンパイルされます。このシンボルはこの DLL が使用するどのプロジェクト上でも未定義でなけ
// ればなりません。この方法ではソースファイルにこのファイルを含むすべてのプロジェクトが DLL 
// からインポートされたものとして DENGAKUDLL_API 関数を参照し、そのためこの DLL はこのマク 
// ロで定義されたシンボルをエクスポートされたものとして参照します。
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
extern SessionInstance*	g_pSessionInstance;
//	LPSTR を返す関数が使用する文字列バッファへのポインタ
extern class StringBuffer *g_pStrBuffer;
//	DLL のバージョン文字列
extern const StringBuffer g_versionStr;

#endif
