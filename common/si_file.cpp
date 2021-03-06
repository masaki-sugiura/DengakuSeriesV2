//	$Id: si_file.cpp,v 1.20 2011/01/07 16:08:38 sugiura Exp $
/*
 *	si_file.cpp
 *	SessionInstance: ファイルサービスの関数
 */

#include "session.h"
#include "seq_op.h"
#include "rec_op.h"
#include "recfind.h"
#include "file.h"
#include "cmdline.h"
#include "optmap.h"
#include "misc.h"

#include <exception>

//	si_TIMESTAMPOF() で使うタイムスタンプを扱う構造体
typedef struct {
	WORD Day:5;
	WORD Month:4;
	WORD Year:7;
} DOSDATE;

typedef struct {
	WORD Second:5;
	WORD Minute:6;
	WORD Hour:5;
} DOSTIME;

//	バッファサイズ
#define	READBUF_SIZE 1024

//	appendcopy(text) の実体
static BOOL
AppendCopy(const PathName& OrgFile, const PathName& DestFile, BOOL bText)
{
	DWORD attr = OrgFile.getAttributes();
	if (attr == 0xFFFFFFFF) {
		// アペンド先が存在しない
//		return CopyPath(DestFile,OrgFile,0) == RO_SUCCESS;
		return FALSE;
	} else if ((attr&FILE_ATTRIBUTE_DIRECTORY) != 0) {
		// アペンド先はディレクトリ
		return FALSE;
	} else if (OrgFile.getSize() == 0) {
		// アペンド先はサイズ０…コピーと一緒
		return CopyPath(DestFile,OrgFile,FLAG_OVERRIDE_FORCED,NULL) == RO_SUCCESS;
	}

	File	fFileOrg(OrgFile,
					GENERIC_READ|GENERIC_WRITE,
					OPEN_EXISTING,
					FILE_FLAG_SEQUENTIAL_SCAN),
			fFileDest(DestFile,
					GENERIC_READ,
					OPEN_EXISTING,
					FILE_FLAG_SEQUENTIAL_SCAN);
	if (!fFileOrg.isValid() || !fFileDest.isValid()) return FALSE;

	LONGLONG fptOrg = fFileOrg.setFilePointer(-1, FILE_END),
			 fptDest = fFileDest.setFilePointer(0, FILE_BEGIN);
	if (fptOrg == -1 || fptDest == -1) return FALSE;

	//	テキストモードの場合、最後の [EOF] を削除する
	TCHAR ch;
	fFileOrg.readFile((BYTE*)&ch, 1);
	if (bText && ch == (TCHAR)0x1A) fFileOrg.setFilePointer(-1, FILE_END);

	Array<BYTE> buf(READBUF_SIZE);
	DWORD rnum = READBUF_SIZE;
	while (rnum == READBUF_SIZE) {
		if (!(rnum = fFileDest.readFile(buf, READBUF_SIZE))) break;
		fFileDest.setFilePointer(0, FILE_CURRENT);	//	不必要？
		if (!fFileOrg.writeFile(buf, rnum)) break;
		fFileOrg.setFilePointer(0, FILE_CURRENT);	//	不必要？
	}
	fFileOrg.flushFileBuffers();

	return TRUE;
}

class SeqEnumFiles : public SequentialOp {
public:
	SeqEnumFiles(int srcidx,
				 CmdLineParser& params,
				 DirList& dl,
				 DWORD flags,
				 SeqOpResult* psor)
		: SequentialOp(srcidx,params,dl,psor),
		  m_flags(flags),
		  m_dwBufferSize(0)
	{}

	BOOL precheck_wc(WIN32_FIND_DATA& fd)
	{
		if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0) return TRUE;
		if ((m_flags&FLAG_RECURSIVE) == 0) return FALSE;
		return	!IsPathNameDots(fd.cFileName);
	}
	DWORD do_op(PathName& file, SeqOpResult* psor)
	{
		m_dwBufferSize += file.length() + 1;
		m_pnList.addItem(new PathName(file), -1);
		return RO_SUCCESS;
	}

	LinkList<PathName> m_pnList;
	DWORD getBufferSize() const { return m_dwBufferSize + 1; }

private:
	DWORD m_flags;
	DWORD m_dwBufferSize;
};

