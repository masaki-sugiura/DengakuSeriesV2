//	DengakuServer v1: Copyright(C) 1999-2000, M. Sugiura
/*
 *	bregexp_mngr.h
 *	bregexp.dll を利用するためのラッパークラス
 */

#ifndef DENGAKUSERIES_CLASSES_BREGEXP_MANAGER
#define DENGAKUSERIES_CLASSES_BREGEXP_MANAGER

#include "hashtbl.h"
#include "linklist.h"
#include "auto_ptr.h"
#include "bregexp.h"

typedef int (*PFN_BMATCH)(LPCSTR, LPCSTR, LPCSTR, BREGEXP**, char*);
typedef int (*PFN_BSUBST)(LPCSTR, LPCSTR, LPCSTR, BREGEXP**, char*);
typedef int (*PFN_BTRANS)(LPCSTR, LPCSTR, LPCSTR, BREGEXP**, char*);
typedef int (*PFN_BSPLIT)(LPCSTR, LPCSTR, LPCSTR, int, BREGEXP**, char*);
typedef void (*PFN_BREGFREE)(BREGEXP*);
typedef char* (*PFN_BREGEXPVERSION)(void);

#define BREGEXP_FILENAME "bregexp.dll"
#define BREGEXP_BMATCH   "BMatch"
#define BREGEXP_BSUBST   "BSubst"
#define BREGEXP_BTRANS   "BTrans"
#define BREGEXP_BSPLIT   "BSplit"
#define BREGEXP_BREGFREE "BRegfree"
#define BREGEXP_BREGEXPVERSION "BRegexpVersion"

#define BREGEXP_RESULT_FAILED (DWORD)-1

inline StringBuffer
make_result(DWORD res)
{
	return StringBuffer(16).append((DWORD)LOWORD(res))
						   .append((TCHAR)':')
						   .append((DWORD)HIWORD(res));
}

inline DWORD
make_dword(int low, int high)
{
	return (DWORD)((USHORT)low | (high << 16));
}

typedef struct _ResultList {
	int m_nHead;
	int m_nSize;
	DWORD* m_pResults;
	_ResultList(int size) : m_nHead(0), m_nSize(size), m_pResults(new DWORD[size])
	{}
	~_ResultList() { delete [] m_pResults; }
} ResultList;

class BRegExp_Manager {
public:
	BRegExp_Manager(const StringBuffer&);
	~BRegExp_Manager();

	StringBuffer bMatch(const StringBuffer& ptn, const StringBuffer& str);
	StringBuffer bSubst(const StringBuffer& ptn, const StringBuffer& str);
	StringBuffer bTrans(const StringBuffer& ptn, const StringBuffer& str);
	int bSplit(const StringBuffer& ptn, const StringBuffer& str, int limit);
	const StringBuffer& bRegexpversion();

	StringBuffer getNextResult();
	BOOL hasMoreResults();

	const StringBuffer& getErrorMessage() const
	{ return m_strErrMsg; }

private:
	HMODULE m_hModuleDll;
	PFN_BMATCH m_pfnBMatch;
	PFN_BSUBST m_pfnBSubst;
	PFN_BTRANS m_pfnBTrans;
	PFN_BSPLIT m_pfnBSplit;
	PFN_BREGFREE m_pfnBRegfree;
	PFN_BREGEXPVERSION m_pfnBRegexpversion;

	HashTable<BREGEXP*,11> m_htblBRegExp;
	LinkList<BREGEXP> m_BRegExp_Pool;
	Auto_Ptr<ResultList> m_pResultList;
	StringBuffer m_strVersion;
	StringBuffer m_strErrMsg;
};

class DllLoadError {};
class DllNotFoundError : public DllLoadError {};
class DllProcAddressNotFoundError : public DllLoadError {};

#endif
