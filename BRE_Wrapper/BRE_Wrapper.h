//	$Id: BRE_Wrapper.h,v 1.1 2002-01-16 15:49:15 sugiura Exp $
/*
 *	BRE_Wrapper.h
 *	メインヘッダーファイル
 */

#ifndef BRE_WRAPPER_DLLMAIN
#define BRE_WRAPPER_DLLMAIN

#ifdef BRE_WRAPPER_EXPORTS

#ifdef __cplusplus
#define BRE_WRAPPER_API extern "C" __declspec(dllexport)
#else
#define BRE_WRAPPER_API __declspec(dllexport)
#endif /* __cplusplus */

#else

#ifdef __cplusplus
#define BRE_WRAPPER_API extern "C" __declspec(dllimport)
#else
#define BRE_WRAPPER_API __declspec(dllimport)
#endif /* __cplusplus */

#endif /* BRE_WRAPPER_EXPORTS */

#endif