static int
RecycleBin(int srcidx,
		   CmdLineParser& params,
		   DirList& dl,
		   DWORD flags,
		   SeqOpResult* psor)
{
	SeqEnumFiles enumFiles(srcidx, params, dl, flags, psor);

	if (!enumFiles.doOp()) return 0;

	LinkList<PathName>& pnList = enumFiles.m_pnList;

	int pnOffset = 0, pnLen = enumFiles.getBufferSize();
	LPSTR pBuf = new TCHAR[pnLen];
	int fnum = pnList.initSequentialGet(), i;
	for (i = 0; i < fnum; i++) {
		PathName* pFile = pnList.getNextItem();
		int len = pFile->length() + 1;
		::CopyMemory(pBuf + pnOffset, (LPCSTR)*pFile, len);
		pnOffset += len;
	}
	pBuf[pnOffset] = 0;

	SHFILEOPSTRUCT shfop;
	shfop.hwnd = NULL;
	shfop.wFunc = FO_DELETE;
	shfop.pFrom = pBuf;
	shfop.pTo = NULL;
	shfop.fAnyOperationsAborted = FALSE;
	shfop.hNameMappings = NULL;
	shfop.lpszProgressTitle = NULL;
	shfop.fFlags = ((flags & FLAG_REMOVE_CONFIRM) ? 0 : FOF_NOCONFIRMATION) |
				   ((flags & FLAG_RECURSIVE) ? 0 : FOF_NORECURSION) |
				   FOF_ALLOWUNDO;

	int ret = ::SHFileOperation(&shfop);
	if (!psor) return !ret;
	pnList.initSequentialGet();
	if (ret) {
		for (i = 0; i < fnum; i++) {
			PathName* pFile = pnList.getNextItem();
			AddFailure(psor, (LPCSTR)*pFile);
		}
	} else if (shfop.fAnyOperationsAborted) {
		for (i = 0; i < fnum; i++) {
			PathName* pFile = pnList.getNextItem();
			AddCancel(psor, (LPCSTR)*pFile);
		}
	} else {
		for (i = 0; i < fnum; i++) {
			PathName* pFile = pnList.getNextItem();
			AddSuccess(psor, (LPCSTR)*pFile);
		}
	}
	return !ret;
}

//	getlongname の実体
static StringBuffer
GetLongFileName(const PathName& file)
{
	ptrdiff_t	olen = file.length();
	if (olen < 3 || file.find((TCHAR)'*') >= 0 || file.find((TCHAR)'?') >= 0)
		//	不正なパス名
		return nullStr;

	TCHAR dl = file[0];
	if (ISDRIVELETTER(dl) && olen == 3) {
		if (file[1] != ':' || file[2] != '\\') return nullStr;
		//	ルートパス名だった
		StringBuffer newname(file);
		newname.setcharAt(0,(TCHAR)('A' + DRIVENUM(dl) - 1));
		return newname;
	}

	StringBuffer buf(file);	//	テンポラリバッファ
	StringBuffer newname(MAX_PATH);

	//	ルートパス名のコピー
	if (ISDRIVELETTER(dl)) {
		//	通常のパス名
		olen = 3;
		newname.append((TCHAR)('A' + DRIVENUM(dl) - 1)).append(":\\");
	} else {
		//	UNC パス名(のはず)
		LPCSTR	h = lstrchr(buf + 2,'\\');
		if (h == NULL ||
			(h = lstrchr(++h,'\\')) == NULL) return nullStr; // 不正なパス名
		olen = ++h - (LPCSTR)buf;
		newname.append(buf,0,olen);
	}

	//	残りのパス名の処理
	HANDLE hdata = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	fd;
	for (LPSTR s = buf.getBufPtr() + olen;
		s != NULL && *s != '\0' && (s = lstrchr(s,'\\')) != NULL;
		*s++ = '\\'){
		*s = '\0';
		if ((hdata = ::FindFirstFile(buf,&fd)) == INVALID_HANDLE_VALUE)
			return nullStr;
		::FindClose(hdata);
		newname.append(fd.cFileName).append((TCHAR)'\\');
	}
	//	最後のノードの処理
	if ((hdata = ::FindFirstFile(buf,&fd)) != INVALID_HANDLE_VALUE) {
		newname.append(fd.cFileName);
	} else {
		newname.append(file.getBaseName());
	}

	return newname;
}

