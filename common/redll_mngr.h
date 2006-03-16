//	$Id: redll_mngr.h,v 1.2 2006-03-16 14:46:56 sugiura Exp $
/*
 *	redll_mngr.h
 *	BRegExp_Manager, HmJre_Manager のスーパークラス
 */

#ifndef DENGAKUSERIES_CLASSES_REDLL_MANAGER
#define DENGAKUSERIES_CLASSES_REDLL_MANAGER

#include "strutils.h"
#include "strbuf.h"

#define REDLL_RESULT_FAILED	(DWORD)-1

inline DWORD
make_dword(int low, int high)
{
	return (DWORD)((USHORT)low | (high << 16));
}

class REDLL_Manager {
public:
	REDLL_Manager(const StringBuffer&);
	virtual ~REDLL_Manager();

	const StringBuffer& getErrorMessage() const
	{ return m_strErrMsg; }

protected:
	HMODULE m_hModuleDll;
	StringBuffer m_strErrMsg;
};

class DllLoadError {};
class DllNotFoundError : public DllLoadError {};
class DllProcAddressNotFoundError : public DllLoadError {};

#endif
