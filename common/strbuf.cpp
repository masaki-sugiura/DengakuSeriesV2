//	$Id: strbuf.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	strbuf.cpp
 *	������N���X
 */

#include <exception>
#include "strbuf.h"

#ifdef _DEBUG
#include <stdio.h>
static int s_id;
#endif

const StringBuffer nullStr = "";

UCHAR char_property[257] = {
	// 0x00 - 0x07
	0, 0, 0, 0, 0, 0, 0, 0,
	// 0x08, HT, LF, VT, FF, CR, 0x0e, 0x0f
	0, IS_SPACE|IS_READABLE, IS_SPACE|IS_READABLE, IS_SPACE|IS_READABLE,
	IS_SPACE|IS_READABLE, IS_SPACE|IS_READABLE, 0, 0,
	//	0x10 - 0x1f
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	// SPACE, !, ", #
	IS_SPACE|IS_READABLE, IS_READABLE, IS_READABLE, IS_READABLE,
	// $, %, &, '
	IS_READABLE, IS_READABLE, IS_READABLE, IS_READABLE,
	// (, ), *, +
	IS_READABLE, IS_READABLE, IS_READABLE, IS_READABLE,
	// ,, -, ., /
	IS_READABLE, IS_READABLE, IS_READABLE, IS_READABLE,
	// 0 - 9
	IS_DIGIT|IS_XDIGIT|IS_READABLE, IS_DIGIT|IS_XDIGIT|IS_READABLE,
	IS_DIGIT|IS_XDIGIT|IS_READABLE, IS_DIGIT|IS_XDIGIT|IS_READABLE,
	IS_DIGIT|IS_XDIGIT|IS_READABLE, IS_DIGIT|IS_XDIGIT|IS_READABLE,
	IS_DIGIT|IS_XDIGIT|IS_READABLE, IS_DIGIT|IS_XDIGIT|IS_READABLE,
	IS_DIGIT|IS_XDIGIT|IS_READABLE, IS_DIGIT|IS_XDIGIT|IS_READABLE,
	// :, ;, <, =, >, ?
	IS_READABLE, IS_READABLE,
	IS_READABLE, IS_READABLE, IS_READABLE, IS_READABLE,
	// @
	IS_TRAILBYTE|IS_READABLE,
	// A - Z
	IS_UPPERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_UPPERCASE|IS_TRAILBYTE|IS_READABLE,
	// [, \, ], ^, _, `
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	// a - z
	IS_LOWERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_XDIGIT|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	IS_LOWERCASE|IS_TRAILBYTE|IS_READABLE,
	// {, |, }, ~, DEL
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE,
	// 0x80 - 0x9f
	IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	// 0xa0 - 0xdf
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE, IS_TRAILBYTE|IS_READABLE,
	// 0xe0 - 0xff
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE,
	IS_TRAILBYTE|IS_READABLE,
	0 // dummy
};


BOOL
isnumber(LPCSTR str)
{
	if (!IsValidPtr(str)) return FALSE;
	for ( ; *str != '\0'; str++) {
		if (!IsCharDigit(*str)) return FALSE;
	}
	return TRUE;
}

int
ival(LPCSTR num)
{
	int	ret = 0, sgn = 1;
	if (IsValidPtr(num) && *num != '\0') {
		while (IsCharSpace(*num)) num++;
		if (*num == '-') sgn = -1, num++;
		while (IsCharDigit(*num))
			ret = ret*10 + (*num++ - '0');
		ret *= sgn;
	}
	return ret;
}

double
dval(LPCSTR num)
{
	double ret = 0.0;
	if (IsValidPtr(num) && *num != '\0') {
		int	sgn = 1;
		while (IsCharSpace(*num)) num++;
		if (*num == '-') sgn = -1, num++;
		else if (*num == '+') num++;
		while (IsCharDigit(*num))
			ret += ret * 10.0 + (*num++ - '0');
		if (*num == '.') {
			num++;
			double deg = 0.1;
			while (IsCharDigit(*num)) {
				ret += deg * (*num++ - '0');
				deg /= 10.0;
			}
		}
		ret *= sgn;
	}
	return ret;
}

int
bintoi(LPCSTR val)
{
	if (!IsValidPtr(val)) return 0;
	if (val[0] == '0' && (val[1] == 'b' || val[1] == 'B')) val += 2;
	int ret = 0;
	while (*val != '\0') {
		ret <<= 1;
		ret += *val - '0';
		val++;
	}
	return ret;
}

