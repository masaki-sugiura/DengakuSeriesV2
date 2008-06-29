//	$Id: redll_mngr.h,v 1.3 2008-06-29 15:19:42 sugiura Exp $
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

inline StringBuffer
make_pos_result(int nPos, int nLength)
{
	if (nPos == -1)
	{
		return nullStr;
	}

	return StringBuffer(16).append((DWORD)nPos)
						   .append((TCHAR)':')
						   .append((DWORD)nLength);
}

inline void
get_pos_and_length_from_pos_result(const StringBuffer& pos, int& nPos, int& nLength)
{
	int sep = pos.find(':');

	if (sep == -1 || sep == 0 || sep >= pos.length() - 1)
	{
		nPos	= -1;
		nLength	= 0;
		return;
	}

	nPos	= ival(pos.extract(0, sep));
	nLength	= ival(pos.extract(sep + 1, -1));
}

#endif
