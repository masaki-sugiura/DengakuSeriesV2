//	$Id: seq_op.h,v 1.4 2002-02-19 15:34:22 sugiura Exp $
/*
 *	seq_op.h
 *	引数をなめながら処理を行うための抽象基底クラス
 */

#ifndef DENGAKUSERIES_CLASSES_SEQ_OP
#define DENGAKUSERIES_CLASSES_SEQ_OP

#include "strbuf.h"
#include "linklist.h"
#include "auto_ptr.h"
#include "enumerator.h"

class CmdLineParser;
class DirList;
class PathName;

#define SEQ_OP_RESULT_SUCCESS  (TCHAR)'0'
#define SEQ_OP_RESULT_FAILURE  (TCHAR)'1'
#define SEQ_OP_RESULT_CANCEL   (TCHAR)'2'
#define SEQ_OP_RESULT_SEPCHAR  (TCHAR)'|'

class SeqOpResult : public LinkList<StringBuffer>,
					public Enumerator<StringBuffer> {
public:
	SeqOpResult() : m_nSuccess(0), m_head(-1) {}

	BOOL isValid() const { return this->itemNum() > 0; }
	BOOL findNext()
	{
		return ++m_head < this->itemNum();
	}
	StringBuffer getValue() const
	{
		if (!this->isValid()) return nullStr;
		return *this->getItemByIndex(m_head);
	}

	int m_nSuccess; // counter of succeeded process

private:
	int m_head;
};

void AddEnumResult(SeqOpResult*, const StringBuffer&, const StringBuffer&, TCHAR);

inline void
AddSuccess(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	if (psor) psor->m_nSuccess++;
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_SUCCESS);
}

inline void
AddFailure(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_FAILURE);
}

inline void
AddCancel(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_CANCEL);
}


class SequentialOp {
public:
	SequentialOp(int, CmdLineParser&, DirList&, SeqOpResult* psor);
	virtual ~SequentialOp();

	int doOp();

protected:
	int m_srcIndex; // m_params 中の最初のソースファイルの位置
	CmdLineParser& m_params;
	DirList& m_DirList;
	SeqOpResult* m_pEnumResult;

	virtual BOOL precheck_wc(WIN32_FIND_DATA&);
	virtual BOOL precheck_normal(PathName&);
	virtual DWORD do_op(PathName&, SeqOpResult*) = 0;

	SequentialOp(const SequentialOp&);
	SequentialOp& operator=(const SequentialOp&);
};

typedef BOOL (*PFN_FILETOFILE)(const PathName&, const PathName&, DWORD, SeqOpResult*);
int FileToFileOperation(PFN_FILETOFILE, DWORD, DirList&, CmdLineParser&, SeqOpResult*&);

#define OPT_INVALID  ((DWORD)-1)

class OptMap {
public:
	OptMap(const StringBuffer& opts, DWORD* flags)
		: m_opts(opts),
		  m_flags(opts.length())
	{
		m_flags.copy(0, flags, m_opts.length());
	}

	DWORD getFlag(TCHAR ch) const
	{
		int idx = m_opts.find(ch);
		return (idx >= 0) ? m_flags[idx] : OPT_INVALID;
	}

private:
	const StringBuffer m_opts;
	Array<DWORD> m_flags;
};

extern OptMap defOptMap;
void GetFlags(const StringBuffer&, DWORD&, const OptMap& omap = defOptMap);

#endif
