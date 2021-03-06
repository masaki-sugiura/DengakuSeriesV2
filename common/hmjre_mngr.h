//	$Id: hmjre_mngr.h,v 1.7 2009/09/20 13:49:01 sugiura Exp $
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
#include "HmJRE/JreFuzzy.h"

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

//	HmJre extended functions
typedef BOOL (WINAPI *PFN_FUZZY_OPEN)( JREFUZZYDATA* pData, BOOL fDummy );
typedef BOOL (WINAPI *PFN_FUZZY_CLOSE)( JREFUZZYDATA* pData );
typedef BOOL (WINAPI *PFN_FUZZY_CONVERTTARGET)( JREFUZZYDATA* pData, const char* pszSrc );
typedef BOOL (WINAPI *PFN_FUZZY_CONVERTFINDSTRING)( JREFUZZYDATA* pData, const char* pszSrc, BOOL fRegular );
typedef int (WINAPI *PFN_FUZZY_FINDPOS2REALPOS)( JREFUZZYDATA* pData, int x );
typedef int (WINAPI *PFN_FUZZY_FINDAREA2REALAREA)( JREFUZZYDATA* pData, int x, int* pcchMatch );
typedef int (WINAPI *PFN_FUZZY_REALPOS2FINDPOS)( JREFUZZYDATA* pData, int x );
typedef JREFUZZYDATA* (WINAPI *PFN_FUZZY_GETFUZZYDATAINJRE)( JRE2* pJre );
typedef BOOL (__cdecl *PFN_FUZZY_OPTIONDIALOG)( HWND hwndParent, UINT flagsDisable );
// Version 1.5から
typedef int (WINAPI *PFN_JREGETTAGPOSITION)( LPJRE2 lpjreJre, char chTagWantGet, int* pcchMatch );
typedef	int (__cdecl *PFN_FINDREGULAR)(LPCSTR pszRE, LPCSTR pszString, int nOffset);
typedef	int (__cdecl *PFN_FINDREGULARNOCASESENSE)(LPCSTR pszRE, LPCSTR pszString, int nOffset);
typedef	int (__cdecl *PFN_GETLASTMATCHLENGTH)();
// Version 1.71から
typedef	int (__cdecl *PFN_ENVCHANGED)();
// Version 1.90から
typedef	int	(__cdecl *PFN_GETLASTMATCHTAGPOSITION)(int nTagNumber);
typedef	int	(__cdecl *PFN_GETLASTMATCHTAGLENGTH)(int nTagNumber);
// Version 1.98から
typedef LPCSTR (__cdecl *PFN_REPLACEREGULAR)(LPCSTR pszPattern, LPCSTR pszTarget, int xStart, LPCSTR pszReplace, BOOL fReplaceAll);
typedef LPCSTR (__cdecl *PFN_REPLACEREGULARNOCASESENSE)(LPCSTR pszPattern, LPCSTR pszTarget, int xStart, LPCSTR pszReplace, BOOL fReplaceAll);

#define	HMJRE_FUZZY_OPEN				"Fuzzy_Open"
#define	HMJRE_FUZZY_CLOSE				"Fuzzy_Close"
#define	HMJRE_FUZZY_CONVERTTARGET		"Fuzzy_ConvertTarget"
#define	HMJRE_FUZZY_CONVERTFINDSTRING	"Fuzzy_ConvertFindString"
#define	HMJRE_FUZZY_FINDPOS2REALPOS		"Fuzzy_FindPos2RealPos"
#define	HMJRE_FUZZY_FINDAREA2REALAREA	"Fuzzy_FindArea2RealArea"
#define	HMJRE_FUZZY_REALPOS2FINDPOS		"Fuzzy_RealPos2FindPos"
#define	HMJRE_FUZZY_GETFUZZYDATAINJRE	"Fuzzy_GetFuzzyDataInJre"
#define	HMJRE_FUZZY_OPTIONDIALOG		"Fuzzy_OptionDialog"
#define	HMJRE_JREGETTAGPOSITION			"JreGetTagPosition"
#define	HMJRE_FINDREGULAR				"FindRegular"
#define	HMJRE_FINDREGULARNOCASESENSE	"FindRegularNoCaseSense"
#define	HMJRE_GETLASTMATCHLENGTH		"GetLastMatchLength"
#define	HMJRE_ENVCHANGED				"EnvChanged"
#define	HMJRE_GETLASTMATCHTAGPOSITION	"GetLastMatchTagPosition"
#define	HMJRE_GETLASTMATCHTAGLENGTH		"GetLastMatchTagLength"
#define	HMJRE_REPLACEREGULAR			"ReplaceRegular"
#define	HMJRE_REPLACEREGULARNOCASESENSE	"ReplaceRegularNoCaseSense"

