//	$Id: ss_func.cpp,v 1.4 2003-02-15 18:37:02 sugiura Exp $
/*
 *	ss_func.cpp
 *	文字列サービスの関数
 */

#include "DengakuDLL.h"

//	sprintf() 限定バージョン
DENGAKUDLL_API HIDEDLL_NUMTYPE
ATOI(LPCSTR val)
{
	try {
		return g_pSessionInstance->si_atoi(val);
	} catch (...) {
		return 0;
	}
}

//	sprintf() 限定バージョン
DENGAKUDLL_API LPCSTR
ITOA(HIDEDLL_NUMTYPE val, HIDEDLL_NUMTYPE radix)
{
	try {
		g_strBuffer = g_pSessionInstance->si_itoa(val,radix);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	stricmp()
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRICMP(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_stricmp(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	str1 中の str2 の出現数を返す
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRCOUNT(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_strcount(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	gsub() in awk
DENGAKUDLL_API LPCSTR
GSUB(LPCSTR str1, LPCSTR str2, LPCSTR str3, HIDEDLL_NUMTYPE num)
{
	try {
		g_strBuffer = g_pSessionInstance->si_gsub(str1,str2,str3,num);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	reverse
DENGAKUDLL_API LPCSTR
REVERSE(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_reverse(str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	tolower
DENGAKUDLL_API LPCSTR
TOLOWER(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_tolower(str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	toupper
DENGAKUDLL_API LPCSTR
TOUPPER(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_toupper(str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	strstr
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRSTR(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_strstr(str1,str2);
	} catch (...) {
		return -1;
	}
}

//	strrstr
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRRSTR(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_strrstr(str1,str2);
	} catch (...) {
		return -1;
	}
}

//	strlen
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRLEN(LPCSTR str)
{
	try {
		return g_pSessionInstance->si_strlen(str);
	} catch (...) {
		return 0;
	}
}

//	leftstr
DENGAKUDLL_API LPCSTR
LEFTSTR(LPCSTR str, HIDEDLL_NUMTYPE num)
{
	try {
		g_strBuffer = g_pSessionInstance->si_leftstr(str,num);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	midstr
DENGAKUDLL_API LPCSTR
MIDSTR(LPCSTR str, HIDEDLL_NUMTYPE num1, HIDEDLL_NUMTYPE num2)
{
	try {
		g_strBuffer = g_pSessionInstance->si_midstr(str,num1,num2);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	rightstr
DENGAKUDLL_API LPCSTR
RIGHTSTR(LPCSTR str, HIDEDLL_NUMTYPE num)
{
	try {
		g_strBuffer = g_pSessionInstance->si_rightstr(str,num);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	全角文字を意識して reverse
DENGAKUDLL_API LPCSTR
REVERSE2(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_reverse2(str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	全角文字も含めて tolower
DENGAKUDLL_API LPCSTR
TOLOWER2(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_tolower2(str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	全角文字も含めて toupper
DENGAKUDLL_API LPCSTR
TOUPPER2(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_toupper2(str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	出現位置を strlen2 風に数える strstr
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRSTR2(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_strstr2(str1,str2);
	} catch (...) {
		return -1;
	}
}

//	出現位置を strlen2 風に数える strrstr
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRRSTR2(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_strrstr2(str1,str2);
	} catch (...) {
		return -1;
	}
}

//	全角文字も１文字として数える strlen
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRLEN2(LPCSTR str)
{
	try {
		return g_pSessionInstance->si_strlen2(str);
	} catch (...) {
		return 0;
	}
}

//	文字数を strlen2 風に数える leftstr
DENGAKUDLL_API LPCSTR
LEFTSTR2(LPCSTR str, HIDEDLL_NUMTYPE num)
{
	try {
		g_strBuffer = g_pSessionInstance->si_leftstr2(str,num);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	文字数を strlen2 風に数える midstr
DENGAKUDLL_API LPCSTR
MIDSTR2(LPCSTR str, HIDEDLL_NUMTYPE num1, HIDEDLL_NUMTYPE num2)
{
	try {
		g_strBuffer = g_pSessionInstance->si_midstr2(str,num1,num2);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	文字数を strlen2 風に数える rightstr
DENGAKUDLL_API LPCSTR
RIGHTSTR2(LPCSTR str, HIDEDLL_NUMTYPE num)
{
	try {
		g_strBuffer = g_pSessionInstance->si_rightstr2(str,num);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	文字列の左側の空白を除去する
DENGAKUDLL_API LPCSTR
LTRIM(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_ltrim(str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	文字列の右側の空白を除去する
DENGAKUDLL_API LPCSTR
RTRIM(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_rtrim(str);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	str1 を str2 に含まれる文字で分割する strtok
DENGAKUDLL_API LPCSTR
GETTOKEN(LPCSTR str1, LPCSTR str2)
{
	try {
		g_strBuffer = g_pSessionInstance->si_gettoken(str1,str2);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	GETTOKEN() でまだトークンがあるかどうかを返す
DENGAKUDLL_API HIDEDLL_NUMTYPE
HASMORETOKENS()
{
	try {
		return g_pSessionInstance->si_hasmoretokens();
	} catch (...) {
		return 0;
	}
}