int
octtoi(LPCSTR val)
{
	if (!IsValidPtr(val)) return 0;
	int ret = 0;
	while (*val != '\0') {
		ret <<= 3;
		ret += *val - '0';
		val++;
	}
	return ret;
}

int
hextoi(LPCSTR val)
{
	if (!IsValidPtr(val)) return 0;
	if (val[0] == '0' && (val[1] == 'x' || val[1] == 'X')) val += 2;
	int ret = 0;
	while (*val != '\0') {
		ret <<= 4;
		if (IsCharDigit(*val)) ret += *val - '0';
		else if (*val >= 'a' && *val <= 'f') ret += *val - 'a' + 10;
		else ret += *val - 'A' + 10;
		val++;
	}
	return ret;
}

void
itooct(LPSTR buf, int val)
{
	int idx = 0;
	BOOL bShowZero = FALSE;
	for (int i = 10; i >= 0; --i) {
		int v = ((unsigned int)val >> (i * 3)) & 0x00000007;
		if (bShowZero || v != 0) {
			bShowZero = TRUE;
			buf[idx++] = '0' + v;
		}
	}
	if (idx == 0) buf[idx++] = '0';
	buf[idx] = '\0';
}

void
itobin(LPSTR buf, int val)
{
	int idx = 0;
	BOOL bShowZero = FALSE;
	for (int i = 31; i >= 0; --i) {
		int v = (val >> i) & 0x00000001;
		if (bShowZero || v != 0) {
			bShowZero = TRUE;
			buf[idx++] = '0' + v;
		}
	}
	buf[idx] = '\0';
}

int
lstrcmpn(LPCSTR lps1, LPCSTR lps2, int num)
{
	if (!IsValidPtr(lps1) || !IsValidPtr(lps2)) return 0;
	while (--num > 0 && *lps1 != '\0' && *lps2 != '\0' && *lps1 == *lps2) {
		lps1++;  lps2++;
	}
	return *lps1 - *lps2;
}

#define	CODE(ch) (IsCharLowerCase(ch) ? (ch) - 0x20 : (ch))

int
lstrcmpni(LPCSTR lps1, LPCSTR lps2, int num)
{
	if (!IsValidPtr(lps1) || !IsValidPtr(lps2)) return 0;
	while (--num > 0 && *lps1 != '\0' && *lps2 != '\0' &&
		CODE(*lps1) == CODE(*lps2)) {
		lps1++;  lps2++;
	}
	return CODE(*lps1) - CODE(*lps2);
}

LPSTR
lstrchr(LPCSTR str, TCHAR ch)
{
	if (!IsValidPtr(str)) return NULL;
	if (ch == '\0')
		return const_cast<LPSTR>(str + lstrlen(str));

	while (*str != '\0') {
		if (*str == ch) return const_cast<LPSTR>(str);
		str = ToNextChar(str);
	}
	return NULL;
}

LPSTR
lstrrchr(LPCSTR str, TCHAR ch)
{
	if (!IsValidPtr(str)) return NULL;
	if (ch == '\0')
		return const_cast<LPSTR>(str + lstrlen(str));

	LPCSTR ret = NULL;
	while (*str != '\0') {
		if (*str == ch) ret = str;
		str = ToNextChar(str);
	}
	return const_cast<LPSTR>(ret);
}

LPSTR
lstrstr(LPCSTR str1, LPCSTR str2)
{
	if (!IsValidPtr(str1) || !IsValidPtr(str2)) return NULL;
	int	len = lstrlen(str2);
	while ((str1 = lstrchr(str1,*str2)) != NULL && *str1 != '\0') {
		if (lstrcmpn(str1,str2,len) == 0) return const_cast<LPSTR>(str1);
		str1 = ToNextChar(str1);
	}
	return NULL;
}

int
lstrlen2(LPCSTR str)
{
	if (!IsValidPtr(str)) return 0;
	int len = 0;
	for ( ; str != NULL && *str != '\0'; str = ToNextChar(str))
		len++;
	return len;
}

LPSTR
lstrninc(LPCSTR str, int num)
{
	if (!IsValidPtr(str)) return NULL;
	while (num-- > 0) str = ToNextChar(str);
	return const_cast<LPSTR>(str);
}