//	getshortname の実体
static StringBuffer
GetShortFileName(const PathName& file)
{
	ptrdiff_t	olen = file.length();
	if (olen < 3 || file.find((TCHAR)'*') >= 0 || file.find((TCHAR)'?') >= 0)
		//	不正なパス名
		return nullStr;

	TCHAR dl = file[0];
	if (ISDRIVELETTER(dl) && olen == 3) {
		if (file[1] != ':' || file[2] != '\\') return nullStr;
		//	ルートパス名だった
		StringBuffer newname(file);
		newname.setcharAt(0,(TCHAR)('A' + DRIVENUM(dl) - 1));
		return newname;
	}

	StringBuffer buf(file);	//	テンポラリバッファ
	StringBuffer newname(MAX_PATH);

	//	ルートパス名のコピー
	if (ISDRIVELETTER(dl)) {
		//	通常のパス名
		olen = 3;
		newname.append((TCHAR)('A' + DRIVENUM(dl) - 1)).append(":\\");
	} else {
		//	UNC パス名(のはず)
		LPCSTR	h = lstrchr(buf + 2,'\\');
		if (h == NULL ||
			(h = lstrchr(++h,'\\')) == NULL) return nullStr; // 不正なパス名
		olen = ++h - (LPCSTR)buf;
		newname.append(buf,0,olen);
	}

	//	残りのパス名の処理
	HANDLE hdata = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	fd;
	for (LPSTR s = buf.getBufPtr() + olen;
		s != NULL && *s != '\0' && (s = lstrchr(s,'\\')) != NULL;
		*s++ = '\\'){
		*s = '\0';
		if ((hdata = ::FindFirstFile(buf,&fd)) == INVALID_HANDLE_VALUE)
			return nullStr;
		::FindClose(hdata);
		if (fd.cAlternateFileName[0]) {
			newname.append(fd.cAlternateFileName).append((TCHAR)'\\');
		} else {
			newname.append(fd.cFileName).append((TCHAR)'\\');
		}
	}
	//	最後のノードの処理
	if ((hdata = ::FindFirstFile(buf,&fd)) != INVALID_HANDLE_VALUE) {
		if (fd.cAlternateFileName[0]) {
			newname.append(fd.cAlternateFileName);
		} else {
			newname.append(fd.cFileName);
		}
	} else {
		newname.append(file.getBaseName());
	}

	return newname;
}

//	*run の実体
static BOOL
RunCmd(CmdLineParser& rCmdLine, const StringBuffer& curdir, int nCmdShow)
{
	int	num = rCmdLine.itemNum();
	if (num < 1) return FALSE;

	StringBuffer strFile(RemoveQuote(rCmdLine.getRawData()));
	LPCSTR lpParam = NULL;
	if (num > 1) {
		strFile = rCmdLine.getArgv(0);
		rCmdLine.delArgv(0);
		lpParam = rCmdLine.getRawData();
	}
	BOOL bRet = (int)::ShellExecute(NULL,NULL,
								strFile,
								lpParam,
								curdir,
								nCmdShow) > 32;
	if (bRet) ::SetLastError(0L);	//	関数が成功しても
									//	エラー値をセットされる場合がある！！
	return bRet;
}

int
SessionInstance::si_setcurdir(const StringBuffer& newdir)
{
	return m_DirList.setCurrentDir(newdir) != NULL;
}

int
SessionInstance::si_copy(CmdLineParser& params)
{
	SeqOpResult* psor = NULL;
	int ret = FileToFileOperation(CopyPath, FLAG_OVERRIDE_DEFAULT,
								  m_DirList, params, psor);
	if (psor) {
		m_pFileOpResult = psor;
		return psor->m_nSuccess;
	} else {
		return ret;
	}
}

int
SessionInstance::si_appendcopy(
	const StringBuffer& orgfile,
	const StringBuffer& destfile,
	BOOL bText)
{
	PathName org, dest;
	if (m_DirList.getPathName(orgfile,org,FALSE) &&
		m_DirList.getPathName(destfile,dest,TRUE) &&
		AppendCopy(org,dest,bText)) return TRUE;
	return FALSE;
}

int
SessionInstance::si_rename(
	const StringBuffer& orgname,
	const StringBuffer& destname)
{
	PathName org, dest;
	if (m_DirList.getPathName(orgname,org,TRUE)   &&
		m_DirList.getPathName(destname,dest,FALSE) &&
		::MoveFile(org,dest)) return TRUE;
	return FALSE;
}

int
SessionInstance::si_delete(const StringBuffer& filename)
{
	PathName file;
	if (m_DirList.getPathName(filename,file,TRUE) &&
		::DeleteFile(file)) return TRUE;
	return FALSE;
}

