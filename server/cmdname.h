//	$Id: cmdname.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	cmdname.h
 *	コマンド名検索テーブルのための定義
 */

#ifndef	DENGAKUSERVER_CMDNAME
#define	DENGAKUSERVER_CMDNAME

#include "convdata.h"

class CmdLineParser;

typedef struct {
	int m_nCmdLength;
	LPCSTR m_pszCmdName;
	PFNDDECMD_EXECUTE m_pfnCmdHandler;
} CSKEY_EXECUTE, *LPCSKEY_EXECUTE;

typedef struct {
	int m_nCmdLength;
	LPCSTR m_pszCmdName;
	PFNDDECMD_REQUEST m_pfnCmdHandler;
} CSKEY_REQUEST, *LPCSKEY_REQUEST;

typedef struct {
	int m_nCmdLength;
	LPCSTR m_pszCmdName;
	PFNDDECMD_POKE m_pfnCmdHandler;
} CSKEY_POKE, *LPCSKEY_POKE;

typedef	enum {
#define	DECLARE_DDECMD_EXECUTE(name)	CMDID_##name,
#define	DECLARE_DDECMD_REQUEST(name)	CMDID_##name,
#define	DECLARE_DDECMD_POKE(name)		CMDID_##name,
#define	DECLARE_DDECMD_FINAL			CMDID_UNKNWON
#define	DECLARE_DDECMD_HEADER
#define	DECLARE_DDECMD_FOOTER
#include "cmd.h"
#undef	DECLARE_DDECMD_EXECUTE
#undef	DECLARE_DDECMD_REQUEST
#undef	DECLARE_DDECMD_POKE
#undef	DECLARE_DDECMD_FINAL
#undef	DECLARE_DDECMD_HEADER
#undef	DECLARE_DDECMD_FOOTER
} CMD_ID;

#endif