StringBuffer::StringBuffer_rep::StringBuffer_rep(int len)
	:	RCObject(),
#ifdef _DEBUG
		m_id(s_id++),
#endif
		m_bufsize((len > 0) ? len : 0),
		m_len(0),
		m_buf(new TCHAR[m_bufsize+1])
{
#ifdef _DEBUG
	fprintf(stdout,"create *%d of size %d\n",m_id,len);
#endif
	::ZeroMemory(m_buf,m_bufsize+1);
}

StringBuffer::StringBuffer_rep::StringBuffer_rep(
	LPCSTR str, int len, int exlen)
	: RCObject()
#ifdef _DEBUG
	  , m_id(s_id++)
#endif
{
	this->init(str,len,exlen);
}

StringBuffer::StringBuffer_rep::StringBuffer_rep(const StringBuffer_rep& str)
	: RCObject(str)
#ifdef _DEBUG
	  , m_id(s_id++)
#endif
{
	if (!str.isShareable()) const_cast<StringBuffer_rep&>(str).recalc();
	this->init(str.m_buf, str.m_len, str.m_bufsize - str.m_len);
}

StringBuffer::StringBuffer_rep::~StringBuffer_rep()
{
#ifdef _DEBUG
	fprintf(stdout,"delete %d\n",m_id);
#endif
	delete [] m_buf;
}

void
StringBuffer::StringBuffer_rep::init(LPCSTR str, int len, int exlen)
{
	if (!IsValidPtr(str)) str = "";
	if (len < 0) len = lstrlen(str);
	if (exlen < 0) exlen = 0;
	m_bufsize = len + exlen;
	m_buf = new TCHAR[m_bufsize + 1];
	::ZeroMemory(m_buf,m_bufsize + 1);
	m_len = len;
	::CopyMemory(m_buf,str,len);
#ifdef _DEBUG
	fprintf(stdout,"create %d of str %s\n",m_id,str);
#endif
}

void
StringBuffer::StringBuffer_rep::recalc()
{
	if (m_buf == NULL) return;
	m_len = lstrlen(m_buf);
	if (m_len > m_bufsize) m_bufsize = m_len + 1;	//	�L�蓾�Ȃ��Ǝv�����ǁc
	this->markShareable();	//	�Ăы��L�\�ɃZ�b�g
}

void
StringBuffer::StringBuffer_rep::resize(int len)
{
	if (m_bufsize < len) {
		LPSTR m_buftmp;
		try {
			m_buftmp = new TCHAR[len + 1];
		} catch (exception&) {
			return;
		}
		m_bufsize = len;
		::CopyMemory(m_buftmp,m_buf,m_len);
		delete [] m_buf;
		m_buf = m_buftmp;
#ifdef _DEBUG
		int old_id = m_id;
		m_id = s_id++;
		fprintf(stdout,"delete %d\ncreate %d when resizing\n",old_id,m_id);
#endif
	}
	::ZeroMemory(m_buf + m_len, m_bufsize + 1 - m_len);
}


void
StringBuffer::dup()
{
	if (m_sbuf->isShared()) {
		//	�o�b�t�@�����L����Ă���Ε������쐬����
		m_sbuf = new StringBuffer_rep(m_sbuf->m_buf,
									m_sbuf->m_len,
									m_sbuf->m_bufsize - m_sbuf->m_len);
	}
}

StringBuffer::StringBuffer(int len)
{
	m_sbuf = new StringBuffer_rep(len);
}

StringBuffer::StringBuffer(LPCSTR str, int len, int exlen)
{
	m_sbuf = new StringBuffer_rep(str,len,exlen);
}

StringBuffer::~StringBuffer()
{
	//	nothing to do.
}

LPSTR
StringBuffer::getBufPtr()
{
	this->dup();	//	�f�[�^�𕡐�
	m_sbuf->markUnshareable();	//	��������邩������Ȃ��̂ŋ��L�s�ɃZ�b�g

	return m_sbuf->m_buf;
}

void
StringBuffer::setlength(int len)
{
	//	������̒����͌��炷����
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (m_sbuf->m_len > len && len >= 0) {
		this->dup();
		m_sbuf->m_len = len;
		*(m_sbuf->m_buf + m_sbuf->m_len) = '\0';
	}
}

