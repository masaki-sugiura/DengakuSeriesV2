//	$Id: si_file.cpp,v 1.2 2001-11-22 13:37:09 sugiura Exp $
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
		return CopyPath(DestFile,OrgFile,FLAG_OVERRIDE_FORCED) == RO_SUCCESS;
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

	BYTE* buf = new BYTE[READBUF_SIZE];
	DWORD rnum = READBUF_SIZE;
	while (rnum == READBUF_SIZE) {
		if (!(rnum = fFileDest.readFile(buf,READBUF_SIZE))) break;
		fFileDest.setFilePointer(0,0,FILE_CURRENT);	//	不必要？
		if (!fFileOrg.writeFile(buf,rnum)) break;
		fFileOrg.setFilePointer(0,0,FILE_CURRENT);	//	不必要？
	}
	fFileOrg.flushFileBuffers();
	delete[] buf;

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
GetAttrFlags(CmdLineParser& params, DWORD& mask, DWORD& attrflags)
{
	LPCSTR av = params.getNextArgv();
	if (isopthead(*av) && *(av + 1) == 'R' && *(av + 2) == '\0') {
		attrflags |= ATTRIBUTE_RECURSIVE;
		av = params.getNextArgv();
	}
	if (av == NULL) return FALSE;
	while (*av != '\0') {
		switch (*av++) {
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
	return TRUE;
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
	return FileToFileOperation(CopyPath,FLAG_OVERRIDE_DEFAULT,
								m_DirList,params);
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
	SeqRemove(LPCSTR av, CmdLineParser& params, DirList& dl, DWORD flags)
		: SequentialOp(av,params,dl), m_flags(flags)
	{}

	BOOL precheck_wc(WIN32_FIND_DATA& fd)
	{
		if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0) return TRUE;
		if ((m_flags&FLAG_RECURSIVE) == 0) return FALSE;
		return	!IsPathNameDots(fd.cFileName);
	}
	DWORD do_op(PathName& file)
	{
		return RemovePath(file,m_flags);
	}

private:
	DWORD m_flags;
};

int
SessionInstance::si_remove(CmdLineParser& params)
{
	if (params.initSequentialGet() < 1) return FALSE;

	//	オプションの解析
	DWORD fFlags = FLAG_REMOVE_DEFAULT;
	LPCSTR av;
	while ((av = params.getNextArgv()) != NULL && isopthead(*av)) {
		GetFlags(++av,fFlags);
	}
	if (av == NULL) return FALSE; // ファイル指定がない

	return SeqRemove(av,params,m_DirList,fFlags).doOp();
}

int
SessionInstance::si_move(CmdLineParser& params)
{
	return FileToFileOperation(
					MovePath,
					FLAG_OVERRIDE_DEFAULT|FLAG_RECURSIVE,
					m_DirList,
					params
				);
}

int
SessionInstance::si_mkdir(CmdLineParser& params)
{
	if (params.initSequentialGet() < 1) return FALSE;

	BOOL bRec = FALSE, bSuccess = TRUE;
	//	オプションの解析
	LPCSTR av;
	while ((av = params.getNextArgv()) != NULL && isopthead(*av)) {
		if (*++av == 'p') bRec = TRUE;
	}
	if (av == NULL) return FALSE;

	PathName dir;
	BOOL bSuccess_Each;
	do {
		m_DirList.getPathName(av,dir,FALSE);
		if (dir.isValid()) {
			//	いずれにしても指定されたパスは存在する
			bSuccess_Each = dir.isDirectory();
		} else {
			bSuccess_Each = bRec ?	(RecursiveMakeDir(dir) == RO_SUCCESS) :
									CreateDirectory(dir,NULL);
		}
		if (bSuccess) bSuccess = bSuccess_Each;
	} while ((av = params.getNextArgv()) != NULL);

	return bSuccess;
}

class SeqRmDir : public SequentialOp {
public:
	SeqRmDir(LPCSTR av, CmdLineParser& params, DirList& dl, DWORD flags)
		: SequentialOp(av,params,dl), m_flags(flags)
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
	DWORD do_op(PathName& file)
	{
		return SafetyRemoveDirectory(file,m_flags);
	}

private:
	DWORD m_flags;
};

int
SessionInstance::si_rmdir(CmdLineParser& params)
{
	if (params.initSequentialGet() < 1) return FALSE;

	//	オプションの解析
	DWORD flags = FLAG_REMOVE_DEFAULT;
	LPCSTR av = params.getNextArgv();
	if (isopthead(*av) && *(av + 1) == 'f' && *(av + 2) == '\0') {
		flags |= FLAG_REMOVE_FORCED;
		if ((av = params.getNextArgv()) == NULL) return FALSE;
	}

	return SeqRmDir(av,params,m_DirList,flags).doOp();
}

class SeqSetAttr : public SequentialOp {
public:
	SeqSetAttr(LPCSTR av, CmdLineParser& params, DirList& dl, DWORD flags)
		: SequentialOp(av,params,dl), m_flags(flags)
	{}

	DWORD do_op(PathName& file)
	{
		return SetPathAttributes(file,m_flags);
	}

private:
	DWORD m_flags;
};

int
SessionInstance::si_setattribute(CmdLineParser& params)
{
	if (params.initSequentialGet() < 2) return FALSE;

	DWORD attrflags = 0, mask = ATTRIBUTE_MASK_ADD;
	//	オプションの解析
	if (!GetAttrFlags(params,mask,attrflags)) return FALSE;
	LPCSTR av = params.getNextArgv();

	//	属性指定子の解析
	if ((attrflags&(ATTRIBUTE_MASK_ADD|ATTRIBUTE_MASK_DEL|ATTRIBUTE_MASK_REP))
		== 0 ||
		av == NULL) return FALSE;

	return SeqSetAttr(av,params,m_DirList,attrflags).doOp();
}

class SeqTouch : public SequentialOp {
public:
	SeqTouch(LPCSTR av, CmdLineParser& params, DirList& dl,
			const FILETIME* pft, DWORD flags)
		: SequentialOp(av,params,dl), m_flags(flags), m_pft(pft)
	{}

	BOOL precheck_normal(PathName&) { return TRUE; }
	DWORD do_op(PathName& file)
	{
		return SetPathTime(file,m_pft,m_flags);
	}

private:
	DWORD m_flags;
	const FILETIME* m_pft;
};

int
SessionInstance::si_touch(CmdLineParser& params)
{
	if (params.initSequentialGet() < 1) return FALSE;

	//	時刻指定の解析
	DWORD flags = TOUCH_ACCESSTIME|TOUCH_MODIFYTIME|TOUCH_FILECREATE;
	LPCSTR	av, pszRefFile = NULL, pszTimeStamp = NULL, *pptr = NULL;

	while ((av = params.getNextArgv()) != NULL) {
		if (pptr != NULL) {
			*pptr = av;
			pptr = NULL;
			continue;
		}
		if (!isopthead(*av)) break;
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
	if (av == NULL) return FALSE;

	FILETIME ft;
	if (pszRefFile != NULL) {
		//	タイムスタンプをファイルから取得
		PathName reffile;
		if (!m_DirList.getPathName(pszRefFile,reffile,TRUE))
			return FALSE;
		::CopyMemory((LPVOID)&ft,(const LPVOID)reffile.getTime(),
					sizeof(FILETIME));
	} else if (pszTimeStamp != NULL) {
		//	タイムスタンプを直接指定
		if (!TimeStampToFileTime(pszTimeStamp,&ft)) return FALSE;
	} else {
		//	現在時刻を取得
		::GetSystemTimeAsFileTime(&ft);
	}

	return SeqTouch(av,params,m_DirList,&ft,flags).doOp();
}

int
SessionInstance::si_run(CmdLineParser& params, int nCmdShow)
{
	return RunCmd(params,m_DirList.getCurrentDir(),nCmdShow);
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

	dl.getPathName(av,path,FALSE);
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
	if (!ParseEnumOptions(params,m_DirList,path,filter,s_order,flags))
		return FALSE;
	try {
		if ((flags & RECFIND_REVERSE) != 0)
			m_pFindData = new RecFindBackward(path,anyPathName,filter,
											s_order,flags);
		else
			m_pFindData = new RecFindForward(path,anyPathName,filter,
											s_order,flags);
	} catch (exception&) {
		return FALSE;
	}
	return m_pFindData->isValid();
}

int
SessionInstance::si_enumfile(CmdLineParser& params)
{
	PathName path;
	StringBuffer filter(16), s_order("nets");
	DWORD flags = ENUMPATH_FINDFILE;
	if (!ParseEnumOptions(params,m_DirList,path,filter,s_order,flags))
		return FALSE;
	try {
		m_pFindData = new FindFile(path,filter,s_order,flags);
	} catch (exception&) {
		return FALSE;
	}
	return m_pFindData->isValid();
}

int
SessionInstance::si_enumdir(CmdLineParser& params)
{
	PathName path;
	StringBuffer filter(16), s_order("nets");
	DWORD flags = ENUMPATH_FINDDIR;
	if (!ParseEnumOptions(params,m_DirList,path,filter,s_order,flags))
		return FALSE;
	try {
		m_pFindData = new FindDir(path,filter,s_order,flags);
	} catch (exception&) {
		return FALSE;
	}
	return m_pFindData->isValid();
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
		if (m_DirList.getPathName(params.getArgvStr(0),file,TRUE))
			bReadOnly = file.isReadOnly();
	} else if (m_pFindData.ptr() && m_pFindData->isValid()) {
		bReadOnly
			= (m_pFindData->getAttributes()&FILE_ATTRIBUTE_READONLY) != 0;
	}
	return bReadOnly ? errorStr : nullStr;
}

StringBuffer
SessionInstance::si_sizeof(CmdLineParser& params)
{
	if (params.itemNum() > 0) {
		PathName file;
		if (m_DirList.getPathName(params.getArgvStr(0),file,TRUE))
			return StringBuffer(16).append(file.getSize());
		else
			return nullStr;
	} else if (m_pFindData.ptr() && m_pFindData->isValid()) {
		return StringBuffer(16).append(m_pFindData->getSize());
	} else {
		return nullStr;
	}
}

StringBuffer
SessionInstance::si_timestampof(CmdLineParser& params)
{
	LPCSTR file = NULL;
	int offset = 2;

	if (params.itemNum() > 0) {
		params.initSequentialGet();
		LPCSTR av = params.getNextArgv();
		if (av != NULL && isopthead(*av)) {
			if (*++av == 'l') {
				av = params.getNextArgv();
				offset = 0;
			} else return nullStr;
		}
		if (av != NULL) file = av;
	}

	FILETIME time;
	if (file != NULL) {
		PathName pfile;
		if (!m_DirList.getPathName(file,pfile,TRUE)) return nullStr;
		time = *pfile.getTime();
	} else if (m_pFindData.ptr() && m_pFindData->isValid()) {
		const FILETIME* ptime = m_pFindData->getTime();
		if (ptime == NULL) return nullStr; // root drive has no timestamp.
		time = *ptime;
	} else {
		return nullStr;
	}
	FILETIME ftlocal;
	if (!::FileTimeToLocalFileTime(&time,&ftlocal)) return nullStr;
	DOSDATE	dosdate;
	DOSTIME	dostime;
	if (!::FileTimeToDosDateTime(&ftlocal,(LPWORD)&dosdate,(LPWORD)&dostime))
		return nullStr;
	TCHAR dbuf[32];
	wsprintf(dbuf,
			"%04d/%02d/%02d %02d:%02d:%02d",
			dosdate.Year+1980,dosdate.Month,dosdate.Day,
			dostime.Hour,dostime.Minute,dostime.Second*2);

	return StringBuffer(32).append(dbuf,offset);
}

StringBuffer
SessionInstance::si_timecountof(CmdLineParser& params)
{
	FILETIME time;

	if (params.itemNum() > 0) {
		PathName file;
		if (!m_DirList.getPathName(params.getArgvStr(0),file,TRUE))
			return nullStr;
		time = *file.getTime();
	} else if (m_pFindData.ptr() && m_pFindData->isValid()) {
		const FILETIME* ptime = m_pFindData->getTime();
		if (ptime == NULL) return nullStr; // root drive has no timestamp.
		time = *ptime;
	} else {
		return nullStr;
	}

	WORD low, high;
	if (!::FileTimeToDosDateTime(&time,&high,&low)) return nullStr;

	return StringBuffer(16).append((DWORD)((high << 16) | low));
}

StringBuffer
SessionInstance::si_attributeof(CmdLineParser& params)
{
	DWORD attr = 0xFFFFFFFF;

	if (params.itemNum() > 0) {
		PathName file;
		if (!m_DirList.getPathName(params.getArgvStr(0),file,TRUE))
			return nullStr;
		attr = file.getAttributes();
	} else if (m_pFindData.ptr() && m_pFindData->isValid()) {
		attr = m_pFindData->getAttributes();
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
	if (m_pFindData.ptr() && m_pFindData->findNext())
		return m_pFindData->getBaseName();
	return nullStr;
}

StringBuffer
SessionInstance::si_getdrives()
{
	StringBuffer drives_str = "00000000000000000000000000000000";
	DWORD drives = ::GetLogicalDrives();

	for (int i = 0; i < 32; i++, drives >>= 1)
		drives_str.setcharAt(i,(drives&0x00000001) != 0 ? '1' : '0');

	return drives_str;
}

StringBuffer
SessionInstance::si_getlongname(const StringBuffer& filename, BOOL bExist)
{
	PathName file;
	if (!m_DirList.getPathName(filename,file,bExist)) return nullStr;
	return GetLongFileName(file);
}

