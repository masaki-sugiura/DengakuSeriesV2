//	$Id: strutils.h,v 1.2 2002-06-16 14:56:09 sugiura Exp $
/*
 *	strutils.h
 *	C 文字列のためのユーティリティ関数群
 */

#ifndef	DENGAKUSERIES_FUNC_STRUTILS
#define	DENGAKUSERIES_FUNC_STRUTILS

#include <windows.h>

BOOL	isnumber(LPCSTR);
int		ival(LPCSTR);
double	dval(LPCSTR);
int		bintoi(LPCSTR);
int		octtoi(LPCSTR);
int		hextoi(LPCSTR);
void	itooct(LPSTR, int);
void	itobin(LPSTR, int);
int		lstrcmpn(LPCSTR, LPCSTR, int);
int		lstrcmpni(LPCSTR, LPCSTR, int);
LPSTR	lstrchr(LPCSTR, TCHAR);
LPSTR	lstrrchr(LPCSTR, TCHAR);
LPSTR	lstrstr(LPCSTR, LPCSTR);
int		lstrlen2(LPCSTR);
LPSTR	lstrninc(LPCSTR, int);

#define	lval(num)	((long)ival(num))

#define IS_LOWERCASE 0x01
#define IS_UPPERCASE 0x02
#define IS_DIGIT     0x04
#define IS_XDIGIT    0x08
#define IS_SPACE     0x10
#define IS_LEADBYTE  0x20
#define IS_TRAILBYTE 0x40
#define IS_READABLE  0x80

extern UCHAR char_property[];

inline int
ABS(int i)
{
//	return (i >= 0) ? i : -i;
	int mask = i >> 31;
	i ^= mask;
	i -= mask;
	return i;
}

inline BOOL
IsCharUpperCase(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_UPPERCASE) != 0;
}

inline BOOL
IsCharLowerCase(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_LOWERCASE) != 0;
}

inline BOOL
IsCharAlphabet(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & (IS_UPPERCASE|IS_LOWERCASE)) != 0;
}

inline BOOL
IsCharDigit(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_DIGIT) != 0;
}

inline BOOL
IsCharXDigit(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_XDIGIT) != 0;
}

inline BOOL
IsCharSpace(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_SPACE) != 0;
}

inline BOOL
IsCharLeadByte(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_LEADBYTE) != 0;
}

inline BOOL
IsCharTrailByte(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_TRAILBYTE) != 0;
}

inline BOOL
IsCharReadable(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_READABLE) != 0;
}

inline BOOL
IsValidPtr(LPCSTR str)
{
	return (str != NULL && !::IsBadStringPtr(str,1));
}

inline LPSTR
ToNextChar(LPCSTR ptr)
{
	return (LPSTR)(ptr + 1 + IsCharLeadByte(*ptr));
}

#endif
