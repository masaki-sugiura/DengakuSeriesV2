//	$Id: misc.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
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

#endif
