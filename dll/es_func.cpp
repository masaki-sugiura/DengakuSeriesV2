//	$Id: es_func.cpp,v 1.3 2002-02-19 15:34:22 sugiura Exp $
/*
 *	es_func.cpp
 *	���ϐ��T�[�r�X�̊֐�
 */

#include "DengakuDLL.h"

//	�ϐ����Z�b�g����
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETVAR(LPCSTR env_name, LPCSTR env_var)
{
	try {
		return g_pSessionInstance->si_setvar(env_name,env_var);
	} catch (...) {
		return 0;
	}
}

//	�ϐ��̒l��Ԃ�
DENGAKUDLL_API LPCSTR
GETVAR(LPCSTR env_name)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getvar(env_name);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�ϐ����폜����
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELVAR(LPCSTR env_name)
{
	try {
		return g_pSessionInstance->si_delvar(env_name);
	} catch (...) {
		return 0;
	}
}

//	�ϐ������݂��邩�ǂ�����Ԃ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
EXISTVAR(LPCSTR env_name)
{
	try {
		return g_pSessionInstance->si_existvar(env_name);
	} catch (...) {
		return 0;
	}
}

//	�ϐ��̗񋓂��J�n����
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMVAR()
{
	try {
		return g_pSessionInstance->si_enumvar();
	} catch (...) {
		return 0;
	}
}

//	ENUMVAR() �ŗ񋓂����ϐ��̖��O��Ԃ�
DENGAKUDLL_API LPCSTR
NEXTVAR()
{
	try {
		g_strBuffer = g_pSessionInstance->si_nextvar();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ENUMVAR(), NEXTVAR() �ŗ񋓂����ϐ����܂����邩�ǂ�����Ԃ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
HASMOREVARS()
{
	try {
		return g_pSessionInstance->si_hasmorevars();
	} catch (...) {
		return 0;
	}
}

