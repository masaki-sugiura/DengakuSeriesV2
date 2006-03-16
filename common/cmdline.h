//	$Id: cmdline.h,v 1.2 2006-03-16 14:46:56 sugiura Exp $
/*
 *	cmdline.h
 *	文字列のパース＆リスト化
 */

#ifndef DENGAKUSERIES_CLASSES_CMDLINEPARSER
#define DENGAKUSERIES_CLASSES_CMDLINEPARSER

#include "linklist.h"

#define MAX_CMDLINE	80

class CmdArgv {
public:
	CmdArgv(const StringBuffer& str, int head, int size)
		: m_str(str), m_head(head), m_size(size)
	{}

	StringBuffer m_str;
	int m_head;
	int m_size;
};

class CmdLineParser : private LinkList<CmdArgv> {
public:
	CmdLineParser(const StringBuffer& cmdline);
	virtual ~CmdLineParser() {}

	//	exports from LinkList
	using LinkList<CmdArgv>::itemNum;
	using LinkList<CmdArgv>::initSequentialGet;

	const StringBuffer& getRawData() const { return m_rawcmdline; }

	int delArgv(int i);

	LPCSTR getArgv(int i) const
	{
		CmdArgv* pArgv = getItemByIndex(i);
		return pArgv != NULL ? (LPCSTR)pArgv->m_str : NULL;
	}

	LPCSTR getNextArgv()
	{
		CmdArgv* pArgv = getNextItem();
		return pArgv != NULL ? (LPCSTR)pArgv->m_str : NULL;
	}

	const StringBuffer&	getArgvStr(int i) const
	{
		CmdArgv* pArgv = getItemByIndex(i);
		return pArgv != NULL ? pArgv->m_str : nullStr;
	}

	const StringBuffer&	getNextArgvStr()
	{
		CmdArgv* pArgv = getNextItem();
		return pArgv != NULL ? pArgv->m_str : nullStr;
	}

	//	引数(一つ)を追加する
	int addArgv(const StringBuffer& newstr)
	{
		int head = m_rawcmdline.length() + 1,
			size = newstr.length();
		int ret = this->addItem(new CmdArgv(newstr,head,size),-1);
		if (ret >= 0) m_rawcmdline.append((TCHAR)' ').append(newstr);
		return ret;
	}

	//	引数(一般に複数)を追加する
	virtual int mergeArgv(const StringBuffer& newstr) = 0;

protected:
	using LinkList<CmdArgv>::addItem;
	StringBuffer m_rawcmdline;
	TCHAR m_workBuffer[MAX_CMDLINE];
};

class RealCmdLineParser : public CmdLineParser {
public:
	RealCmdLineParser(const StringBuffer&);
	int mergeArgv(const StringBuffer&);
};

inline BOOL
isopthead(TCHAR ch)
{
	return ch == '-' || ch == '/';
}

StringBuffer RemoveQuote(const StringBuffer&);

#endif
