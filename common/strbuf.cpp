//	$Id: strbuf.cpp,v 1.4 2002-02-19 15:34:22 sugiura Exp $
/*
 *	strbuf.cpp
 *	文字列クラス
 */

#include <exception>

#include "strutils.h"
#include "strbuf.h"

#ifdef _DEBUG
#include <stdio.h>
static int s_id;
#endif

const StringBuffer nullStr("");

inline int
neg_to_zero(int x)
{
	return x & ~(x >> 31);
}

StringBuffer::StringBuffer_rep::StringBuffer_rep(int len)
	:	RCObject(),
#ifdef _DEBUG
		m_id(s_id++),
#endif
		m_len(0),
		m_buf((len > 0) ? (len + 1) : 1)
{
#ifdef _DEBUG
	fprintf(stdout,"create *%d of size %d\n",m_id,len);
#endif
	m_buf.zero(0, -1);
}

StringBuffer::StringBuffer_rep::StringBuffer_rep(
	LPCSTR str, int len, int exlen)
	: RCObject(),
	  m_len(neg_to_zero(len)),
	  m_buf(m_len +  neg_to_zero(exlen) + 1) // 仮のサイズ
#ifdef _DEBUG
	  , m_id(s_id++)
#endif
{
	this->init(str, len, exlen);
}

StringBuffer::StringBuffer_rep::StringBuffer_rep(const StringBuffer_rep& str)
	: RCObject(str),
	  m_buf(str.m_buf) // 仮のコピー
#ifdef _DEBUG
	  , m_id(s_id++)
#endif
{
	if (!str.isShareable()) {
		const_cast<StringBuffer_rep&>(str).recalc();
		this->init(str.m_buf, str.m_len, str.m_buf.size() - str.m_len - 1);
	}
}

StringBuffer::StringBuffer_rep::~StringBuffer_rep()
{
#ifdef _DEBUG
	fprintf(stdout,"delete %d\n",m_id);
#endif
}

void
StringBuffer::StringBuffer_rep::init(LPCSTR str, int len, int exlen)
{
	if (!IsValidPtr(str)) str = "";
	if (len < 0) len = lstrlen(str);
	if (exlen < 0) exlen = 0;
	m_buf.resize(len + exlen + 1);
	m_len = len;
	m_buf.copy(0, str, len);
	m_buf.zero(len, -1);
#ifdef _DEBUG
	fprintf(stdout,"create %d of str %s\n",m_id,str);
#endif
}

void
StringBuffer::StringBuffer_rep::recalc()
{
	if (m_buf == NULL) return;
	m_len = lstrlen(m_buf);
	if (m_len >= m_buf.size()) m_buf.resize(m_len + 1);	//	有り得ないと思うけど…
	this->markShareable();	//	再び共有可能にセット
}

void
StringBuffer::StringBuffer_rep::resize(int len)
{
	if (m_buf.size() <= len) {
		m_buf.resize(len + 1);
#ifdef _DEBUG
		int old_id = m_id;
		m_id = s_id++;
		fprintf(stdout, "delete %d\ncreate %d when resizing\n", old_id, m_id);
#endif
	}
	m_buf.zero(m_len, -1);
}


void
StringBuffer::dup()
{
	if (m_sbuf->isShared()) {
		//	バッファが共有されていれば複製を作成する
		m_sbuf = new StringBuffer_rep(m_sbuf->m_buf);
	}
}

StringBuffer::StringBuffer(int len)
{
	m_sbuf = new StringBuffer_rep(len);
}

StringBuffer::StringBuffer(LPCSTR str, int len, int exlen)
{
	m_sbuf = new StringBuffer_rep(str, len, exlen);
}

StringBuffer::~StringBuffer()
{
	//	nothing to do.
}

LPSTR
StringBuffer::getBufPtr()
{
	this->dup();	//	データを複製
	m_sbuf->markUnshareable();	//	何をされるか分からないので共有不可にセット

	return m_sbuf->m_buf;
}

void
StringBuffer::setlength(int len)
{
	//	文字列の長さは減らすだけ
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (m_sbuf->m_len > len && len >= 0) {
		this->dup();
		m_sbuf->m_len = len;
		m_sbuf->m_buf[m_sbuf->m_len] = '\0';
	}
}

TCHAR
StringBuffer::charAt(int at) const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (ABS(at) > m_sbuf->m_len) return '\0';
	else if (at < 0) at += m_sbuf->m_len;
	return m_sbuf->m_buf[at];
}

void
StringBuffer::setcharAt(int at, TCHAR ch)
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (at >= m_sbuf->m_len) return;
	else if (at < 0) at = m_sbuf->m_len - 1;
	if (ch == '\0') {
		//	ch == '\0' は setlength(at) と同じ
		this->setlength(at);
		return;
	}

	this->dup();

	m_sbuf->m_buf[at] = ch;
}

