//	$Id: seq_op.cpp,v 1.6 2002-04-17 16:23:31 sugiura Exp $
/*
 *	seq_op.cpp
 *	SequentialOp クラスの実装
 */

#include "seq_op.h"
#include "dirlist.h"
#include "pathname.h"
#include "rec_op.h"
#include "cmdline.h"
#include "optmap.h"

static BOOL
IsPathWildCard(const StringBuffer& path)
{
	if (path.length() == 0) return FALSE;
	int sep = path.rfind((TCHAR)'\\') + 1; // 見つからなかったら -1 + 1 = 0
	int wcc = path.rfind((TCHAR)'*');
	if (wcc >= 0 && sep <= wcc) return TRUE;
	wcc = path.rfind((TCHAR)'?');
	return (wcc >= 0 && sep <= wcc);
}

void
AddEnumResult(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2,
	TCHAR result)
{
	if (psor) {
		StringBuffer*
			pstr = new StringBuffer(path.length() + path2.length() + 4);
		pstr->append(result);
		pstr->append(SEQ_OP_RESULT_SEPCHAR);
		pstr->append(path);
		if (path2.length() > 0) {
			pstr->append(SEQ_OP_RESULT_SEPCHAR);
			pstr->append(path2);
		}
		psor->addItem(pstr);
	}
}


SequentialOp::SequentialOp(
	int srcidx,
	CmdLineParser& params,
	DirList& dl,
	SeqOpResult* pEnumResult)
	: m_srcIndex(srcidx),
	  m_params(params),
	  m_DirList(dl),
	  m_pEnumResult(pEnumResult)
{
}

SequentialOp::~SequentialOp()
{
	//	do nothing.
}

int
SequentialOp::doOp()
{
	int fnum = m_params.initSequentialGet(m_srcIndex);

	BOOL ret = TRUE;
	PathName file;
	while (fnum-- > 0) {
		const StringBuffer& av = m_params.getNextArgvStr();
		if (IsPathWildCard(av)) {
			if (!m_DirList.getPathName(av, file, FALSE)) {
				ret = FALSE;
				AddFailure(m_pEnumResult, av);
				continue;
			}
			WIN32_FIND_DATA fd;
			HANDLE hFile = ::FindFirstFile(file, &fd);
			if (hFile == INVALID_HANDLE_VALUE) {
				// dirname may be invalid..??
				StringBuffer basename(file.getBaseName());
				file.delPath();
				if (!file.isValid()) {
					ret = FALSE;
					AddFailure(m_pEnumResult, av);
					continue; // dirname is invalid!!
				}
				// else no match file for the specified wildcard
				file.addPath(basename);
				continue;
			}
			file.delPath();
			do {
				if (!precheck_wc(fd)) continue;
				file.addPath(fd.cFileName);
				DWORD result = do_op(file, m_pEnumResult);
				if ((result&RO_STOP) != 0) return ret;
				else if ((result&RO_FAILED) != 0) ret = FALSE;
				file.delPath();
			} while (::FindNextFile(hFile, &fd));
			::FindClose(hFile);
		} else if (m_DirList.getPathName(av, file, FALSE) &&
			precheck_normal(file)) {
			DWORD result = do_op(file, m_pEnumResult);
			if ((result&RO_STOP) != 0) return ret;
			else if ((result&RO_FAILED) != 0) ret = FALSE;
		} else {
			ret = FALSE;
			AddFailure(m_pEnumResult, av);
		}
	}

	return ret;
}

BOOL
SequentialOp::precheck_wc(WIN32_FIND_DATA& fd)
{
	return (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0 ||
			!IsPathNameDots(fd.cFileName);
}

BOOL
SequentialOp::precheck_normal(PathName& file)
{
	return file.isValid();
}

BOOL
FileToFileOperation(
	PFN_FILETOFILE pfnFTF,
	DWORD fFlags,
	DirList& DirList,
	CmdLineParser& params,
	SeqOpResult*& psor)
{
	int pnum = params.initSequentialGet();
	if (pfnFTF == NULL || pnum < 2) return FALSE;

	//	オプションの解析
	int optnum = 0;
	while (optnum < pnum) {
		const StringBuffer& av = params.getNextArgvStr();
		if (!isopthead(av.charAt(0))) break;
		GetFlags(av, fFlags);
		optnum++;
	}
	//	移動すべきファイル・フォルダの指定がない
	if (pnum - optnum < 2) return FALSE;

	//	操作先フォルダ名の正当性チェック
	PathName dest;
	if (!DirList.getPathName(params.getArgvStr(-1), dest, FALSE)) {
#if 0
		StringBuffer msg("specified filename: ");
		msg.append(dest).append("\nis invalid");
		::MessageBox(NULL, msg, NULL, MB_OK);
#endif
		return FALSE;
	}

	params.delArgv(-1);
	pnum--;

	if (fFlags & FLAG_RETURNNUM) psor = new SeqOpResult();

	PathName file;
	if (dest.isValid() && dest.isDirectory()) {
		//	移動先はフォルダ
		BOOL ret = TRUE;
		params.initSequentialGet(optnum);
		pnum -= optnum;
		while (pnum-- > 0) {
			const StringBuffer& av = params.getNextArgvStr();
			if (IsPathWildCard(av)) {
				if (!DirList.getPathName(av, file, FALSE)) {
					ret = FALSE;
					AddFailure(psor, av, dest);
					continue;
				}
				WIN32_FIND_DATA fd;
				HANDLE hFile = ::FindFirstFile(file, &fd);
				if (hFile == INVALID_HANDLE_VALUE) {
					// dirname may be invalid..??
					StringBuffer basename(file.getBaseName());
					file.delPath();
					if (!file.isValid()) {
						// dirname is invalid
						ret = FALSE;
						AddFailure(psor, av, dest);
						continue;
					}
					// else no match file for the specified wildcard
					file.addPath(basename);
					continue;
				}
				file.delPath();
				do {
					if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
						if ((fFlags & FLAG_RECURSIVE) == 0 ||
							IsPathNameDots(fd.cFileName)) continue;
					}
					file.addPath(fd.cFileName);
					dest.addPath(fd.cFileName);
					DWORD ret = (*pfnFTF)(file, dest, fFlags, psor);
					if ((ret&RO_STOP) != 0) return ret;
					else if ((ret&RO_FAILED) != 0) ret = FALSE;
					dest.delPath();
					file.delPath();
				} while (::FindNextFile(hFile, &fd));
				::FindClose(hFile);
			} else if (DirList.getPathName(av, file, TRUE)) {
				dest.addPath(file.getBaseName());
				DWORD result = (*pfnFTF)(file, dest, fFlags, psor);
				if ((result&RO_STOP) != 0) return ret;
				else if ((result&RO_FAILED) != 0) ret = FALSE;
				dest.delPath(1);
			} else {
				//	コピー元ファイル・フォルダがない
				ret = FALSE;
				AddFailure(psor, av, dest);
			}
		}
		return ret;
	} else if (DirList.getPathName(params.getArgvStr(optnum), file, TRUE)) {
		//	移動先は存在しない、またはファイル
		return ((*pfnFTF)(file, dest, fFlags, psor) == RO_SUCCESS);
	} else {
		AddFailure(psor, params.getArgvStr(optnum), dest);
		return FALSE;
	}
	// not reached.
}

