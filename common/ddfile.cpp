//	$Id: ddfile.cpp,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	ddfile.cpp
 *	ダイアログデータのデータファイルを扱うクラス
 */

#include "strutils.h"
#include "ddfile.h"
#include "pathname.h"

DlgDataFile::DlgDataFile(const PathName& filename)
	:	m_filename(filename),
		m_secname(32)
{
	//	nothing to do more.
}

BOOL
DlgDataFile::isValid()
{
	return TRUE;	//	implement in future
}

void
DlgDataFile::setSecName(const StringBuffer& secname)
{
	m_secname = secname;
}

BOOL
DlgDataFile::read(int* buf, LPCSTR key, LPCSTR sec)
{
	if (key == NULL || *key == '\0') return FALSE;
	if (sec == NULL || *sec == '\0') sec = m_secname;
	TCHAR ibuf[16];
	::GetPrivateProfileString(sec,key,"",ibuf,16,m_filename);
	*buf = (*ibuf != '\0') ? ival(ibuf) : 0;
	return TRUE;
}

BOOL
DlgDataFile::read(WORD* buf, LPCSTR key, LPCSTR sec)
{
	int	ret;
	if (!this->read(&ret,key,sec)) return FALSE;
	*buf = (WORD)ret;
	return TRUE;
}

BOOL
DlgDataFile::read(DWORD* buf, LPCSTR key, LPCSTR sec)
{
	int	ret;
	if (!this->read(&ret,key,sec)) return FALSE;
	*buf = (DWORD)ret;
	return TRUE;
}

BOOL
DlgDataFile::read(StringBuffer& buf, LPCSTR key, LPCSTR sec)
{
	if (key == NULL || *key == '\0') return FALSE;
	if (sec == NULL || *sec == '\0') sec = m_secname;
	TCHAR sbuf[MAX_PATH];
	::GetPrivateProfileString(sec,key,"",sbuf,MAX_PATH,m_filename);
	buf.reset(sbuf);
	buf.replaceStr("\\n","\n");
	return TRUE;
}

BOOL
DlgDataFile::write(int buf, LPCSTR key, LPCSTR sec)
{
	if (key == NULL || *key == '\0') return FALSE;
	if (sec == NULL || *sec == '\0') sec = m_secname;
	TCHAR ibuf[16];
	wsprintf(ibuf,"%d",buf);
	return ::WritePrivateProfileString(sec,key,ibuf,m_filename);
}

BOOL
DlgDataFile::write(WORD buf, LPCSTR key, LPCSTR sec)
{
	return this->write((int)buf,key,sec);
}

BOOL
DlgDataFile::write(DWORD buf, LPCSTR key, LPCSTR sec)
{
	return this->write((int)buf,key,sec);
}

BOOL
DlgDataFile::write(LPCSTR buf, LPCSTR key, LPCSTR sec)
{
	if (key == NULL || *key == '\0') return FALSE;
	if (sec == NULL || *sec == '\0') sec = m_secname;
	StringBuffer sbuf(buf,-1,20);
	sbuf.replaceStr("\n","\\n");
	return ::WritePrivateProfileString(sec,key,sbuf,m_filename);
}

