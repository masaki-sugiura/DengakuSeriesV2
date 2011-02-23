//	$Id: cmdname.cpp,v 1.2 2006/03/16 14:46:56 sugiura Exp $
/*
 *	cmdname.cpp
 *	コマンド名検索テーブル
 */

#include "cmdname.h"
#include "ddeserv.h"
#include "convdata.h"

#define	DECLARE_DDECMD_HEADER		CSKEY_EXECUTE execcmd[] = {
#define	DECLARE_DDECMD_EXECUTE(name)	{sizeof(#name)-1,#name,&ConvData::On_##name},
#define	DECLARE_DDECMD_REQUEST(name)	
#define	DECLARE_DDECMD_POKE(name)		
#define	DECLARE_DDECMD_FINAL			{0,"unknown",NULL}
#define	DECLARE_DDECMD_FOOTER		};
#include "cmd.h"
#undef	DECLARE_DDECMD_HEADER
#undef	DECLARE_DDECMD_EXECUTE
#undef	DECLARE_DDECMD_REQUEST
#undef	DECLARE_DDECMD_POKE
#undef	DECLARE_DDECMD_FINAL
#undef	DECLARE_DDECMD_FOOTER

#define	DECLARE_DDECMD_HEADER		CSKEY_REQUEST reqcmd[] = {
#define	DECLARE_DDECMD_EXECUTE(name)	
#define	DECLARE_DDECMD_REQUEST(name)	{sizeof(#name)-1,#name,&ConvData::On_##name},
#define	DECLARE_DDECMD_POKE(name)		
#define	DECLARE_DDECMD_FINAL			{0,"unknown",NULL}
#define	DECLARE_DDECMD_FOOTER		};
#include "cmd.h"
#undef	DECLARE_DDECMD_HEADER
#undef	DECLARE_DDECMD_EXECUTE
#undef	DECLARE_DDECMD_REQUEST
#undef	DECLARE_DDECMD_POKE
#undef	DECLARE_DDECMD_FINAL
#undef	DECLARE_DDECMD_FOOTER

#define	DECLARE_DDECMD_HEADER		CSKEY_POKE pokecmd[] = {
#define	DECLARE_DDECMD_EXECUTE(name)	
#define	DECLARE_DDECMD_REQUEST(name)	
#define	DECLARE_DDECMD_POKE(name)		{sizeof(#name)-1,#name,&ConvData::On_##name},
#define	DECLARE_DDECMD_FINAL			{0,"unknown",NULL}
#define	DECLARE_DDECMD_FOOTER		};
#include "cmd.h"
#undef	DECLARE_DDECMD_HEADER
#undef	DECLARE_DDECMD_EXECUTE
#undef	DECLARE_DDECMD_REQUEST
#undef	DECLARE_DDECMD_POKE
#undef	DECLARE_DDECMD_FINAL
#undef	DECLARE_DDECMD_FOOTER

PFNDDECMD_EXECUTE
DdeServer::getExecCmd(const StringBuffer& cmdname)
{
	int	len = cmdname.length();
	if (len <= 0) return NULL;
	for (int i = 0; /* no condition */; i++) {
		if (execcmd[i].m_nCmdLength == 0) break;
		else if (execcmd[i].m_nCmdLength == len	&&
				lstrcmpn((LPCSTR)cmdname,execcmd[i].m_pszCmdName,len) == 0)
			return execcmd[i].m_pfnCmdHandler;
	}
	return NULL;
}

PFNDDECMD_REQUEST
DdeServer::getReqCmd(const StringBuffer &cmdname)
{
	int	len = cmdname.length();
	if (len <= 0) return NULL;
	for (int i=0; /* no condition */; i++) {
		if (reqcmd[i].m_nCmdLength == 0) break;
		else if (reqcmd[i].m_nCmdLength == len	&&
				lstrcmpn((LPCSTR)cmdname,reqcmd[i].m_pszCmdName,len) == 0)
			return reqcmd[i].m_pfnCmdHandler;
	}
	return NULL;
}

PFNDDECMD_POKE
DdeServer::getPokeCmd(const StringBuffer &cmdname)
{
	int	len = cmdname.length();
	if (len <= 0) return NULL;
	for (int i=0; /* no condition */; i++) {
		if (pokecmd[i].m_nCmdLength == 0) break;
		else if (pokecmd[i].m_nCmdLength == len	&&
				lstrcmpn((LPCSTR)cmdname,pokecmd[i].m_pszCmdName,len) == 0)
			return pokecmd[i].m_pfnCmdHandler;
	}
	return NULL;
}

