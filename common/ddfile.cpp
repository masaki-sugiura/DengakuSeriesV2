//	$Id: ddfile.cpp,v 1.4 2005/01/18 13:52:08 sugiura Exp $
/*
 *	ddfile.cpp
 *	�_�C�A���O�f�[�^�̃f�[�^�t�@�C���������N���X
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
#ifdef _DEBUG
	char msgbuf[256];
	wsprintf(msgbuf, "sec=%s, key=%s, value=%s\n", (LPCSTR)(sec.length() > 0 ? sec : m_secname),
			 (LPCSTR)key, ibuf);
	::OutputDebugString(msgbuf);
#endif
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
#ifdef _DEBUG
	char msgbuf[256];
	wsprintf(msgbuf, "sec=%s, key=%s, value=%s\n", (LPCSTR)(sec.length() > 0 ? sec : m_secname),
			 (LPCSTR)key, (LPCSTR)sbuf);
	::OutputDebugString(msgbuf);
#endif
	return ::WritePrivateProfileString(sec.length() > 0 ? sec : m_secname,
									   key,
									   sbuf,
									   m_filename);
}

