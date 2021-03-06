//	$Id: strutils.cpp,v 1.6 2007/02/10 19:00:02 sugiura Exp $
/*
 *	strutils.cpp
 *	C 文字列のためのユーティリティ関数群の実装
 */

#include "strutils.h"

//	文字の性質をビットで表現した配列
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
	IS_READABLE, // 0x7f
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
	// 0xe0 - 0xef
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	// 0xf0 - 0xff
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE, IS_LEADBYTE|IS_TRAILBYTE|IS_READABLE,
	0, // 0xfd
	0, // 0xfe
	0, // 0xff
	0 // dummy
};


BOOL
isnumber(LPCSTR str)
{
	if (!IsValidPtr(str)) return FALSE;
	while (*str != '\0') {
		if (!IsCharDigit(*str++)) return FALSE;
	}
	return TRUE;
}

int
ival(LPCSTR num)
{
	int	ret = 0, sgn = 0;
	if (IsValidPtr(num) && *num != '\0') {
		while (IsCharSpace(*num)) num++;
		if (*num == '-') sgn = -1, num++;
		while (IsCharDigit(*num))
			ret = ret * 10 + (*num++ - '0');
//		if (sgn < 0) ret *= -1;
		ret ^= sgn;
		ret -= sgn;
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
	while (*val == '0' || *val == '1') {
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
	int ret = 0, n;
	while ((n = *val - '0') >= 0 && n <= 7) {
		ret <<= 3;
		ret += n;
		val++;
	}
	return ret;
}

int
hextoi(LPCSTR val)
{
	if (!IsValidPtr(val)) return 0;
	if (val[0] == '0' && (val[1] == 'x' || val[1] == 'X')) val += 2;
	int ret = 0, n;
	for (;;) {
		if      (*val >= '0' && *val <= '9') n = *val - '0';
		else if (*val >= 'a' && *val <= 'f') n = *val - 'a' + 10;
		else if (*val >= 'A' && *val <= 'F') n = *val - 'A' + 10;
		else break;
		ret <<= 4;
		ret += n;
		val++;
	}
	return ret;
}

void
itooct(LPSTR buf, int val)
{
	int idx = 0, i, v;
	BOOL bShowZero = FALSE;
	for (i = 30; i >= 0; i -= 3) {
		v = ((unsigned int)val >> i) & 0x00000007;
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
	int idx = 0, i, v;
	BOOL bShowZero = FALSE;
	for (i = 31; i >= 0; --i) {
		v = (val >> i) & 0x00000001;
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

static inline int
CODE(char ch)
{
	return IsCharLowerCase(ch) ? (ch & 0x5F) : ch;
}

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
	for ( ; *str != '\0'; str = ToNextChar(str))
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

LPSTR
reverse(LPSTR str)
{
	if (!IsValidPtr(str)) return NULL;
	int num = lstrlen(str);
	if (num <= 1) return str;

	int hnum = num / 2;
	for (int i = 0; i < hnum; i++) {
		TCHAR ch = str[i];
		str[i] = str[num - i - 1];
		str[num - i - 1] = ch;
	}

	return str;
}

LPSTR
reverse2(LPSTR str)
{
	if (!IsValidPtr(str)) return NULL;
	int num = lstrlen(str);
	if (num <= 1) return str;

#if 0
	LPSTR buf = new TCHAR[num];
	int offset = num;
	LPCSTR ptr = str;
	while (ptr - str < num) {
		if (IsCharLeadByte(*ptr) && num - (ptr - str) >= 2) {
			// ２バイト文字かつ残りが２バイト以上
			offset -= 2;
			buf[offset] = *ptr++;
			buf[offset + 1] = *ptr++;
		} else {
			// １バイト文字または２バイト文字の先頭のみ
			buf[--offset] = *ptr++;
		}
	}

	::CopyMemory(str, buf, num);
	delete [] buf;
#else
	// とりあえず何も考えずに反転
	int hnum = num / 2, i;
	for (i = 0; i < hnum; i++) {
		TCHAR ch = str[i];
		str[i] = str[num - i - 1];
		str[num - i - 1] = ch;
	}

	// ２バイト文字のバイト順序を入れ替え
	for (i = num - 1; i > 0; --i) {
		if (IsCharLeadByte(str[i])) {
			TCHAR ch = str[i];
			str[i] = str[i - 1];
			str[i - 1] = ch;
			--i;
		}
	}
#endif

	return str;
}

int
tohankaku(LPCSTR str, LPSTR buf, int bufsize)
{
	return ::LCMapString(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						 SORT_DEFAULT),
						 LCMAP_HALFWIDTH | LCMAP_KATAKANA,
						 str, -1,
						 buf, bufsize) - 1;
}

int
tozenkakuhira(LPCSTR str, LPSTR buf, int bufsize)
{
	return ::LCMapString(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						 SORT_DEFAULT),
						 LCMAP_FULLWIDTH | LCMAP_HIRAGANA,
						 str, -1,
						 buf, bufsize) - 1;
}

int
tozenkakukata(LPCSTR str, LPSTR buf, int bufsize)
{
	return ::LCMapString(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						 SORT_DEFAULT),
						 LCMAP_FULLWIDTH | LCMAP_KATAKANA,
						 str, -1,
						 buf, bufsize) - 1;
}

