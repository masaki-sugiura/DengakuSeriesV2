//	$Id: seq_op.cpp,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	seq_op.cpp
 *	SequentialOp クラスの実装
 */

#include "seq_op.h"
#include "dirlist.h"
#include "pathname.h"
#include "rec_op.h"
#include "cmdline.h"

static BOOL
IsPathWildCard(LPCSTR path)
{
	if (path == NULL || *path == '\0') return FALSE;
	LPCSTR	sep = lstrrchr(path,'\\');
	if (sep == NULL) sep = path;
	else sep++;
	return (lstrchr(sep,'*') != NULL || lstrchr(sep,'?') != NULL);
}

SequentialOp::SequentialOp(LPCSTR av, CmdLineParser& params, DirList& dl)
	: m_av(av), m_params(params), m_DirList(dl)
{
	//	nothing to do more.
}

SequentialOp::~SequentialOp()
{
	//	do nothing.
}

BOOL
SequentialOp::doOp()
{
	if (m_av == NULL) return FALSE;
	BOOL bSuccess = TRUE;
	PathName file;
	do {
		if (IsPathWildCard(m_av)) {
			if (!m_DirList.getPathName(m_av,file,FALSE)) continue;
			WIN32_FIND_DATA fd;
			HANDLE hFile = ::FindFirstFile(file,&fd);
			if (hFile == INVALID_HANDLE_VALUE) {
				// dirname may be invalid..??
				StringBuffer basename(file.getBaseName());
				file.delPath();
				if (!file.isValid()) bSuccess = FALSE;
				// else no match file for the specified wildcard
				file.addPath(basename);
				continue;
			}
			file.delPath();
			do {
				if (!precheck_wc(fd)) continue;
				file.addPath(fd.cFileName);
				DWORD ret = do_op(file);
				if ((ret&RO_STOP) != 0) return FALSE;
				if ((ret&RO_FAILED) != 0) bSuccess = FALSE;
				file.delPath();
			} while (::FindNextFile(hFile,&fd));
			::FindClose(hFile);
		} else if (m_DirList.getPathName(m_av,file,FALSE) &&
			precheck_normal(file)) {
			DWORD ret = do_op(file);
			if ((ret&RO_STOP) != 0) return FALSE;
			if ((ret&RO_FAILED) != 0) bSuccess = FALSE;
		} else {
			bSuccess = FALSE;
		}
	} while ((m_av = m_params.getNextArgv()) != NULL);

	return bSuccess;
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
	CmdLineParser& params)
{
	int pnum = params.itemNum();
	if (pfnFTF == NULL || pnum < 2) return FALSE;

	//	操作先ファイル・フォルダ名の正当性チェック
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

	//	オプションの解析
	params.initSequentialGet();
	int optnum = 0;
	LPCSTR av;
	while ((av = params.getNextArgv()) != NULL && isopthead(*av)) {
		GetFlags(++av,fFlags);
		optnum++;
	}
	//	移動すべきファイル・フォルダの指定がない
	if (av == NULL || optnum >= pnum - 1) return FALSE;

	BOOL bSuccess = TRUE;
	PathName file;
	if (dest.isValid() && dest.isDirectory()) {
		//	移動先はフォルダ
		do {
			if (IsPathWildCard(av)) {
				if (!DirList.getPathName(av,file,FALSE)) {
					bSuccess = FALSE;
					continue;
				}
				WIN32_FIND_DATA fd;
				HANDLE hFile = ::FindFirstFile(file,&fd);
				if (hFile == INVALID_HANDLE_VALUE) {
					// dirname may be invalid..??
					StringBuffer basename(file.getBaseName());
					file.delPath();
					if (!file.isValid()) bSuccess = FALSE;
					// else no match file for the specified wildcard
					file.addPath(basename);
					continue;
				}
				file.delPath();
				do {
					if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0) {
						if ((fFlags&FLAG_RECURSIVE) == 0 ||
							IsPathNameDots(fd.cFileName)) continue;
					}
					file.addPath(fd.cFileName);
					dest.addPath(fd.cFileName);
					DWORD ret = (*pfnFTF)(file,dest,fFlags);
					if ((ret&RO_STOP) != 0) return FALSE;
					else if ((ret&RO_FAILED) != 0) bSuccess = FALSE;
					dest.delPath();
					file.delPath();
				} while (::FindNextFile(hFile,&fd));
				::FindClose(hFile);
			} else if (DirList.getPathName(av,file,TRUE)) {
				dest.addPath(file.getBaseName());
				DWORD ret = (*pfnFTF)(file,dest,fFlags);
				if ((ret&RO_STOP) != 0) return FALSE; // 処理の停止要求
				else if ((ret&RO_FAILED) != 0)
					bSuccess = FALSE; // このファイルの処理は失敗
				dest.delPath(1);
			} else {
				//	コピー元ファイル・フォルダがない
				bSuccess = FALSE;
			}
		} while ((av = params.getNextArgv()) != NULL);
	} else if (DirList.getPathName(av,file,TRUE)) {
		//	移動先は存在しない、またはファイル
		bSuccess = ((*pfnFTF)(file,dest,fFlags) == RO_SUCCESS);
	} else {
		bSuccess = FALSE;
	}

	return bSuccess;
}

//	オプションの解析
//	copy, move, remove で使用
void
GetFlags(LPCSTR av, DWORD& fFlags)
{
	while (*av) {
		switch (*av++) {
		case 'r':	//	再帰的に検索
			fFlags |=	FLAG_RECURSIVE;
			break;
		case 'f':	//	強制的に処理
			fFlags |=	(FLAG_OVERRIDE_FORCED|FLAG_REMOVE_FORCED);
			break;
		case 'i':	//	確認
			fFlags |=	(FLAG_OVERRIDE_CONFIRM|FLAG_REMOVE_CONFIRM);
			break;
		case 'u':	//	新しいファイル・フォルダのみ
			fFlags |=	FLAG_OVERRIDE_NOTNEWER;
			break;
		}
	}
}