TCHAR
StringBuffer::charAt(int at) const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (ABS(at) > m_sbuf->m_len) return '\0';
	else if (at < 0) at += m_sbuf->m_len;
	return *(m_sbuf->m_buf + at);
}

void
StringBuffer::setcharAt(int at, TCHAR ch)
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (at >= m_sbuf->m_len) return;
	else if (at < 0) at = m_sbuf->m_len - 1;
	if (ch == '\0') {
		//	ch == '\0' �� setlength(at) �Ɠ���
		this->setlength(at);
		return;
	}

	this->dup();

	*(m_sbuf->m_buf + at) = ch;
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
		::CopyMemory(m_sbuf->m_buf + m_sbuf->m_len, str + head, len + 1);
		m_sbuf->m_len += len;
		*(m_sbuf->m_buf + m_sbuf->m_len) = '\0';
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
		*(m_sbuf->m_buf + m_sbuf->m_len++) = ch;
		*(m_sbuf->m_buf + m_sbuf->m_len) = '\0';
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
	*m_sbuf->m_buf = '\0';
	if (IsValidPtr(str)) this->append(str,head,len);
}

void
StringBuffer::reset(LPCWSTR wstr, int head, int len)
{
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

	int	newlen = m_sbuf->m_bufsize + (dlen - olen) * num;
	StringBuffer_rep* newbuf;
	try {
		newbuf = new StringBuffer_rep(newlen);
	} catch (exception&) {
		return *this;
	}
	LPSTR ptop;
	for (t = m_sbuf->m_buf, ptop = newbuf->m_buf;
		num-- > 0 && (s = lstrstr(t,ostr)) != NULL;
		t = s + olen, ptop += dlen) {
		::CopyMemory(ptop,t,s - t);
		ptop += s - t;
		::CopyMemory(ptop,dstr,dlen);
	}
	lstrcpy(ptop,t);
	newbuf->m_len = lstrlen(newbuf->m_buf);
	m_sbuf = newbuf;	//	RCPtr<StringBuffer> �ւ̑��
	return *this;
}

int
StringBuffer::compareTo(LPCSTR lpstr, BOOL casesense, int len) const
{
	if (!IsValidPtr(lpstr)) return *m_sbuf->m_buf;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	if (len >= 0) {
		return	casesense?
				lstrcmpn(m_sbuf->m_buf,lpstr,len):
				lstrcmpni(m_sbuf->m_buf,lpstr,len);
	} else {
		return	casesense?
				lstrcmp(m_sbuf->m_buf,lpstr):
				lstrcmpi(m_sbuf->m_buf,lpstr);
	}
}

int
StringBuffer::count(LPCSTR str) const
{
	if (!IsValidPtr(str)) return -1;
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	int	num = 0, len = lstrlen(str);
	for (LPCSTR	s = m_sbuf->m_buf;
		(s = lstrstr(s,str)) != NULL;
		num++, s += len)
		/* no operation here */;
	return num;
}

int
StringBuffer::find(TCHAR ch) const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	LPCSTR	s = lstrchr(m_sbuf->m_buf,ch);
	return s != NULL ? (s - m_sbuf->m_buf) : -1;
}

int
StringBuffer::rfind(TCHAR ch) const
{
	if (!m_sbuf->isShareable()) m_sbuf->recalc();
	LPCSTR	s = lstrrchr(m_sbuf->m_buf,ch);
	return s != NULL ? (s - m_sbuf->m_buf) : -1;
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
	if (!m_sbuf->isShareable()) m_sbuf->recalc();

	LPCSTR nstr = lstrstr(m_sbuf->m_buf,lpstr);
	if (nstr == NULL) return -1;
	LPCSTR str;
	for ( ; (str = lstrstr(ToNextChar(nstr),lpstr)) != NULL; nstr = str)
		/* no operation here */;
	return nstr - m_sbuf->m_buf;
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
	LPCSTR s = lstrstr(m_sbuf->m_buf,lpstr);
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

	LPCSTR nstr = lstrstr(m_sbuf->m_buf,lpstr);
	if (nstr == NULL) return -1;
	LPCSTR str;
	for ( ; (str = lstrstr(ToNextChar(nstr),lpstr)) != NULL; nstr = str)
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
	LPCSTR	h = lstrninc(m_sbuf->m_buf,head),
			t = lstrninc(h,len);
	return StringBuffer(h,t - h);
}
