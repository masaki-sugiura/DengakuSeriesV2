//	$Id: ds_func.cpp,v 1.2 2002-01-16 16:31:04 sugiura Exp $
/*
 *	ds_func.cpp
 *	�_�C�A���O����֐�
 */

#include "DengakuDLL.h"
#include "cmdline.h"

//	�_�C�A���O����̒ʒm�R�[�h�̓�����҂�
DENGAKUDLL_API LPCSTR
WAITCTRLNOTIFY(HIDEDLL_NUMTYPE num)
{
	if (num < 0) num = 0;
	g_strBuffer = nullStr;
	if (g_pSessionInstance->getDlgFrame().getUserDlg() != NULL)
		g_pSessionInstance->getNotify(g_strBuffer,(DWORD)num);
	return g_strBuffer;
}

//	�_�C�A���O�̕\��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWDIALOG(HIDEDLL_NUMTYPE num1, HIDEDLL_NUMTYPE num2)
{
	return g_pSessionInstance->si_showdialog((HWND)num1,(BOOL)num2);
}

//	�_�C�A���O�̔j��
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENDDIALOG()
{
	return g_pSessionInstance->si_enddialog();
}

//	�R���g���[���̏�Ԃ�ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSTATE(LPCSTR str1, LPCSTR str2)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_setctrlstate(str1,argv);
}

//	�R���g���[���̃e�L�X�g��ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSTRING(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_setctrlstring(str1,str2);
}

//	�R���g���[���� IME �̏�Ԃ�ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLIMESTATE(LPCSTR str1, HIDEDLL_NUMTYPE num2)
{
	return g_pSessionInstance->si_setctrlimestate(str1,num2);
}

//	�R���g���[���̍��ڂ̓��e��ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGECTRLITEM(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_changectrlitem(str1,argv,str3);
}

//	�R���g���[���̍��ڂ�ǉ�����
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLITEM(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_setctrlitem(str1,argv,str3);
}

//	�R���g���[���̍��ڂ��폜����
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELETECTRLITEM(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_deletectrlitem(str1,str2);
}

//	�R���g���[���̍��ڂ�S�폜����
DENGAKUDLL_API HIDEDLL_NUMTYPE
RESETCTRLITEM(LPCSTR str1)
{
	return g_pSessionInstance->si_resetctrlitem(str1);
}

//	�R���g���[���̗L���E������؂肩����
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENABLECTRL(HIDEDLL_NUMTYPE num, LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_enablectrl(num,argv);
}

//	�R���g���[���̃t�H���g��ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLFONT(LPCSTR str1, LPCSTR str2)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_setctrlfont(str1,argv);
}

//	���b�Z�[�W�{�b�N�X��\������
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWMESSAGE(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_showmessage(str1,str2);
}

//	����ɓ����郁�b�Z�[�W�{�b�N�X��\������
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWQUESTION(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	return g_pSessionInstance->si_showquestion(str1,str2,str3);
}

//	�_�C�A���O�̌��݂̓��e���f�[�^�t�@�C���ɕۑ�����
DENGAKUDLL_API HIDEDLL_NUMTYPE
SAVEDIALOG(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_savedialog(str1,str2);
}

//	�ۑ������_�C�A���O�̃f�[�^�t�@�C������f�[�^��ǂݍ���
DENGAKUDLL_API HIDEDLL_NUMTYPE
LOADDIALOG(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_loaddialog(str1,str2);
}

//	�_�C�A���O�̃^�C�g����ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETDLGTITLE(LPCSTR str1)
{
	return g_pSessionInstance->si_setdlgtitle(str1);
}

//	�_�C�A���O�̃^�C�g����Ԃ�
DENGAKUDLL_API LPCSTR
GETDLGTITLE()
{
	g_strBuffer = g_pSessionInstance->si_getdlgtitle();
	return g_strBuffer;
}

//	�R���g���[���̏�Ԃ�Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLSTATE(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getctrlstate(str1);
	return g_strBuffer;
}

//	�R���g���[���̃e�L�X�g��Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLSTRING(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getctrlstring(str1);
	return g_strBuffer;
}

//	�R���g���[���� IME �̏�Ԃ�Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLIMESTATE(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getctrlimestate(str1);
	return g_strBuffer;
}

//	�R���g���[���̃t�H���g����Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLFONT(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getctrlfont(str1);
	return g_strBuffer;
}

//	�R���g���[���̍��ڃe�L�X�g��Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLITEM(LPCSTR str1, LPCSTR str2)
{
	g_strBuffer = g_pSessionInstance->si_getctrlitem(str1,str2);
	return g_strBuffer;
}

//	�_�C�A���O�f�[�^�t�@�C���̃V�O�l�`����Ԃ�
DENGAKUDLL_API LPCSTR
GETDLGSIGNATURE(LPCSTR str1)
{
	g_strBuffer = g_pSessionInstance->si_getdlgsignature(str1);
	return g_strBuffer;
}

//	�V�K�_�C�A���O�e���v���[�g�̍쐬
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWDIALOG(LPCSTR str1, HIDEDLL_NUMTYPE num1, LPCSTR str2)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_newdialog(str1,(WORD)num1,argv);
}

//	�V�K�q�_�C�A���O�e���v���[�g�̍쐬
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWDLGPAGE(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_newdlgpage(str1,(WORD)num1);
}

//	�ύX�ΏۂƂȂ�q�_�C�A���O�̑I��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCURDLGPAGE(LPCSTR str1)
{
	return g_pSessionInstance->si_setcurdlgpage(str1);
}

//	���ݕύX�ΏۂƂȂ��Ă���q�_�C�A���O�̖��O��Ԃ�
DENGAKUDLL_API LPCSTR
GETCURDLGPAGE()
{
	g_strBuffer = g_pSessionInstance->si_getcurdlgpage();
	return g_strBuffer;
}

//	�V�K�R���g���[���̍쐬
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWCONTROL(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	return g_pSessionInstance->si_newcontrol(str1,str2,str3);
}

//	�R���g���[���̕���ݒ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLWIDTH(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_setctrlwidth(str1,(WORD)num1);
}

//	�R���g���[���̍�����ݒ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLHEIGHT(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_setctrlheight(str1,(WORD)num1);
}

//	�R���g���[���̒ʒm�R�[�h��ݒ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLNOTIFY(LPCSTR str1, LPCSTR str2)
{
	RealCmdLineParser argv(str2);
	return g_pSessionInstance->si_setctrlnotify(str1,argv);
}

//	�V������̊J�n
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWCOLUMN(HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_newcolumn((WORD)num1);
}

//	�V�����y�[�W�̊J�n
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWPAGE(HIDEDLL_NUMTYPE num1)
{
	return g_pSessionInstance->si_newpage((WORD)num1);
}