class SeqRemove : public SequentialOp {
public:
	SeqRemove(int srcidx,
			  CmdLineParser& params,
			  DirList& dl,
			  DWORD flags,
			  SeqOpResult* psor)
		: SequentialOp(srcidx,params,dl,psor), m_flags(flags)
	{}

	BOOL precheck_wc(WIN32_FIND_DATA& fd)
	{
		if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0) return TRUE;
		if ((m_flags&FLAG_RECURSIVE) == 0) return FALSE;
		return	!IsPathNameDots(fd.cFileName);
	}
	DWORD do_op(PathName& file, SeqOpResult* psor)
	{
		return RemovePath(file, m_flags, psor);
	}

private:
	DWORD m_flags;
};

static DWORD flagsRemove[] = {
	FLAG_RECURSIVE,	//	再帰的に検索
	FLAG_RETURNNUM,	//	処理したファイル・フォルダの数を返す
	FLAG_OVERRIDE_FORCED | FLAG_REMOVE_FORCED,	//	強制的に処理
	FLAG_OVERRIDE_CONFIRM | FLAG_REMOVE_CONFIRM,	//	確認
	FLAG_OVERRIDE_NOTNEWER,	//	新しいファイル・フォルダのみ
	FLAG_REMOVE_RECYCLE // ごみ箱へ移動
};
static OptMap optMapRemove("rnfiug", flagsRemove);

int
SessionInstance::si_remove(CmdLineParser& params)
{
	int pnum = params.initSequentialGet();
	if (pnum < 1) return 0;

	//	オプションの解析
	DWORD fFlags = FLAG_REMOVE_DEFAULT;

	int optnum = 0;
	while (optnum < pnum) {
		const StringBuffer& av = params.getNextArgvStr();
		if (!isopthead(av[0]) ||
			!GetFlags(av, fFlags, optMapRemove)) break;
		optnum++;
	}
	// ファイル指定がない
	if (optnum == pnum) return 0;

	SeqOpResult* psor = NULL;
	int ret;
	if (fFlags & FLAG_RETURNNUM) psor = new SeqOpResult();
	if (fFlags & FLAG_REMOVE_RECYCLE) {
		// ごみ箱へ移動
		ret = RecycleBin(optnum, params, m_DirList, fFlags, psor);
	} else {
		// いきなり削除
		ret = SeqRemove(optnum, params, m_DirList, fFlags, psor).doOp();
	}
	if (psor) {
		m_pFileOpResult = psor;
		return psor->m_nSuccess;
	} else {
		return ret;
	}
}

int
SessionInstance::si_move(CmdLineParser& params)
{
	SeqOpResult* psor = NULL;
	int ret = FileToFileOperation(
					MovePath,
					FLAG_OVERRIDE_DEFAULT|FLAG_RECURSIVE,
					m_DirList,
					params,
					psor
				);
	if (psor) {
		m_pFileOpResult = psor;
		return psor->m_nSuccess;
	} else {
		return ret;
	}
}

static DWORD flagsMkDir[] = { FLAG_RECURSIVE, FLAG_RETURNNUM };
static OptMap optMapMkDir("pn", flagsMkDir);

int
SessionInstance::si_mkdir(CmdLineParser& params)
{
	int pnum = params.initSequentialGet();
	if (pnum < 1) return FALSE;

	DWORD flags = 0;
	//	オプションの解析
	int optnum = 0;
	while (optnum < pnum) {
		const StringBuffer& av = params.getNextArgvStr();
		if (!isopthead(av[0]) ||
			!GetFlags(av, flags, optMapMkDir)) break;
		optnum++;
	}
	if (optnum == pnum) return FALSE;

	SeqOpResult* psor = NULL;
	if (flags & FLAG_RETURNNUM) psor = new SeqOpResult();

	int ret = TRUE;
	pnum = params.initSequentialGet(optnum);
	PathName dir;
	while (pnum-- > 0) {
		m_DirList.getPathName(params.getNextArgvStr(), dir, FALSE);
		BOOL bResult;
		if (dir.isValid()) {
			//	いずれにしても指定されたパスは存在する
			bResult = dir.isDirectory();
		} else {
			if (flags & FLAG_RECURSIVE) {
				bResult = (RecursiveMakeDir(dir) == RO_SUCCESS);
			} else {
				bResult = CreateDirectory(dir, NULL);
			}
		}
		if (bResult) {
			AddSuccess(psor, dir);
		} else {
			AddFailure(psor, dir);
			ret = FALSE;
		}
	}

	if (psor) {
		m_pFileOpResult = psor;
		return psor->m_nSuccess;
	} else {
		return ret;
	}
}

