//	$Id: common.h,v 1.2 2007-03-05 13:04:54 sugiura Exp $
/*
 *	common.h
 *	その他サーバ特有の定義
 */

#ifndef DENGAKUSERVER_COMMON_DEF
#define DENGAKUSERVER_COMMON_DEF

#include "str_tbl.h"

#define	WM_USER_DDE_START		(WM_USER_MAX + 1)
#define	WM_USER_DDE_STOP		(WM_USER_MAX + 2)
#define WM_USER_MENU			(WM_USER_MAX + 3)
#define	WM_USER_SHOWICON		(WM_USER_MAX + 4)
#define	WM_USER_NOTIFYPOSTADV	(WM_USER_MAX + 5)
#define WM_USER_SHOWCOMDLG		(WM_USER_MAX + 6)

#define	MSG_ERR_START		0
#define	MSG_ERR_INIT		1
#define	MSG_ERR_STOP		2
#define	MSG_ERR_CREATEDLG	3

#define SVR_STR_UNKNOWN			0
#define	SVR_STR_VERSION			1
#define	SVR_STR_SERVERNAME		2
#define	SVR_STR_DSSERVICENAME	3
#define	SVR_STR_MSSERVICENAME	4
#define	SVR_STR_MSEXENAME		5
#define SVR_STR_DSCLASSNAME		6
#define SVR_STR_DSMUTEXNAME		7
#define SVR_STR_TOPICNAMES		8
#define SVR_STR_MAXINDEX		9

extern const StringBuffer existStr;

extern LPCSTR svr_strings[];
extern LPCSTR msg_strings[];

inline LPCSTR
GetServerString(WORD msgid)
{
	return msgid < SVR_STR_MAXINDEX ? svr_strings[msgid] :
									svr_strings[SVR_STR_UNKNOWN];
}

inline int
ErrorMessageBox(WORD msgid)
{
	return ::MessageBox(NULL,
						msg_strings[msgid],
						"田楽サーバ：エラー",
						MB_OK|MB_ICONERROR);
}

#ifdef	_DEBUG
DWORD	OutputErrorMsgToDebugTerminal(LPCSTR);
#define	DEBUGOUTPUT	OutputErrorMsgToDebugTerminal
#endif

#endif

