//	$Id: dirlist.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	dirlist.cpp
 *	全ドライブのカレントディレクトリの管理
 */

#include "dirlist.h"
#include "pathname.h"
#include "stack.h"
#include <exception>

static int
GetTokenLength(LPCSTR str, TCHAR sep)
{
	if (str == NULL) return -1;
	LPCSTR top = str;
	while (*str != '\0' && *str != sep)
		str = ToNextChar(str);
	return str - top;
}

int
DirList::getPathType(const StringBuffer& sbPathName)
{
	if (sbPathName.length() <= 0) return ISPATHINVALID;

	LPCSTR pathname = sbPathName;
	if (*pathname == '\\') {
		//	1st char is "\"
		if (*++pathname == '\\') {
			//	UNC path ??
			int	len = GetTokenLength(++pathname,'\\');
			if (len <= 0)	//	"\\" or "\\\"
				return ISPATHINVALID;
			else if (*(pathname += len) == '\0')	//	"\\svr"
				return ISPATHREMOTE|ISSERVERSPECIFIED;

			len = GetTokenLength(++pathname,'\\');
			if (len < 0)	//	有り得ない！！
				return ISPATHINVALID;
			else if (len == 0)	//	"\\svr\"
				return ISPATHREMOTE|ISSERVERSPECIFIED;
			else if (*(pathname += len) == '\0')	//	"\\svr\share"
				return ISPATHREMOTE|ISSERVERSPECIFIED|ISSHARENAMESPECIFIED;

			len = GetTokenLength(++pathname,'\\');
			if (len < 0)	//	有り得ない！！
				return ISPATHINVALID;
			else if (len == 0)	//	"\\svr\share\"
				return ISPATHREMOTE|ISSERVERSPECIFIED|ISSHARENAMESPECIFIED;
			else	//	"\\svr\share\dir.."
				return ISPATHREMOTE|ISSERVERSPECIFIED|ISSHARENAMESPECIFIED|
						ISPATHSPECIFIED|ISPATHFROMROOT;
		} else {
			//	"\dir.." ??
			return ROOTPATH; //	ISPATHLOCAL|ISPATHSPECIFIED|ISPATHFROMROOT;
		}
	} else if (ISDRIVELETTER(*pathname) && *++pathname == ':') {
		//	"X:.."
		if (*++pathname == '\\')
			return ABSOLUTEPATH; // ISPATHLOCAL|ISDRIVESPECIFIED|
								 // ISPATHSPECIFIED|ISPATHFROMROOT;
		else
			return DRIVERELPATH; // ISPATHLOCAL|ISDRIVESPECIFIED|
								 // ISPATHSPECIFIED;
	} else {
		//	"dir.."
		return RELPATH;	//	ISPATHLOCAL|ISPATHSPECIFIED;
	}
	//	not reached.
}

BOOL
DirList::resolveRelPath(int type, const StringBuffer& tmpbuf, PathName& pbuf)
{
	//	copy header
	LPCSTR ptr = tmpbuf;
	if ((type&ISPATHREMOTE) != 0) {
		//	UNC path
		ptr = lstrchr(ptr + 2,'\\');
		if (ptr == NULL) return FALSE;	//	有り得ない！！
		ptr = lstrchr(ptr + 1,'\\');
		if (ptr == NULL) return FALSE;	//	有り得ない！！
		pbuf.reset(tmpbuf,0,ptr - tmpbuf);
		pbuf.append((TCHAR)'\\');	//	これがないと×
		ptr++;
	} else {
		//	local path
		pbuf.reset(ptr,0,3);
		ptr += 3;
	}

	//	code for resolving relative path, "." or ".." .
	Stack<TCHAR> ptr_stack(MAX_PATH/2);
	for (; *ptr != '\0'; ptr = lstrchr(ptr,'\\') + 1) {
		if (*ptr == '.') {
			if (*++ptr == '\\') continue;
			else if (*ptr == '.') {
				if (*++ptr == '\\') {
					ptr_stack.pop();
					continue;
				}
				ptr--;
			}
			ptr--;
		}
		ptr_stack.push(ptr);
	}
	LPSTR* pptr = ptr_stack.gethead();
	BOOL isDbcs;
	if (*pptr == NULL) return TRUE;
	while ((ptr = *pptr++) != NULL) {
		while ((isDbcs = IsCharLeadByte(*ptr)) || *ptr != '\\') {
			pbuf.append((TCHAR)*ptr);
			if (isDbcs) pbuf.append((TCHAR)*(ptr + 1));
			ptr = ToNextChar(ptr);
		}
		pbuf.append((TCHAR)'\\');
	}
	if (pbuf.length() > 3) pbuf.setcharAt(-1,'\0');
	pbuf.refresh();
	return TRUE;
}

