//	$Id: misc.h,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	misc.h
 *	雑多なユーティリティ関数
 */

#ifndef DENGAKUSERIES_FUNC_MISC
#define DENGAKUSERIES_FUNC_MISC

#include "strbuf.h"

extern BOOL isWinNT();
extern DWORD GetDialogBaseUnits(HWND hDlg, LPCSTR str = "M");
extern COLORREF ColorStrToColorRef(LPCSTR);
extern StringBuffer ColorRefToColorStr(COLORREF);

extern void InternalError(LPCSTR file, int line);

#endif
