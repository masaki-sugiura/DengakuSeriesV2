//	$Id: si_string.cpp,v 1.4 2003-07-06 16:27:46 sugiura Exp $
/*
 *	si_string.cpp
 *	文字列サービスの関数
 */

#include "session.h"
#include "cmdline.h"
#include "tokenizer.h"

int
SessionInstance::si_atoi(const StringBuffer& val)
{
	if (val[0] == '0') {
		if (val.compareTo("0x", FALSE, 2) == 0)
			return hextoi(val);
		else if (val.compareTo("0b", FALSE, 2) == 0)
			return bintoi(val);
		else
			return octtoi(val);
	} else {
		return ival(val);
	}
}

StringBuffer
SessionInstance::si_itoa(int val, int radix)
{
	TCHAR buf[34];
	switch (radix) {
	case 16:
		wsprintf(buf,"%x",val);
		break;
	case 8:
		itooct(buf,val);
		break;
	case 2:
		itobin(buf,val);
		break;
	default:
		wsprintf(buf,"%d",val);
		break;
	}
	return buf;
}

int
SessionInstance::si_stricmp(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	return str1.compareTo(str2,FALSE);
}

int
SessionInstance::si_strcount(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	return str1.count(str2);
}

StringBuffer
SessionInstance::si_gsub(
	const StringBuffer& orgstr,
	const StringBuffer& oldstr,
	const StringBuffer& newstr,
	int num)
{
	return StringBuffer(orgstr).replaceStr(oldstr,newstr,num);
}

StringBuffer
SessionInstance::si_reverse(const StringBuffer& str)
{
	return StringBuffer(str).reverse();
}

StringBuffer
SessionInstance::si_tolower(const StringBuffer& str)
{
	return StringBuffer(str).toLower();
}

StringBuffer
SessionInstance::si_toupper(const StringBuffer& str)
{
	return StringBuffer(str).toUpper();
}

int
SessionInstance::si_strstr(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	return str1.find(str2);
}

int
SessionInstance::si_strrstr(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	return str1.rfind(str2);
}

int
SessionInstance::si_strlen(const StringBuffer& str)
{
	return str.length();
}

StringBuffer
SessionInstance::si_leftstr(const StringBuffer& str, int num)
{
	return StringBuffer(str).extract(0,num);
}

StringBuffer
SessionInstance::si_midstr(const StringBuffer& str, int head, int len)
{
	return str.extract(head,len);
}

StringBuffer
SessionInstance::si_rightstr(const StringBuffer& str, int ext_size)
{
	int head = str.length() - ext_size;
	return str.extract(~(head >> 31) & head, ext_size);
}

StringBuffer
SessionInstance::si_reverse2(const StringBuffer& str)
{
	return StringBuffer(str).reverse2();
}

StringBuffer
SessionInstance::si_tolower2(const StringBuffer& str)
{
	return StringBuffer(str).toLower2();
}

StringBuffer
SessionInstance::si_toupper2(const StringBuffer& str)
{
	return StringBuffer(str).toUpper2();
}

StringBuffer
SessionInstance::si_tohankaku(const StringBuffer& str)
{
	return StringBuffer(str).toHankaku();
}

StringBuffer
SessionInstance::si_tozenkakuhira(const StringBuffer& str)
{
	return StringBuffer(str).toZenkakuHira();
}

StringBuffer
SessionInstance::si_tozenkakukata(const StringBuffer& str)
{
	return StringBuffer(str).toZenkakuKata();
}

int
SessionInstance::si_strstr2(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	return str1.find2(str2);
}

int
SessionInstance::si_strrstr2(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	return str1.rfind2(str2);
}

int
SessionInstance::si_strlen2(const StringBuffer& str)
{
	return str.length2();
}

StringBuffer
SessionInstance::si_leftstr2(const StringBuffer& str, int num)
{
	return str.extract2(0,num);
}

StringBuffer
SessionInstance::si_midstr2(const StringBuffer& str, int head, int len)
{
	return str.extract2(head,len);
}

StringBuffer
SessionInstance::si_rightstr2(const StringBuffer& str, int ext_size)
{
	int head = str.length2() - ext_size;
	return str.extract2(~(head >> 31) & head, ext_size);
}

StringBuffer
SessionInstance::si_ltrim(const StringBuffer& str)
{
	LPCSTR ptr = str;
	while (IsCharSpace(*ptr)) ptr++;
	return str.extract(ptr - (LPCSTR)str, str.length());
}

StringBuffer
SessionInstance::si_rtrim(const StringBuffer& str)
{
	int	len = str.length();
	LPCSTR head = str;
	LPCSTR tail = head + len;
	while (--tail >= head) {
		if (!IsCharSpace(*tail) ||
			tail > head &&
			IsCharLeadByte(*(tail - 1))) {
			break;
		}
	}
	return ++tail > head ? str.extract(0, tail - head) : nullStr;
}

StringBuffer
SessionInstance::si_gettoken(
	const StringBuffer& str1,
	const StringBuffer& str2)
{
	if (str1.length() <= 0) {
		if (m_pStrToken.ptr() != NULL) {
			m_pStrToken->setSeparatorChars(str2);
			return m_pStrToken->getNextToken();
		}
		return nullStr;
	} else {
		m_pStrToken = new Tokenizer(str1,str2);
		return m_pStrToken->getNextToken();
	}
	//	not reached.
}

int
SessionInstance::si_hasmoretokens()
{
	return m_pStrToken.ptr() ? m_pStrToken->hasMoreTokens() : 0;
}

