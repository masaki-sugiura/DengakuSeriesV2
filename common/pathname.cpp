//	$Id: pathname.cpp,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	pathname.cpp
 *	パス名を扱うクラス
 */

#include "misc.h"
#include "pathname.h"

const StringBuffer anyPathName("*.*");

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
PathName::getPathType(LPCSTR pathname, LPCSTR* pphead)
{
	if (!IsValidPtr(pathname)) return ISPATHINVALID;

	int type = 0; // return itself if null string
	if (*pathname == '\\') {
		//	1st char is "\"
		if (*++pathname == '\\') {
			//	UNC path ??
			type |= ISPATHREMOTE;
			int	len = GetTokenLength(++pathname,'\\');
			if (len <= 0)	//	"\\" or "\\\"
				return ISPATHINVALID;

			pathname += len; // skip server name
			type |= ISSERVERSPECIFIED;
			if (*pathname != '\0') { // "\\svr\.."
				len = GetTokenLength(++pathname,'\\');
				pathname += len;
				if (len == 0) {
					if (*pathname != '\0') return ISPATHINVALID; // "\\svr\\.."
					// else "\\svr\"
				} else { // "\\svr\share..."
					type |= ISSHARENAMESPECIFIED;
					if (*pathname != '\0') { // "\\svr\share\.."
						type |= ISPATHFROMROOT;
						pathname++;
					}
				}
			}
		} else {
			//	"\dir.." ??
			type |= ISPATHFROMROOT;
			pathname++;
		}
	} else if (*pathname != '\0') { // 1st. char is not "\"
		if (*(pathname + 1) == ':') {
			// maybe drive is specified
			if (!ISDRIVELETTER(*pathname)) return ISPATHINVALID;
			pathname += 2;
			//	"X:.."
			type |= ISDRIVESPECIFIED;
			if (*pathname == '\\') {
				type |= ISPATHFROMROOT;
				pathname++;
			}
		}
	}
	if (pphead != NULL) *pphead = pathname;
	return type | (*pathname != '\0' ? ISPATHSPECIFIED : 0);
}

void
PathName::refresh()
{
	LPCSTR phead = NULL;
	m_type = getPathType(*this, &phead);
	// calculates the number of nodes of path (without header)
	m_node = 0; // invalid, null, "X:\" or "\" or "\\svr[\[share[\]]]"
	if ((m_type & ISPATHSPECIFIED) != 0) {
		m_node++;
		while ((phead = lstrchr(phead,'\\')) != NULL && *++phead != '\0')
			m_node++;
		if (phead != NULL) this->setlength(this->length() - 1);
	}
	this->getFileType();
}

BOOL
PathName::isRootPath() const
{
	return m_node == 0 &&
			((m_type & ISPATHREMOTE) != 0 ||
			 ((m_type & ISDRIVESPECIFIED) != 0 &&
			  (m_type & ISPATHFROMROOT) != 0));
}

void
PathName::getFileType() const
{
	m_fd.dwFileAttributes = 0xFFFFFFFF;
	if (this->find('*') >= 0 || this->find('?') >= 0) return; // wild card
	if (this->isRootPath()) {
		if ((m_type & ISPATHREMOTE) != 0 &&
			(m_type & ISSHARENAMESPECIFIED) == 0)
			return; // "\\svr[\]"
		// ルートフォルダの場合 FindFirstFile() は失敗する(怒)
		m_fd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
	} else {
		HANDLE hFd = ::FindFirstFile((LPCSTR)*this,&m_fd);
		if (hFd != INVALID_HANDLE_VALUE) ::FindClose(hFd);
	}
}

PathName::PathName(const StringBuffer& name)
	: StringBuffer(name)
{
	this->refresh();
}

void
PathName::reset(const StringBuffer& newname, int head, int num)
{
	this->StringBuffer::reset(newname,head,num);
	this->refresh();
}

PathName&
PathName::addPath(const StringBuffer& name)
{
	if (name.length() > 0) {
		LPCSTR pname = name; // むぅ...
		if (*pname == '\\') ++pname;
		if (*pname != '\0') {
			if (!IsPathEndWithSep(*this))
				this->StringBuffer::append((TCHAR)'\\');
			this->StringBuffer::append(pname);
			this->refresh();
		}
	}
	return *this;
}

PathName&
PathName::delPath(int num)
{
	if (m_node > 0) {
		if (num < 0 || num > m_node) num = m_node;
		num = m_node - num;
		LPCSTR s = NULL;
		this->getPathType(*this, &s);
		if (s != NULL) {
			while (num-- > 0) {
				//	set s to the deleting "\"
				s = lstrchr(s,'\\');
				if (s == NULL) {
					InternalError(__FILE__, __LINE__);
					return *this;
				}
				s++;
			}
			this->StringBuffer::setlength(s - (LPCSTR)*this);
			this->refresh();
		}
	}
	return *this;
}

void
PathName::setBaseName(const StringBuffer& name)
{
	this->delPath(1);
	this->addPath(name);
}

