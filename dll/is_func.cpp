//	$Id: is_func.cpp,v 1.2 2002-01-16 16:31:04 sugiura Exp $
/*
 *	is_func.cpp
 *	�摜�̊Ǘ��Ɋւ���N���X
 */

#include "DengakuDLL.h"

//	Susie Plug-in �����[�h����
DENGAKUDLL_API HIDEDLL_NUMTYPE
LOADSPI(LPCSTR spi_file)
{
	return g_pSessionInstance->si_loadspi(spi_file);
}

//	Susie Plug-in ���A�����[�h����
DENGAKUDLL_API HIDEDLL_NUMTYPE
FREESPI()
{
	return g_pSessionInstance->si_freespi();
}

//	���[�h���� Plug-in �̏���Ԃ�
DENGAKUDLL_API LPCSTR
GETSPIINFO(HIDEDLL_NUMTYPE type)
{
	g_strBuffer = g_pSessionInstance->si_getspiinfo(type);
	return g_strBuffer;
}

//	�w�肳�ꂽ�摜�t�@�C���� Plug-in �ŃT�|�[�g����Ă��邩�ǂ�����Ԃ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
ISSUPPORTEDPIC(LPCSTR filename)
{
	return g_pSessionInstance->si_issupportedpic(filename);
}

//	�w�肳�ꂽ�摜�t�@�C���̏���Ԃ�
DENGAKUDLL_API LPCSTR
GETPICINFO(LPCSTR filename)
{
	g_strBuffer = g_pSessionInstance->si_getpicinfo(filename);
	return g_strBuffer;
}

