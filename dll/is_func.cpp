//	$Id: is_func.cpp,v 1.3 2002-02-19 15:34:22 sugiura Exp $
/*
 *	is_func.cpp
 *	�摜�̊Ǘ��Ɋւ���N���X
 */

#include "DengakuDLL.h"

//	Susie Plug-in �����[�h����
DENGAKUDLL_API HIDEDLL_NUMTYPE
LOADSPI(LPCSTR spi_file)
{
	try {
		return g_pSessionInstance->si_loadspi(spi_file);
	} catch (...) {
		return 0;
	}
}

//	Susie Plug-in ���A�����[�h����
DENGAKUDLL_API HIDEDLL_NUMTYPE
FREESPI()
{
	try {
		return g_pSessionInstance->si_freespi();
	} catch (...) {
		return 0;
	}
}

//	���[�h���� Plug-in �̏���Ԃ�
DENGAKUDLL_API LPCSTR
GETSPIINFO(HIDEDLL_NUMTYPE type)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getspiinfo(type);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�w�肳�ꂽ�摜�t�@�C���� Plug-in �ŃT�|�[�g����Ă��邩�ǂ�����Ԃ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
ISSUPPORTEDPIC(LPCSTR filename)
{
	try {
		return g_pSessionInstance->si_issupportedpic(filename);
	} catch (...) {
		return 0;
	}
}

//	�w�肳�ꂽ�摜�t�@�C���̏���Ԃ�
DENGAKUDLL_API LPCSTR
GETPICINFO(LPCSTR filename)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getpicinfo(filename);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

