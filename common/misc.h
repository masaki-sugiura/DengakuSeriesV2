//	$Id: misc.h,v 1.3 2002-02-15 17:46:08 sugiura Exp $
/*
 *	misc.h
 *	雑多なユーティリティ関数
 */

#ifndef DENGAKUSERIES_FUNC_MISC
#define DENGAKUSERIES_FUNC_MISC

#include "strbuf.h"

extern BOOL isWinNT();
extern DWORD GetDialogBaseUnits(HWND hDlg, LPCSTR str = "M");

extern void InternalError(LPCSTR file, int line);

#endif
