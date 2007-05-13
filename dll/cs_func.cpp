//	$Id: cs_func.cpp,v 1.7 2007-05-13 17:02:41 sugiura Exp $
/*
 *	cs_func.cpp
 *	���ʃT�[�r�X�̊֐�
 */

#include "DengakuDLL.h"
#include "cmdline.h"
// #include "evalexpr.h"

//	DLL �̃o�[�W�����������Ԃ�
DENGAKUDLL_API LPCSTR
GETVERSION()
{
//	return g_versionStr;
	try {
		g_strBuffer = g_pSessionInstance->getVersion();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	���C�u�����̃o�[�W�����������Ԃ�
DENGAKUDLL_API LPCSTR
GETLIBVERSION()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getlibversion();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	OS �̏���Ԃ�
DENGAKUDLL_API LPCSTR
GETOSINFO(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_getosinfo(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

#if 0
//	�v�Z����]�����āA�����w��ɏ]���Đ��`�������̂�Ԃ�
DENGAKUDLL_API LPCSTR
CALC(LPCSTR str1, LPCSTR str2)
{
	g_strBuffer = g_pSessionInstance->si_calc(str1,str2);
	return g_strBuffer;
}
#endif

DENGAKUDLL_API LPCSTR
GETCARETPOS()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getcaretpos();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
GETCURSORPOS()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getcursorpos();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API HIDEDLL_NUMTYPE
SLEEP(HIDEDLL_NUMTYPE nTime)
{
	try {
		return g_pSessionInstance->si_sleep(nTime);
	} catch (...) {
		return 0;
	}
}

//	���b�Z�[�W�{�b�N�X��\������
DENGAKUDLL_API LPCSTR
MSGBOX(LPCSTR msg, LPCSTR opt, LPCSTR caption)
{
	try {
		RealCmdLineParser argv(opt);
		g_strBuffer = g_pSessionInstance->si_msgbox(msg, argv, caption);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

