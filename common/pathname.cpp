//	$Id: pathname.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	pathname.cpp
 *	パス名を扱うクラス
 */

#include "pathname.h"

const StringBuffer anyPathName("*.*");

void
PathName::refresh()
{
	if (this->StringBuffer::charAt(-1) == '\\')
		this->StringBuffer::setcharAt(-1,'\0');
	if (this->StringBuffer::length() == 2) {
		this->StringBuffer::append((TCHAR)'\\');
		m_node = 0;
	} else {
		LPCSTR str = (LPCSTR)*this;
		for (m_node = 0; (str = lstrchr(str,'\\')) != NULL; m_node++, str++)
			/* no operation here */;
	}
	this->getFileType();
}

void
PathName::getFileType() const
{
	m_fd.dwFileAttributes = 0xFFFFFFFF;
	HANDLE hFd = ::FindFirstFile((LPCSTR)*this,&m_fd);
	if (hFd != INVALID_HANDLE_VALUE) ::FindClose(hFd);
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
			if (this->StringBuffer::charAt(-1) != '\\')
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
	if (this->StringBuffer::length() > 3) {
		if (num < 0 || num > m_node) num = m_node;
		num = m_node - num;
		LPCSTR s = (LPCSTR)*this;
		if (*s == '\\' && *(s + 1) == '\\') {
			//	UNC Path
			num -= 2;
			s += 2;	//	skip "\\" at head.
			s = lstrchr(s,'\\');	//	set s to the next "\"
			if (s == NULL) return *this;	//	no change.
		} else if (*(s + 1) == ':') {
			//	Absolute Path
			s += 2;
		}
		while (s != NULL && num-- > 0) {
			//	set s to the deleting "\"
			s = lstrchr(++s,'\\');
		}
		this->StringBuffer::setcharAt(s - (LPCSTR)*this,'\0');
		this->refresh();
	}
	return *this;
}

void
PathName::setBaseName(const StringBuffer& name)
{
	if (name.length() <= 0) return;
	LPSTR s = lstrrchr(*this,'\\');
	if (s == NULL) return;
	this->StringBuffer::setlength(++s - (LPCSTR)*this);
	this->StringBuffer::append(name);
	this->refresh();
}