DirList::DirList(BOOL bTrueCD)
	: m_bTrueCD(bTrueCD)
{
	//	カレントディレクトリ(ドライブ)の決定
	TCHAR cdbuf[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,cdbuf);
	if (!ISDRIVELETTER(*cdbuf)) ::GetWindowsDirectory(cdbuf,MAX_PATH);
	m_nCurDrive = DRIVENUM(*cdbuf) - 1;

	//	存在する全てのドライブのカレントディレクトリを設定
	::ZeroMemory(m_ppsbCurDir, sizeof(m_ppsbCurDir[0]) * 32);
	DWORD drives = ::GetLogicalDrives();
	try {
		for (int i = 0; i < 32; i++) {
			if ((0x1 & drives) != 0) {
				if (i == m_nCurDrive) {
					//	カレントドライブ
					m_ppsbCurDir[i] = new StringBuffer(cdbuf);
				} else {
					//	それ以外
					m_ppsbCurDir[i] = new StringBuffer(" :\\");
				}
				//	ドライブレターを大文字に
				m_ppsbCurDir[i]->setcharAt(0,(TCHAR)('A' + i));
			}
			drives >>= 1;
		}
	} catch (exception&) {
		// nothing to be able to do...
	}
}

DirList::~DirList()
{
	for (int i = 0; i < 32; i++) delete m_ppsbCurDir[i];
}

BOOL
DirList::getPathName(
	const StringBuffer& name,
	PathName& pbuf,
	BOOL bChkValid) const
{
	int	type = DirList::getPathType(name);

	if (type == INVALIDPATH) {
		if (name.length() == 0) {
			pbuf.reset(this->getCurrentDir());
			return TRUE;
		} else {
			return FALSE;
		}
	}

	StringBuffer tmpbuf(MAX_PATH);

	if ((type&ISPATHREMOTE) != 0) {
		//	UNC path
		tmpbuf.append(name);
		if ((type&ISPATHSPECIFIED) == 0) {
			//	"\\svr" or "\\svr\share"
			pbuf.reset(name);
			if (pbuf.charAt(-1) == '\\') pbuf.setcharAt(-1,'\0');
			return TRUE;
		}
	} else {
		LPCSTR pname = name; // むぅ…
		//	local path
		switch (type) {
		case ABSOLUTEPATH:
			if (!ISDRIVELETTER(*pname)) return FALSE;	//	inconsistent
			tmpbuf.append((TCHAR)('A' + DRIVENUM(*pname++) - 1));
			tmpbuf.append(pname);
			break;
		case ROOTPATH:
			tmpbuf.append((TCHAR)('A' + m_nCurDrive)).append((TCHAR)':')
					.append(name);
			break;
		case DRIVERELPATH:
			{
				int	drive = DRIVENUM(*pname);
				pname += 2;
				LPCSTR curdir = this->getCurrentDir(drive);
				if (curdir != NULL) {
					tmpbuf.append(curdir);
					if (tmpbuf.charAt(-1) != '\\' ||
						IsCharLeadByte(tmpbuf.charAt(-2)))
						tmpbuf.append((TCHAR)'\\');
				} else {
					//	specified drive does not exist!
					tmpbuf.append((TCHAR)('A' + drive - 1)).append(":\\");
				}
				tmpbuf.append(pname);
			}
			break;
		case RELPATH:
			tmpbuf.append(this->getCurrentDir());
			if (tmpbuf.charAt(-1) != '\\' ||
				IsCharLeadByte(tmpbuf.charAt(-2)))
				tmpbuf.append((TCHAR)'\\');
			tmpbuf.append(name);
			break;
		default:
			return FALSE;
		}
		if (ISDRIVELETTER(tmpbuf[0])	&&
			tmpbuf.length() == 3	&&
			tmpbuf[1] == ':'	&&
			tmpbuf[2] == '\\') {
			//	local drive
			pbuf.append(tmpbuf);
			return bChkValid ?
					(this->getCurrentDir(DRIVENUM(pbuf[0])) != NULL) :
					TRUE;
		}
	}

	if (tmpbuf.charAt(-1) != '\\' ||
		IsCharLeadByte(tmpbuf.charAt(-2)))
		tmpbuf.append((TCHAR)'\\');	//	terminator

	//	相対パス名の解決
	if (!resolveRelPath(type,tmpbuf,pbuf)) return FALSE;

	return bChkValid ? pbuf.isValid() : TRUE;
}

LPCSTR
DirList::getCurrentDir(int drive) const
{
	if (drive < 0 || drive > 32) return NULL;
	else if (drive == 0) drive = m_nCurDrive;
	else drive--;
	if (m_ppsbCurDir[drive] == NULL) return NULL;
	return (LPCSTR)*m_ppsbCurDir[drive];
}

LPCSTR
DirList::setCurrentDir(const StringBuffer& dirname)
{
	if (dirname.length() == 0) return NULL;
	PathName buf;
	if (!this->getPathName(dirname,buf,TRUE) ||
		(buf.length() > 3 && !buf.isDirectory())) return NULL;
	TCHAR ch = buf[0];
	if (!ISDRIVELETTER(ch)) return NULL;	//	UNC path
	m_nCurDrive = ch - 'A';
	if (m_ppsbCurDir[m_nCurDrive] == NULL) {
		try {
			m_ppsbCurDir[m_nCurDrive] = new StringBuffer(buf);
		} catch (exception&) {
			return NULL;
		}
	} else *m_ppsbCurDir[m_nCurDrive] = buf;
	if (m_bTrueCD) // 実際にディレクトリを移動
		::SetCurrentDirectory(*m_ppsbCurDir[m_nCurDrive]);

	return (LPCSTR)*m_ppsbCurDir[m_nCurDrive];
}