class SeqRmDir : public SequentialOp {
public:
	SeqRmDir(int srcidx,
			 CmdLineParser& params,
			 DirList& dl,
			 DWORD flags,
			 SeqOpResult* psor)
		: SequentialOp(srcidx, params, dl, psor), m_flags(flags)
	{}

	BOOL precheck_wc(WIN32_FIND_DATA& fd)
	{
		if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0) return FALSE;
		return	!IsPathNameDots(fd.cFileName);
	}
	BOOL precheck_normal(PathName& file)
	{
		return file.isDirectory();
	}
	DWORD do_op(PathName& file, SeqOpResult* psor)
	{
		int ret = SafetyRemoveDirectory(file,m_flags);
		if ((ret & RO_STOP) || (ret & RO_CANCELED)) {
			AddCancel(psor, file);
		} else if (ret & RO_FAILED) {
			AddFailure(psor, file);
		} else {
			AddSuccess(psor, file);
		}
		return ret;
	}

private:
	DWORD m_flags;
};

static DWORD flagsRmDir[] = { FLAG_REMOVE_CONFIRM, FLAG_REMOVE_FORCED, FLAG_RETURNNUM };
static OptMap optMapRmDir("ifn", flagsRmDir);

int
SessionInstance::si_rmdir(CmdLineParser& params)
{
	int pnum = params.initSequentialGet();
	if (pnum < 1) return FALSE;

	//	オプションの解析
	DWORD flags = FLAG_REMOVE_DEFAULT;
	int optnum = 0;
	while (optnum < pnum) {
		const StringBuffer& av = params.getNextArgvStr();
		if (!isopthead(av[0]) ||
			!GetFlags(av, flags, optMapRmDir)) break;
		optnum++;
	}
	if (optnum == pnum) return FALSE;

	SeqOpResult* psor = NULL;
	if (flags & FLAG_RETURNNUM) psor = new SeqOpResult();
	int ret = SeqRmDir(optnum, params, m_DirList, flags, psor).doOp();
	if (psor) {
		m_pFileOpResult = psor;
		return psor->m_nSuccess;
	} else {
		return ret;
	}
}

class SeqSetAttr : public SequentialOp {
public:
	SeqSetAttr(int srcidx,
			   CmdLineParser& params,
			   DirList& dl,
			   DWORD flags,
			   SeqOpResult* psor)
		: SequentialOp(srcidx, params, dl, psor), m_flags(flags)
	{}

	DWORD do_op(PathName& file, SeqOpResult* psor)
	{
		return SetPathAttributes(file,m_flags,psor);
	}

private:
	DWORD m_flags;
};

static DWORD flagsGetAttr[] = { ATTRIBUTE_RECURSIVE, FLAG_RETURNNUM };
static OptMap optMapGetAttr("Rn", flagsGetAttr);

static int
GetAttrFlags(CmdLineParser& params, DWORD& mask, DWORD& attrflags)
{
	int pnum = params.initSequentialGet();

	int optnum = 0;
	while (optnum < pnum) {
		const StringBuffer& av = params.getNextArgvStr();
		if (!isopthead(av[0]) ||
			!GetFlags(av, attrflags, optMapGetAttr)) break;
		optnum++;
	}
	if (optnum == pnum) return 0;

	const StringBuffer& attropts = params.getArgvStr(optnum++);

	int i = 0, len = attropts.length();
	while (i < len) {
		switch (attropts[i++]) {
		case '-':
			mask = ATTRIBUTE_MASK_DEL;
			break;
		case '+':
			mask = ATTRIBUTE_MASK_ADD;
			break;
		case '=':
			mask = ATTRIBUTE_MASK_REP;
			break;
		case 'a':
			attrflags |= (ATTRIBUTE_FLAG_ARCHIVE&mask);
			break;
		case 'r':
			attrflags |= (ATTRIBUTE_FLAG_READONLY&mask);
			break;
		case 'h':
			attrflags |= (ATTRIBUTE_FLAG_HIDDEN&mask);
			break;
		case 's':
			attrflags |= (ATTRIBUTE_FLAG_SYSTEM&mask);
			break;
		default:
			break;
		}
	}

	return optnum;
}

