//	$Id: seq_op.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	seq_op.h
 *	ˆø”‚ğ‚È‚ß‚È‚ª‚çˆ—‚ğs‚¤‚½‚ß‚Ì’ŠÛŠî’êƒNƒ‰ƒX
 */

#ifndef DENGAKUSERIES_CLASSES_SEQ_OP
#define DENGAKUSERIES_CLASSES_SEQ_OP

#include <windows.h>

class CmdLineParser;
class DirList;
class PathName;

class SequentialOp {
public:
	SequentialOp(LPCSTR, CmdLineParser&, DirList&);
	virtual ~SequentialOp();

	BOOL doOp();

protected:
	LPCSTR m_av;
	CmdLineParser& m_params;
	DirList& m_DirList;

	virtual BOOL precheck_wc(WIN32_FIND_DATA&);
	virtual BOOL precheck_normal(PathName&);
	virtual DWORD do_op(PathName&) = 0;

	SequentialOp(const SequentialOp&);
	SequentialOp& operator=(const SequentialOp&);
};

typedef BOOL (*PFN_FILETOFILE)(const PathName&, const PathName&, DWORD);
BOOL FileToFileOperation(PFN_FILETOFILE,DWORD,DirList&,CmdLineParser&);
void GetFlags(LPCSTR, DWORD&);

#endif
