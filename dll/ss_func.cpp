//	$Id: ss_func.cpp,v 1.2 2002-01-16 16:31:04 sugiura Exp $
/*
 *	ss_func.cpp
 *	������T�[�r�X�̊֐�
 */

#include "DengakuDLL.h"

//	sprintf() ����o�[�W����
DENGAKUDLL_API HIDEDLL_NUMTYPE
ATOI(LPCSTR val)
{
	return g_pSessionInstance->si_atoi(val);
}

//	sprintf() ����o�[�W����
DENGAKUDLL_API LPCSTR
ITOA(HIDEDLL_NUMTYPE val, HIDEDLL_NUMTYPE radix)
{
	g_strBuffer = g_pSessionInstance->si_itoa(val,radix);
	return g_strBuffer;
}

//	stricmp()
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRICMP(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_stricmp(str1,str2);
}

//	str1 ���� str2 �̏o������Ԃ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRCOUNT(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_strcount(str1,str2);
}

//	gsub() in awk
DENGAKUDLL_API LPCSTR
GSUB(LPCSTR str1, LPCSTR str2, LPCSTR str3, HIDEDLL_NUMTYPE num)
{
	g_strBuffer = g_pSessionInstance->si_gsub(str1,str2,str3,num);
	return g_strBuffer;
}

//	tolower
DENGAKUDLL_API LPCSTR
TOLOWER(LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_tolower(str);
	return g_strBuffer;
}

//	toupper
DENGAKUDLL_API LPCSTR
TOUPPER(LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_toupper(str);
	return g_strBuffer;
}

//	strstr
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRSTR(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_strstr(str1,str2);
}

//	strrstr
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRRSTR(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_strrstr(str1,str2);
}

//	strlen
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRLEN(LPCSTR str)
{
	return g_pSessionInstance->si_strlen(str);
}

//	leftstr
DENGAKUDLL_API LPCSTR
LEFTSTR(LPCSTR str, HIDEDLL_NUMTYPE num)
{
	g_strBuffer = g_pSessionInstance->si_leftstr(str,num);
	return g_strBuffer;
}

//	midstr
DENGAKUDLL_API LPCSTR
MIDSTR(LPCSTR str, HIDEDLL_NUMTYPE num1, HIDEDLL_NUMTYPE num2)
{
	g_strBuffer = g_pSessionInstance->si_midstr(str,num1,num2);
	return g_strBuffer;
}

//	rightstr
DENGAKUDLL_API LPCSTR
RIGHTSTR(LPCSTR str, HIDEDLL_NUMTYPE num)
{
	g_strBuffer = g_pSessionInstance->si_rightstr(str,num);
	return g_strBuffer;
}

//	�S�p�������܂߂� tolower
DENGAKUDLL_API LPCSTR
TOLOWER2(LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_tolower2(str);
	return g_strBuffer;
}

//	�S�p�������܂߂� toupper
DENGAKUDLL_API LPCSTR
TOUPPER2(LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_toupper2(str);
	return g_strBuffer;
}

//	�o���ʒu�� strlen2 ���ɐ����� strstr
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRSTR2(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_strstr2(str1,str2);
}

//	�o���ʒu�� strlen2 ���ɐ����� strrstr
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRRSTR2(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_strrstr2(str1,str2);
}

//	�S�p�������P�����Ƃ��Đ����� strlen
DENGAKUDLL_API HIDEDLL_NUMTYPE
STRLEN2(LPCSTR str)
{
	return g_pSessionInstance->si_strlen2(str);
}

//	�������� strlen2 ���ɐ����� leftstr
DENGAKUDLL_API LPCSTR
LEFTSTR2(LPCSTR str, HIDEDLL_NUMTYPE num)
{
	g_strBuffer = g_pSessionInstance->si_leftstr2(str,num);
	return g_strBuffer;
}

//	�������� strlen2 ���ɐ����� midstr
DENGAKUDLL_API LPCSTR
MIDSTR2(LPCSTR str, HIDEDLL_NUMTYPE num1, HIDEDLL_NUMTYPE num2)
{
	g_strBuffer = g_pSessionInstance->si_midstr2(str,num1,num2);
	return g_strBuffer;
}

//	�������� strlen2 ���ɐ����� rightstr
DENGAKUDLL_API LPCSTR
RIGHTSTR2(LPCSTR str, HIDEDLL_NUMTYPE num)
{
	g_strBuffer = g_pSessionInstance->si_rightstr2(str,num);
	return g_strBuffer;
}

//	������̍����̋󔒂���������
DENGAKUDLL_API LPCSTR
LTRIM(LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_ltrim(str);
	return g_strBuffer;
}

//	������̉E���̋󔒂���������
DENGAKUDLL_API LPCSTR
RTRIM(LPCSTR str)
{
	g_strBuffer = g_pSessionInstance->si_rtrim(str);
	return g_strBuffer;
}

//	str1 �� str2 �Ɋ܂܂�镶���ŕ������� strtok
DENGAKUDLL_API LPCSTR
GETTOKEN(LPCSTR str1, LPCSTR str2)
{
	g_strBuffer = g_pSessionInstance->si_gettoken(str1,str2);
	return g_strBuffer;
}

//	GETTOKEN() �ł܂��g�[�N�������邩�ǂ�����Ԃ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
HASMORETOKENS()
{
	return g_pSessionInstance->si_hasmoretokens();
}

