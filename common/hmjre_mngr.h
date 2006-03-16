//	$Id: hmjre_mngr.h,v 1.3 2006-03-16 14:46:56 sugiura Exp $
/*
 *	hmjre_mngr.h
 *	hmjre.dll を利用するためのラッパークラス
 */

#ifndef DENGAKUSERIES_CLASSES_HMJRE_MANAGER
#define DENGAKUSERIES_CLASSES_HMJRE_MANAGER

#include "redll_mngr.h"
#include "hashtbl.h"
#include "linklist.h"
#include "strutils.h"
#include "jreusr.h"

typedef WORD (EXTAPI *PFN_JREGETVERSION)(void);
typedef UINT (EXTAPI *PFN_DECODEESCSEQ)(LPCSTR lpszRe, LPSTR lpszBuff, UINT uiSize);
typedef int (EXTAPI *PFN_GETJREMESSAGE)(int nMessageType, int nLanguage, LPSTR lpszBuff, int cbBuff);
typedef BOOL (EXTAPI *PFN_JRE2OPEN)(LPJRE2 lpJre2);
typedef BOOL (EXTAPI *PFN_JRE2COMPILE)(LPJRE2 lpJre2, LPCSTR lpszRe);
typedef BOOL (EXTAPI *PFN_JRE2GETMATCHINFO)(LPJRE2 lpJre2, LPCSTR lpszStr);
typedef BOOL (EXTAPI *PFN_JRE2CLOSE)(LPJRE2 lpJre2);

#define HMJRE_FILENAME          "hmjre.dll"
#define HMJRE_JREGETVERSION     "JreGetVersion"
#define HMJRE_DECODEESCSEQ      "DecodeEscSeq"
#define HMJRE_GETJREMESSAGE     "GetJreMessage"
#define HMJRE_JRE2OPEN          "Jre2Open"
#define HMJRE_JRE2COMPILE       "Jre2Compile"
#define HMJRE_JRE2GETMATCHINFO  "Jre2GetMatchInfo"
#define HMJRE_JRE2CLOSE         "Jre2Close"

class HmJre_Manager : REDLL_Manager {
public:
	HmJre_Manager(const StringBuffer&);
	~HmJre_Manager();

	DWORD match(const StringBuffer& ptn, const StringBuffer& str, int nFlags);
	StringBuffer replace(const StringBuffer& ptn,
						 const StringBuffer& strSrc,
						 const StringBuffer& strDst,
						 int nFlags);
	const StringBuffer& jreGetVersion();

private:
	PFN_JREGETVERSION m_pfnJreGetVersion;
	PFN_DECODEESCSEQ m_pfnDecodeEscSeq;
	PFN_GETJREMESSAGE m_pfnGetJreMessage;
	PFN_JRE2OPEN m_pfnJre2Open;
	PFN_JRE2COMPILE m_pfnJre2Compile;
	PFN_JRE2GETMATCHINFO m_pfnJre2GetMatchInfo;
	PFN_JRE2CLOSE m_pfnJre2Close;

	HashTable<JRE2*,11> m_htblJre;
	LinkList<JRE2> m_Jre_Pool;
	StringBuffer m_strVersion;

	void setErrorMessage();
};

#endif