class HmJre_Manager : REDLL_Manager {
public:
	HmJre_Manager(const StringBuffer&);
	~HmJre_Manager();

	const StringBuffer& jreGetVersion();

	//	return "pos:len"
	StringBuffer	match(const StringBuffer& strFind, const StringBuffer& strTarget, int nOffset, int nFlags, int nRegExp);

	//	return "pos:len"
	StringBuffer	getTagPosition(int nTagNumber);

	StringBuffer	getMatchString(const StringBuffer& strTarget, const StringBuffer& strMatchResult);

	int	findRegular(const StringBuffer& strRE, const StringBuffer& strTarget, int nOffset);
	int	findRegularNoCaseSense(const StringBuffer& strRE, const StringBuffer& strTarget, int nOffset);
	int	getLastMatchLength();
	int	envChanged();
	int	getLastMatchTagPosition(int nTagNumber);
	int	getLastMatchTagLength(int nTagNumber);
	int	optionDialog(int nHwnd, int nDisableFlags);
	StringBuffer	replaceRegular(const StringBuffer& strPattern, const StringBuffer& strTarget, int nStart, const StringBuffer& strReplace, int fReplaceAll);
	StringBuffer	replaceRegularNoCaseSense(const StringBuffer& strPattern, const StringBuffer& strTarget, int nStart, const StringBuffer& strReplace, int fReplaceAll);

private:
	PFN_JREGETVERSION m_pfnJreGetVersion;
	PFN_JREGETTAGPOSITION	m_pfnJreGetTagPosition;
	PFN_JRE2OPEN	m_pfnJre2Open;
	PFN_JRE2CLOSE	m_pfnJre2Close;
	PFN_JRE2COMPILE	m_pfnJre2Compile;
	PFN_JRE2GETMATCHINFO	m_pfnJre2GetMatchInfo;
	PFN_FUZZY_OPEN	m_pfnFuzzy_Open;
	PFN_FUZZY_CLOSE	m_pfnFuzzy_Close;
	PFN_FUZZY_CONVERTTARGET	m_pfnFuzzy_ConvertTarget;
	PFN_FUZZY_CONVERTFINDSTRING	m_pfnFuzzy_ConvertFindString;
	PFN_FUZZY_FINDPOS2REALPOS	m_pfnFuzzy_FindPos2RealPos;
	PFN_FUZZY_FINDAREA2REALAREA	m_pfnFuzzy_FindArea2RealArea;
	PFN_FUZZY_REALPOS2FINDPOS	m_pfnFuzzy_RealPos2FindPos;
	PFN_FUZZY_GETFUZZYDATAINJRE	m_pfnFuzzy_GetFuzzyDataInJre;
	PFN_FUZZY_OPTIONDIALOG	m_pfnFuzzy_OptionDialog;
	PFN_FINDREGULAR	m_pfnFindRegular;
	PFN_FINDREGULARNOCASESENSE	m_pfnFindRegularNoCaseSense;
	PFN_GETLASTMATCHLENGTH	m_pfnGetLastMatchLength;
	PFN_ENVCHANGED	m_pfnEnvChanged;
	PFN_GETLASTMATCHTAGPOSITION	m_pfnGetLastMatchTagPosition;
	PFN_GETLASTMATCHTAGLENGTH	m_pfnGetLastMatchTagLength;
	PFN_REPLACEREGULAR				m_pfnReplaceRegular;
	PFN_REPLACEREGULARNOCASESENSE	m_pfnReplaceRegularNoCaseSense;

	StringBuffer m_strVersion;

	HashTable<JRE2*,11>	m_htblJre2;
	LinkList<JRE2>	m_Jre2_Pool;
	JRE2*	m_pLastJre2;

	JREFUZZYDATA*	m_pLastFuzzyData;

	BOOL	m_bLastIsFuzzy;

	JRE2*	GetJre2(const StringBuffer& strRE, int nCaseSense);
};

#endif
