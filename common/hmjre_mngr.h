//	$Id: hmjre_mngr.h,v 1.1 2003-12-03 17:17:58 sugiura Exp $
/*
 *	hmjre_mngr.h
 *	hmjre.dll を利用するためのラッパークラス
 */

#ifndef DENGAKUSERIES_CLASSES_HMJRE_MANAGER
#define DENGAKUSERIES_CLASSES_HMJRE_MANAGER

#include "hashtbl.h"
#include "linklist.h"
#include "auto_ptr.h"
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

#define HMJRE_RESULT_FAILED (DWORD)-1

inline StringBuffer
make_result(DWORD res)
{
	if (res == HMJRE_RESULT_FAILED) return nullStr;
	return StringBuffer(16).append((DWORD)LOWORD(res))
						   .append((TCHAR)':')
						   .append((DWORD)HIWORD(res));
}

inline DWORD
make_dword(int low, int high)
{
	return (DWORD)((USHORT)low | (high << 16));
}

inline DWORD
make_dword_from_pos(const StringBuffer& pos)
{
	int sep = pos.find(':');
	if (sep == -1 || sep == 0 || sep >= pos.length() - 1)
		return HMJRE_RESULT_FAILED;
	return make_dword(ival(pos.extract(0, sep)),
					  ival(pos.extract(sep + 1, -1)));
}

typedef struct _ResultList {
	int m_nHead;
	int m_nSize;
	DWORD* m_pResults;
	_ResultList(int size) : m_nHead(0), m_nSize(size), m_pResults(new DWORD[size])
	{}
	~_ResultList() { delete [] m_pResults; }
} ResultList;

class HmJre_Manager {
public:
	HmJre_Manager(const StringBuffer&);
	~HmJre_Manager();

	DWORD match(const StringBuffer& ptn, const StringBuffer& str, int nFlags);
	StringBuffer replace(const StringBuffer& ptn,
						 const StringBuffer& strSrc,
						 const StringBuffer& strDst,
						 int nFlags);
	const StringBuffer& jreGetVersion() const;
	const StringBuffer& getErrorMessage() const
	{ return m_strErrMsg; }

	DWORD getNextResult();
	BOOL hasMoreResults();

	StringBuffer posToString(DWORD pos) const;

private:
	HMODULE m_hModuleDll;
	PFN_JREGETVERSION m_pfnJreGetVersion;
	PFN_DECODEESCSEQ m_pfnDecodeEscSeq;
	PFN_GETJREMESSAGE m_pfnGetJreMessage;
	PFN_JRE2OPEN m_pfnJre2Open;
	PFN_JRE2COMPILE m_pfnJre2Compile;
	PFN_JRE2GETMATCHINFO m_pfnJre2GetMatchInfo;
	PFN_JRE2CLOSE m_pfnJre2Close;

	HashTable<JRE2*,11> m_htblJre;
	LinkList<JRE2> m_Jre2_Pool;
	Auto_Ptr<ResultList> m_pResultList;
	StringBuffer m_strVersion;
	StringBuffer m_strErrMsg;
};

class DllLoadError {};
class DllNotFoundError : public DllLoadError {};
class DllProcAddressNotFoundError : public DllLoadError {};

#endif
