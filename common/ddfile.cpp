//	$Id: ddfile.cpp,v 1.3 2002-02-28 15:32:30 sugiura Exp $
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
DlgDataFile::read(int* buf, const StringBuffer& key, const StringBuffer& sec)
{
	if (key.length() == 0) return FALSE;
	TCHAR ibuf[16];
	::GetPrivateProfileString(sec.length() > 0 ? sec : m_secname,
							  key,
							  "",
							  ibuf,
							  16,
							  m_filename);
	*buf = ival(ibuf);
	return TRUE;
}

BOOL
DlgDataFile::read(WORD* buf, const StringBuffer& key, const StringBuffer& sec)
{
	int	ret;
	if (!this->read(&ret, key, sec)) return FALSE;
	*buf = (WORD)ret;
	return TRUE;
}

BOOL
DlgDataFile::read(DWORD* buf, const StringBuffer& key, const StringBuffer& sec)
{
	return this->read((int*)buf, key, sec);
}

BOOL
DlgDataFile::read(StringBuffer& buf, const StringBuffer& key, const StringBuffer& sec)
{
	if (key.length() == 0) return FALSE;
	TCHAR sbuf[MAX_PATH];
	::GetPrivateProfileString(sec.length() > 0 ? sec : m_secname,
							  key,
							  "",
							  sbuf,
							  MAX_PATH,
							  m_filename);
	buf.reset(sbuf);
	buf.replaceStr("\\n", "\n");
	return TRUE;
}

BOOL
DlgDataFile::write(int buf, const StringBuffer& key, const StringBuffer& sec)
{
	if (key.length() == 0) return FALSE;
	TCHAR ibuf[16];
	wsprintf(ibuf, "%d", buf);
	return ::WritePrivateProfileString(sec.length() > 0 ? sec : m_secname,
									   key,
									   ibuf,
									   m_filename);
}

BOOL
DlgDataFile::write(WORD buf, const StringBuffer& key, const StringBuffer& sec)
{
	return this->write((int)buf, key, sec);
}

BOOL
DlgDataFile::write(DWORD buf, const StringBuffer& key, const StringBuffer& sec)
{
	return this->write((int)buf, key, sec);
}

BOOL
DlgDataFile::write(
	const StringBuffer& buf,
	const StringBuffer& key,
	const StringBuffer& sec)
{
	if (key.length() == 0) return FALSE;
	StringBuffer sbuf(buf, -1, 20);
	sbuf.replaceStr("\n", "\\n");
	return ::WritePrivateProfileString(sec.length() > 0 ? sec : m_secname,
									   key,
									   sbuf,
									   m_filename);
}