StringBuffer&
StringBuffer::append(LPCSTR str, int head, int len)
{
	if (!IsValidPtr(str) || *str == '\0') return *this;

	if (!m_sbuf->isShareable()) m_sbuf->recalc();

	int	slen = lstrlen(str);
	if (head < slen && head >= 0) {
		this->dup();
		if (len < 0 || len > slen) len = slen;
		if (head + len > slen) len = slen - head;
		m_sbuf->resize(m_sbuf->m_len + len);
		m_sbuf->m_buf.copy(m_sbuf->m_len, str + head, len);
		m_sbuf->m_len += len;
		m_sbuf->m_buf[m_sbuf->m_len] = '\0';
	}
	return *this;
}

StringBuffer&
StringBuffer::append(TCHAR ch)
{
	if (ch != '\0') {
		if (!m_sbuf->isShareable()) m_sbuf->recalc();
		this->dup();
		m_sbuf->resize(m_sbuf->m_len + 1);
		m_sbuf->m_buf[m_sbuf->m_len++] = ch;
		m_sbuf->m_buf[m_sbuf->m_len]   = '\0';
	}
	return *this;
}

StringBuffer&
StringBuffer::append(int i)
{
	TCHAR buf[32];
	wsprintf(buf,"%-d",i);
	return this->append(buf);
}

StringBuffer&
StringBuffer::append(long l)
{
	TCHAR buf[32];
	wsprintf(buf,"%-ld",l);
	return this->append(buf);
}

StringBuffer&
StringBuffer::append(ULONG ul)
{
	TCHAR buf[32];
	wsprintf(buf,"%-lu",ul);
	return this->append(buf);
}

void
StringBuffer::reset(LPCSTR str, int head, int len)
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	this->dup();
	m_sbuf->m_len = 0;
	m_sbuf->m_buf[0] = '\0';
	if (IsValidPtr(str)) this->append(str, head, len);
}

void
StringBuffer::reset(LPCWSTR wstr, int head, int len)
{
	if (!IsValidPtr((LPCSTR)wstr)) return;
	int size = ::WideCharToMultiByte(CP_ACP,
									WC_COMPOSITECHECK,
									wstr + head, len,
									NULL, 0,
									NULL, NULL);
	if (size == 0) return;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	this->dup();
	m_sbuf->resize(size);
	::WideCharToMultiByte(CP_ACP,
							WC_COMPOSITECHECK,
							wstr + head, len,
							m_sbuf->m_buf, size,
							NULL, NULL);
}

int
StringBuffer::toUnicode(LPWSTR wbuf) const
{
	if (wbuf == NULL) return -1;
	return ::MultiByteToWideChar(CP_ACP,
								MB_PRECOMPOSED,
								m_sbuf->m_buf,-1,
								wbuf, this->length() + 1);
}

int
StringBuffer::length() const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	return m_sbuf->m_len;
}

StringBuffer&
StringBuffer::toLower()
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	this->dup();
	for (LPSTR s = m_sbuf->m_buf; s != NULL && *s != '\0'; s = ToNextChar(s)) {
		if (IsCharUpperCase(*s)) *s += 0x20;
	}
	return *this;
}

StringBuffer&
StringBuffer::toUpper()
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	this->dup();
	for (LPSTR s = m_sbuf->m_buf; s != NULL && *s != '\0'; s = ToNextChar(s)) {
		if (IsCharLowerCase(*s)) *s -= 0x20;
	}
	return *this;
}

int
StringBuffer::length2() const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	return lstrlen2(m_sbuf->m_buf);
}

StringBuffer&
StringBuffer::toLower2()
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	this->dup();
	::CharLower(m_sbuf->m_buf);
	return *this;
}

StringBuffer&
StringBuffer::toUpper2()
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	this->dup();
	::CharUpper(m_sbuf->m_buf);
	return *this;
}

StringBuffer&
StringBuffer::replaceStr(LPCSTR ostr, LPCSTR dstr, int num)
{
	if (!IsValidPtr(ostr) || !IsValidPtr(dstr)) return *this;

	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	int	olen = lstrlen(ostr), dlen = lstrlen(dstr);
	LPSTR t = m_sbuf->m_buf, s;

	int n = 0;
	while ((s = lstrstr(t,ostr)) != NULL) {
		n++;
		t = s + olen;
	}
	if (num < 0 || n < num) num = n;

	int	newlen = m_sbuf->m_buf.size() + (dlen - olen) * num;
	StringBuffer_rep* newbuf = new StringBuffer_rep(newlen);

	LPSTR ptop;
	for (t = m_sbuf->m_buf, ptop = newbuf->m_buf;
		num-- > 0 && (s = lstrstr(t, ostr)) != NULL;
		t = s + olen, ptop += dlen) {
		::CopyMemory(ptop, t, s - t);
		ptop += s - t;
		::CopyMemory(ptop, dstr, dlen);
	}
	lstrcpy(ptop, t);
	newbuf->m_len = lstrlen(newbuf->m_buf);
	m_sbuf = newbuf;	//	RCPtr<StringBuffer> への代入
	return *this;
}

