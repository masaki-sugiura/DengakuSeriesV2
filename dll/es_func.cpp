//	$Id: es_func.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	es_func.cpp
 *	���ϐ��T�[�r�X�̊֐�
 */

#include "DengakuDLL.h"

//	�ϐ����Z�b�g����
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETVAR(LPCSTR env_name, LPCSTR env_var)
{
	return g_pSessionInstance->si_setvar(env_name,env_var);
}

//	�ϐ��̒l��Ԃ�
DENGAKUDLL_API LPCSTR
GETVAR(LPCSTR env_name)
{
	*g_pStrBuffer = g_pSessionInstance->si_getvar(env_name);
	return *g_pStrBuffer;
}

//	�ϐ����폜����
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELVAR(LPCSTR env_name)
{
	return g_pSessionInstance->si_delvar(env_name);
}

//	�ϐ������݂��邩�ǂ�����Ԃ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
EXISTVAR(LPCSTR env_name)
{
	return g_pSessionInstance->si_existvar(env_name);
}

//	�ϐ��̗񋓂��J�n����
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMVAR()
{
	return g_pSessionInstance->si_enumvar();
}

//	ENUMVAR() �ŗ񋓂����ϐ��̖��O��Ԃ�
DENGAKUDLL_API LPCSTR
NEXTVAR()
{
	*g_pStrBuffer = g_pSessionInstance->si_nextvar();
	return *g_pStrBuffer;
}

//	ENUMVAR(), NEXTVAR() �ŗ񋓂����ϐ����܂����邩�ǂ�����Ԃ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
HASMOREVARS()
{
	return g_pSessionInstance->si_hasmorevars();
}