int
SessionInstance::si_setattribute(CmdLineParser& params)
{
	if (params.itemNum() < 1) return FALSE;

	DWORD attrflags = 0, mask = ATTRIBUTE_MASK_ADD;

	//	オプション＆属性指定子の解析
	int optnum = GetAttrFlags(params, mask, attrflags);

	//	オプション＆属性指定の妥当性検査
	if (optnum == 0 ||
		(attrflags&(ATTRIBUTE_MASK_ADD|ATTRIBUTE_MASK_DEL|ATTRIBUTE_MASK_REP))
		== 0) return FALSE;

	SeqOpResult* psor = NULL;
	if (attrflags & FLAG_RETURNNUM) psor = new SeqOpResult();
	int ret = SeqSetAttr(optnum, params, m_DirList, attrflags, psor).doOp();
	if (psor) {
		m_pFileOpResult = psor;
		return psor->m_nSuccess;
	} else {
		return ret;
	}
}

class SeqTouch : public SequentialOp {
public:
	SeqTouch(int srcidx,
			 CmdLineParser& params,
			 DirList& dl,
			 const FILETIME* pft,
			 DWORD flags,
			 SeqOpResult* psor)
		: SequentialOp(srcidx,params,dl,psor), m_flags(flags), m_pft(pft)
	{}

	BOOL precheck_normal(PathName&) { return TRUE; }
	DWORD do_op(PathName& file, SeqOpResult* psor)
	{
		return SetPathTime(file,m_pft,m_flags,psor);
	}

private:
	DWORD m_flags;
	const FILETIME* m_pft;
};

int
SessionInstance::si_touch(CmdLineParser& params)
{
	int pnum = params.initSequentialGet();
	if (pnum < 1) return FALSE;

	BOOL bRetNum = FALSE;
	int optnum = 0;
	LPCSTR av = NULL;

	//	時刻指定の解析
	DWORD flags = TOUCH_ACCESSTIME|TOUCH_MODIFYTIME|TOUCH_FILECREATE;
	LPCSTR pszRefFile = NULL, pszTimeStamp = NULL, *pptr = NULL;

	while ((av = params.getNextArgv()) != NULL) {
		if (pptr != NULL) {
			*pptr = av;
			pptr = NULL;
			optnum++;
			continue;
		}
		if (!isopthead(*av)) break;
		optnum++;
		while (*++av != '\0') {
			switch (*av) {
			case 'a':
				flags &= TOUCH_FILECREATE;
				flags |= TOUCH_ACCESSTIME;
				break;
			case 'm':
				flags &= TOUCH_FILECREATE;
				flags |= TOUCH_MODIFYTIME;
				break;
			case 'c':
				flags &= ~TOUCH_FILECREATE;
				break;
			case 'R':
				flags |= TOUCH_RECURSIVE;
				break;
			case 'r':
				if (*++av != '\0') {
					pszRefFile = av;
					while (*(av + 1) != '\0') av++;
				} else pptr = &pszRefFile;
				break;
			case 't':
				if (*++av != '\0') {
					pszTimeStamp = av;
					while (*(av + 1) != '\0') av++;
				} else pptr = &pszTimeStamp;
				break;
			case 'n':
				bRetNum = TRUE;
				break;
			default:
				break;
			}
		}
	}
	if (pnum == optnum) return FALSE;

	FILETIME ft;
	if (pszRefFile != NULL) {
		//	タイムスタンプをファイルから取得
		PathName reffile;
		if (!m_DirList.getPathName(pszRefFile, reffile, TRUE))
			return FALSE;
		::CopyMemory((LPVOID)&ft, (const LPVOID)reffile.getTime(),
					sizeof(FILETIME));
	} else if (pszTimeStamp != NULL) {
		//	タイムスタンプを直接指定
		if (!TimeStampToFileTime(pszTimeStamp, &ft))
			return FALSE;
	} else {
		//	現在時刻を取得
		::GetSystemTimeAsFileTime(&ft);
	}

	SeqOpResult* psor = NULL;
	if (bRetNum) {
		psor = new SeqOpResult();
		flags |= FLAG_RETURNNUM;
	}

	int ret = SeqTouch(optnum, params, m_DirList, &ft, flags, psor).doOp();
	if (psor) {
		m_pFileOpResult = psor;
		return psor->m_nSuccess;
	} else {
		return ret;
	}
}

StringBuffer
SessionInstance::si_fileopresult()
{
	if (m_pFileOpResult.ptr() && m_pFileOpResult->findNext())
		return m_pFileOpResult->getValue();
	return nullStr;
}