int
StringBuffer::compareTo(LPCSTR lpstr, BOOL casesense, int len) const
{
	if (!IsValidPtr(lpstr)) return *m_sbuf->m_buf;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (len >= 0) {
		return	casesense?
				lstrcmpn(m_sbuf->m_buf, lpstr, len):
				lstrcmpni(m_sbuf->m_buf, lpstr, len);
	} else {
		return	casesense?
				lstrcmp(m_sbuf->m_buf, lpstr):
				lstrcmpi(m_sbuf->m_buf, lpstr);
	}
}

int
StringBuffer::count(LPCSTR str) const
{
	if (!IsValidPtr(str)) return -1;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	int	num = 0, len = lstrlen(str);
	for (LPCSTR	s = m_sbuf->m_buf;
		(s = lstrstr(s, str)) != NULL;
		num++, s += len)
		/* no operation here */;
	return num;
}

int
StringBuffer::find(TCHAR ch) const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	LPCSTR	s = lstrchr(m_sbuf->m_buf, ch);
	return s != NULL ? (s - m_sbuf->m_buf) : -1;
}

int
StringBuffer::rfind(TCHAR ch) const
{
	if (ch == '\0') return m_sbuf->m_len;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
//	LPCSTR	s = lstrrchr(m_sbuf->m_buf,ch);
	LPCSTR t = m_sbuf->m_buf;
	for (LPCSTR s = t + m_sbuf->m_len - 1;
		 s > t;
		 s--) {
		if (*s == ch && !IsCharLeadByte(*(s-1)))
			return s - t;
	}
	return 1 - (*s == ch);
}

int
StringBuffer::find(LPCSTR lpstr) const
{
	if (!IsValidPtr(lpstr)) return -1;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	LPCSTR	s = lstrstr(m_sbuf->m_buf,lpstr);
	return s != NULL ? (s - m_sbuf->m_buf) : -1;
}

int
StringBuffer::rfind(LPCSTR lpstr) const
{
	if (!IsValidPtr(lpstr)) return -1;

	int len = lstrlen(lpstr);
	if (len == 0) return -1;

	if (!m_sbuf->isShareable()) m_sbuf->recalc();

	LPCSTR t = m_sbuf->m_buf;
	for (LPCSTR s = t + m_sbuf->m_len - 1;
		 s > t;
		 s--) {
		if (*s == *lpstr && !IsCharLeadByte(*(s-1)) &&
			lstrcmpn(s + 1, lpstr + 1, len - 1) == 0)
			return s - t;
	}
	return (lstrcmpn(s, lpstr, len) == 0) - 1;
}

StringBuffer
StringBuffer::extract(int head, int len) const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (head < 0) head = 0;
	else if (head >= m_sbuf->m_len) return nullStr;
	if (len < 0) len = m_sbuf->m_len;
	if (head + len > m_sbuf->m_len) len = m_sbuf->m_len - head;
	return StringBuffer(m_sbuf->m_buf + head, len);
}

int
StringBuffer::find2(LPCSTR lpstr) const
{
	if (!IsValidPtr(lpstr)) return -1;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	LPCSTR s = lstrstr(m_sbuf->m_buf, lpstr);
	if (s == NULL) return -1;
	LPCSTR t = m_sbuf->m_buf;
	int i = 0;
	while (t < s) {
		t = ToNextChar(t);
		i++;
	}
	return i;
}

int
StringBuffer::rfind2(LPCSTR lpstr) const
{
	if (!IsValidPtr(lpstr)) return -1;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();

	LPCSTR nstr = lstrstr(m_sbuf->m_buf, lpstr);
	if (nstr == NULL) return -1;
	LPCSTR str;
	for ( ; (str = lstrstr(ToNextChar(nstr), lpstr)) != NULL; nstr = str)
		/* no operation here */;
	int i = 0;
	for (str = m_sbuf->m_buf; str < nstr; str = ToNextChar(str))
		i++;
	return i;
}

StringBuffer
StringBuffer::extract2(int head, int len) const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	int	len2 = lstrlen2(m_sbuf->m_buf);
	if (head < 0) head = 0;
	else if (head >= len2) return nullStr;
	if (len < 0) len = len2;
	if (head+len > len2) len = len2 - head;
	LPCSTR	h = lstrninc(m_sbuf->m_buf, head),
			t = lstrninc(h, len);
	return StringBuffer(h, t - h);
}

