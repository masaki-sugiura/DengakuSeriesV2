//	$Id: cs_func.cpp,v 1.2 2002-01-16 16:31:04 sugiura Exp $
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
	return g_versionStr;
}

//	���C�u�����̃o�[�W�����������Ԃ�
DENGAKUDLL_API LPCSTR
GETLIBVERSION()
{
	g_strBuffer = g_pSessionInstance->si_getlibversion();
	return g_strBuffer;
}

//	OS �̏���Ԃ�
DENGAKUDLL_API LPCSTR
GETOSINFO(LPCSTR str)
{
	RealCmdLineParser argv(str);
	g_strBuffer = g_pSessionInstance->si_getosinfo(argv);
	return g_strBuffer;
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

