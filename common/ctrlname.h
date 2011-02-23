//	$Id: ctrlname.h,v 1.1.1.1 2001/10/07 14:41:22 sugiura Exp $
/*
 *	ctrlname.h
 *	コントロール名のテーブル定義
 */

#ifndef	DENGAKUSERIES_CTRLNAME
#define	DENGAKUSERIES_CTRLNAME

#include "strbuf.h"

typedef	enum {
#define	DECLARE_CTRL_CTRL(name)	CTRLID_##name,
#define	DECLARE_CTRL_LAST		CTRLID_UNKNOWN
#define	DECLARE_CTRL_HEADER
#define	DECLARE_CTRL_FOOTER
#include "ctrl.h"
#undef	DECLARE_CTRL_CTRL
#undef	DECLARE_CTRL_LAST
#undef	DECLARE_CTRL_HEADER
#undef	DECLARE_CTRL_FOOTER
} CTRL_ID;

typedef struct {
	int		m_nCtrlNameLength;
	LPCSTR	m_pszCtrlName;
	CTRL_ID	m_ctrlID;
} CTRLSEARCHKEY, *LPCTRLSEARCHKEY;

#endif