int
SessionInstance::si_run(CmdLineParser& params, int nCmdShow)
{
	return RunCmd(params, m_DirList.getCurrentDir(), nCmdShow);
}

static BOOL
ParseEnumOptions(
	CmdLineParser& params,
	DirList& dl,
	PathName& path,
	StringBuffer& filter,
	StringBuffer& s_order,
	DWORD& flags)
{
	if (params.initSequentialGet() < 1) return FALSE;

	LPCSTR av;
	while ((av = params.getNextArgv()) != NULL && isopthead(*av)) {
		while (*++av != '\0') {
			switch (*av) {
			case 'r':
				flags |= (RECFIND_REVERSE|ENUMPATH_SORTREVERSE);
				break;
			case 'f':
				flags &= ~RECFIND_SHOWDIR;
				break;
			case 'o':
				if (*++av == '\0') {
					if ((av = params.getNextArgv()) == NULL) return FALSE;
				}
				s_order = av;
				av = NULL;
				break;
			default:
				return FALSE;
			}
			if (av == NULL) break;
		}
	}
	if (av == NULL) return FALSE;

	dl.getPathName(av, path, FALSE);
	if ((av = params.getNextArgv()) != NULL) {
		for (;;) {
			filter.append(av);
			if ((av = params.getNextArgv()) == NULL) break;
			filter.append((TCHAR)';');
		}
	} else {
		filter = path.getBaseName();
		path.delPath(1);
	}

	return TRUE;
}

int
SessionInstance::si_enumpath(CmdLineParser& params)
{
	PathName path;
	StringBuffer filter(16), s_order("nets");
	DWORD flags = RECFIND_SHOWDIR;
	if (!ParseEnumOptions(params, m_DirList, path, filter, s_order, flags))
		return FALSE;
	if ((flags & RECFIND_REVERSE) != 0)
		m_pEnumerator = new RecFindBackward(m_DirList,
											path, anyPathName, filter,
											s_order, flags);
	else
		m_pEnumerator = new RecFindForward(m_DirList,
										   path, anyPathName, filter,
										   s_order, flags);

	return m_pEnumerator->isValid();
}

int
SessionInstance::si_enumfile(CmdLineParser& params)
{
	PathName path;
	StringBuffer filter(16), s_order("nets");
	DWORD flags = ENUMPATH_FINDFILE;
	if (!ParseEnumOptions(params, m_DirList, path, filter, s_order, flags))
		return FALSE;
	m_pEnumerator = new FindFile(m_DirList, path, filter, s_order, flags);

	return m_pEnumerator->isValid();
}

int
SessionInstance::si_enumdir(CmdLineParser& params)
{
	PathName path;
	StringBuffer filter(16), s_order("nets");
	DWORD flags = ENUMPATH_FINDDIR;
	if (!ParseEnumOptions(params, m_DirList, path, filter, s_order, flags))
		return FALSE;
	m_pEnumerator = new FindDir(m_DirList, path, filter, s_order, flags);

	return m_pEnumerator->isValid();
}

StringBuffer
SessionInstance::si_getcurdir()
{
	return m_DirList.getCurrentDir();
}

StringBuffer
SessionInstance::si_isreadonly(CmdLineParser& params)
{
	BOOL bReadOnly = FALSE;
	if (params.itemNum() > 0) {
		PathName file;
		if (m_DirList.getPathName(params.getArgvStr(0), file, TRUE))
			bReadOnly = file.isReadOnly();
	} else if (m_pEnumerator.ptr() && m_pEnumerator->isValid()) {
		bReadOnly = (m_pEnumerator->getAttributes()&FILE_ATTRIBUTE_READONLY) != 0;
	}
	return bReadOnly ? errorStr : nullStr;
}

StringBuffer
SessionInstance::si_sizeof(CmdLineParser& params)
{
	if (params.itemNum() > 0) {
		PathName file;
		if (m_DirList.getPathName(params.getArgvStr(0), file, TRUE))
			return StringBuffer(16).append(file.getSize());
		else
			return nullStr;
	} else if (m_pEnumerator.ptr() && m_pEnumerator->isValid()) {
		return StringBuffer(16).append(m_pEnumerator->getSize());
	}
	return nullStr;
}

