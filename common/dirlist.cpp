//	$Id: dirlist.cpp,v 1.3 2002/02/19 15:34:21 sugiura Exp $
/*
 *	dirlist.cpp
 *	全ドライブのカレントディレクトリの管理
 */

#include "misc.h"
#include "dirlist.h"
#include "pathname.h"
#include "stack.h"
#include <exception>

BOOL
DirList::resolveRelPath(int type, const StringBuffer& tmpbuf, PathName& pbuf)
{
	//	copy header
	LPCSTR ptr = tmpbuf;
	if ((type&ISPATHREMOTE) != 0) {
		//	UNC path
		ptr = lstrchr(ptr + 2,'\\');
		if (ptr == NULL) {
			InternalError(__FILE__, __LINE__);
			return FALSE;
		}
		ptr = lstrchr(ptr + 1,'\\');
		if (ptr == NULL) { // "\\svr\"
			pbuf.reset(tmpbuf);
			return TRUE;
		}
		ptr++; // 最後の "\" も含めてコピー
		pbuf.reset(tmpbuf,0,ptr - tmpbuf);
	} else {
		//	local path
		pbuf.reset(ptr,0,3);
		ptr += 3;
	}

	// no more path is specified
	if (*ptr == '\0') return TRUE;

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
	if (IsPathEndWithSep(pbuf)) pbuf.setcharAt(-1,'\0');
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
	if (name.length() == 0) {
		pbuf.reset(this->getCurrentDir());
		return TRUE;
	}

	int	type = PathName::getPathType(name);

	if ((type & ISPATHINVALID) != 0) return FALSE;

	StringBuffer tmpbuf(MAX_PATH);

	if ((type & ISPATHREMOTE) != 0) {
		//	UNC path
		tmpbuf.append(name);
		if ((type & ISPATHSPECIFIED) == 0) {
			//	"\\svr[\]" or "\\svr\share[\]"
//			if (tmpbuf.charAt(-1) == '\\') tmpbuf.setcharAt(-1,'\0');
			pbuf.reset(tmpbuf);
			return bChkValid ? pbuf.isValid() : TRUE;
		}
	} else {
		LPCSTR pname = name; // むぅ…
		//	local path
		if ((type & ISDRIVESPECIFIED) != 0) {
			if ((type & ISPATHFROMROOT) != 0) {
				// absolute path
				tmpbuf.append((TCHAR)('A' + DRIVENUM(*pname++) - 1));
				tmpbuf.append(pname);
			} else {
				// drive relative path
				int	drive = DRIVENUM(*pname);
				LPCSTR curdir = this->getCurrentDir(drive);
				if (curdir != NULL) {
					tmpbuf.append(curdir);
					if (!IsPathEndWithSep(tmpbuf))
						tmpbuf.append((TCHAR)'\\');
				} else {
					//	specified drive does not exist!
					tmpbuf.append((TCHAR)('A' + drive - 1)).append(":\\");
				}
				pname += 2;
				tmpbuf.append(pname);
			}
		} else if ((type & ISPATHFROMROOT) != 0) {
			// "\dir..."
			tmpbuf.append((TCHAR)('A' + m_nCurDrive)).append((TCHAR)':');
			tmpbuf.append(pname);
		} else {
			// "dir..."
			tmpbuf.append(this->getCurrentDir());
			if (!IsPathEndWithSep(tmpbuf)) tmpbuf.append((TCHAR)'\\');
			tmpbuf.append(pname);
		}
	}

	if (!IsPathEndWithSep(tmpbuf))
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
		!buf.isDirectory()) return NULL;
	TCHAR ch = buf[0];
	if (!ISDRIVELETTER(ch)) return NULL; // UNC path
	m_nCurDrive = ch - 'A';
	if (m_ppsbCurDir[m_nCurDrive] == NULL) {
		m_ppsbCurDir[m_nCurDrive] = new StringBuffer(buf);
	} else {
		*m_ppsbCurDir[m_nCurDrive] = buf;
	}
	if (m_bTrueCD) // 実際にディレクトリを移動
		::SetCurrentDirectory(*m_ppsbCurDir[m_nCurDrive]);

	return (LPCSTR)*m_ppsbCurDir[m_nCurDrive];
}

