//	$Id: si_file.cpp,v 1.8 2002-03-05 14:19:35 sugiura Exp $
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

	DWORD	fptOrg = fFileOrg.setFilePointer(-1,0,FILE_END),
			fptDest = fFileDest.setFilePointer(0,0,FILE_BEGIN);
	if (fptOrg == 0xFFFFFFFF || fptDest == 0xFFFFFFFF) return FALSE;

	TCHAR	ch;

	//	テキストモードの場合、最後の [EOF] を削除する
	fFileOrg.readFile((BYTE*)&ch,1);
	if (bText && ch == (TCHAR)0x1A) fFileOrg.setFilePointer(-1,0,FILE_END);

	Array<BYTE> buf(READBUF_SIZE);
	DWORD rnum = READBUF_SIZE;
	while (rnum == READBUF_SIZE) {
		if (!(rnum = fFileDest.readFile(buf,READBUF_SIZE))) break;
		fFileDest.setFilePointer(0,0,FILE_CURRENT);	//	不必要？
		if (!fFileOrg.writeFile(buf,rnum)) break;
		fFileOrg.setFilePointer(0,0,FILE_CURRENT);	//	不必要？
	}
	fFileOrg.flushFileBuffers();

	return TRUE;
}

//	getlongname の実体
static StringBuffer
GetLongFileName(const PathName& file)
{
	int	olen = file.length();
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

//	*run の実体
static BOOL
RunCmd(CmdLineParser& rCmdLine, const StringBuffer& curdir, int nCmdShow)
{
	int	num = rCmdLine.itemNum();
	if (num < 1) return FALSE;

	StringBuffer strFile(rCmdLine.getRawData());
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

static BOOL
TimeStampToFileTime(LPCSTR pszTimeStamp, FILETIME* pft)
{
	TCHAR buf[20];
	if (*(pszTimeStamp + 2) == '/') {
		//	"YY/MM/DD#hh:mm:ss" の場合
		buf[0] = '1'; buf[1] = '9';
		lstrcpyn(buf + 2,pszTimeStamp,18);
	} else lstrcpyn(buf,pszTimeStamp,20);

	//	正しい指定かどうかのチェック
	if (buf[4] != '/' || buf[7] != '/' ||
		buf[10] != '#' || buf[13] != ':' || buf[16] != ':')
		return FALSE;
	buf[4] = buf[7] = buf[10] = buf[13] = buf[16] = '\0';
	if (!isnumber(buf) || !isnumber(buf+5) || !isnumber(buf+8) ||
		!isnumber(buf+11) || !isnumber(buf+14) || !isnumber(buf+17))
		return FALSE;

	int	val;
	SYSTEMTIME st;
	if ((val = ival(buf)) < 1980) val = 1980;
	st.wYear	= (WORD)val;
	if ((val = ival(buf+5)) < 1 || val > 12) val = 1;
	st.wMonth	= (WORD)val;
	if ((val = ival(buf+8)) < 1 || val > 31) val = 1;
	st.wDay		= (WORD)val;
	st.wDayOfWeek	= 0;
	if ((val = ival(buf+11)) < 0 || val > 23) val = 0;
	st.wHour	= (WORD)val;
	if ((val = ival(buf+14)) < 0 || val > 59) val = 0;
	st.wMinute	= (WORD)val;
	if ((val = ival(buf+17)) < 0 || val > 59) val = 0;
	st.wSecond	= (WORD)val;
	st.wMilliseconds	= 0;
	FILETIME ftlocal;
	::SystemTimeToFileTime(&st,&ftlocal);
	::LocalFileTimeToFileTime(&ftlocal,pft);

	return TRUE;
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
			!GetFlags(av, fFlags)) break;
		optnum++;
	}
	// ファイル指定がない
	if (optnum == pnum) return 0;

	SeqOpResult* psor = NULL;
	if (fFlags & FLAG_RETURNNUM) psor = new SeqOpResult();
	int ret = SeqRemove(optnum, params, m_DirList, fFlags, psor).doOp();
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
		if (ret & RO_STOP) {
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

static DWORD flagsRmDir[] = { FLAG_REMOVE_FORCED, FLAG_RETURNNUM };
static OptMap optMapRmDir("fn", flagsRmDir);

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
	LPCSTR av = params.getNextArgv();
	if (isopthead(*av)) {
		if (*(av + 1) == 'n') bRetNum = TRUE;
		av = params.getNextArgv();
		optnum++;
	}
	if (pnum == optnum) return FALSE;

	//	時刻指定の解析
	DWORD flags = TOUCH_ACCESSTIME|TOUCH_MODIFYTIME|TOUCH_FILECREATE;
	LPCSTR pszRefFile = NULL, pszTimeStamp = NULL, *pptr = NULL;

	while ((av = params.getNextArgv()) != NULL) {
		if (pptr != NULL) {
			*pptr = av;
			pptr = NULL;
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
		drives_str[i] = (drives&0x00000001) != 0 ? '1' : '0';

	return drives_str;
}

StringBuffer
SessionInstance::si_getlongname(const StringBuffer& filename, BOOL bExist)
{
	PathName file;
	if (!m_DirList.getPathName(filename,file,bExist)) return nullStr;
	return GetLongFileName(file);
}