StringBuffer
SessionInstance::si_timestampof(CmdLineParser& params)
{
	int offset = 2, optnum = 0, pnum = params.itemNum();

	if (pnum > 0) {
		const StringBuffer& av = params.getArgvStr(0);
		if (isopthead(av[0])) {
			if (av[1] == 'l') offset = 0;
			optnum++;
		}
	}

	FILETIME time;
	if (pnum != optnum) {
		PathName pfile;
		if (!m_DirList.getPathName(params.getArgvStr(optnum), pfile, TRUE))
			return nullStr;
		time = *pfile.getTime();
	} else if (m_pEnumerator.ptr() && m_pEnumerator->isValid()) {
		const FILETIME* ptime = m_pEnumerator->getTime();
		if (ptime == NULL) return nullStr; // root drive has no timestamp.
		time = *ptime;
	} else {
		return nullStr;
	}

	FILETIME ftlocal;
	if (!::FileTimeToLocalFileTime(&time, &ftlocal)) return nullStr;
	DOSDATE	dosdate;
	DOSTIME	dostime;
	if (!::FileTimeToDosDateTime(&ftlocal, (LPWORD)&dosdate, (LPWORD)&dostime))
		return nullStr;

	TCHAR dbuf[32];
	wsprintf(dbuf,
			"%04d/%02d/%02d %02d:%02d:%02d",
			dosdate.Year + 1980,
			dosdate.Month,
			dosdate.Day,
			dostime.Hour,
			dostime.Minute,
			dostime.Second * 2);

	return StringBuffer(32).append(dbuf, offset);
}

StringBuffer
SessionInstance::si_timecountof(CmdLineParser& params)
{
	FILETIME time;

	if (params.itemNum() > 0) {
		PathName file;
		if (!m_DirList.getPathName(params.getArgvStr(0), file, TRUE))
			return nullStr;
		time = *file.getTime();
	} else if (m_pEnumerator.ptr() && m_pEnumerator->isValid()) {
		const FILETIME* ptime = m_pEnumerator->getTime();
		if (ptime == NULL) return nullStr; // root drive has no timestamp.
		time = *ptime;
	} else {
		return nullStr;
	}

	WORD low, high;
	if (!::FileTimeToDosDateTime(&time, &high, &low)) return nullStr;

	return StringBuffer(16).append((DWORD)((high << 16) | low));
}

StringBuffer
SessionInstance::si_attributeof(CmdLineParser& params)
{
	DWORD attr = 0xFFFFFFFF;

	if (params.itemNum() > 0) {
		PathName file;
		if (!m_DirList.getPathName(params.getArgvStr(0), file, TRUE))
			return nullStr;
		attr = file.getAttributes();
	} else if (m_pEnumerator.ptr() && m_pEnumerator->isValid()) {
		attr = m_pEnumerator->getAttributes();
	}
	if (attr == 0xFFFFFFFF) return nullStr;

	TCHAR buf[6];
	int i = 0;
	if ((attr&FILE_ATTRIBUTE_DIRECTORY) != 0)	buf[i++] = 'd';
	if ((attr&FILE_ATTRIBUTE_ARCHIVE) != 0)		buf[i++] = 'a';
	if ((attr&FILE_ATTRIBUTE_READONLY) != 0)	buf[i++] = 'r';
	if ((attr&FILE_ATTRIBUTE_HIDDEN) != 0)		buf[i++] = 'h';
	if ((attr&FILE_ATTRIBUTE_SYSTEM) != 0)		buf[i++] = 's';
	buf[i] = '\0';

	return buf;
}

StringBuffer
SessionInstance::si_findnext()
{
	if (m_pEnumerator.ptr() && m_pEnumerator->findNext())
		return m_pEnumerator->getValue();
	return nullStr;
}

StringBuffer
SessionInstance::si_getdrives()
{
	TCHAR drives_str[33] = "00000000000000000000000000000000";
	DWORD drives = ::GetLogicalDrives();

	for (int i = 0; i < 32; i++, drives >>= 1)
		drives_str[i] = '0' + (drives & 0x00000001);

	return drives_str;
}

StringBuffer
SessionInstance::si_getlongname(const StringBuffer& filename, BOOL bExist)
{
	PathName file;
	if (!m_DirList.getPathName(filename,file,bExist)) return nullStr;
	return GetLongFileName(file);
}

StringBuffer
SessionInstance::si_getshortname(const StringBuffer& filename, BOOL bExist)
{
	PathName file;
	if (!m_DirList.getPathName(filename,file,bExist)) return nullStr;
	return GetShortFileName(file);
}

