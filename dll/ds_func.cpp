//	$Id: ds_func.cpp,v 1.5 2002-11-03 15:36:50 sugiura Exp $
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
	try {
		if (num < 0) num = 0;
		g_strBuffer = nullStr;
		if (g_pSessionInstance->getDlgFrame().getUserDlg() != NULL)
			g_pSessionInstance->getNotify(g_strBuffer,(DWORD)num);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�_�C�A���O�̕\��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWDIALOG(HIDEDLL_NUMTYPE num1, HIDEDLL_NUMTYPE num2)
{
	try {
		return g_pSessionInstance->si_showdialog((HWND)num1,(BOOL)num2);
	} catch (...) {
		return 0;
	}
}

//	�_�C�A���O�̔j��
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENDDIALOG()
{
	try {
		return g_pSessionInstance->si_enddialog();
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̏�Ԃ�ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSTATE(LPCSTR str1, LPCSTR str2)
{
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlstate(str1,argv);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̃e�L�X�g��ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSTRING(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_setctrlstring(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���� IME �̏�Ԃ�ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLIMESTATE(LPCSTR str1, HIDEDLL_NUMTYPE num2)
{
	try {
		return g_pSessionInstance->si_setctrlimestate(str1,num2);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̃\�[�g�̏�Ԃ�ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLSORT(LPCSTR str1, LPCSTR str2)
{
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlsort(str1, argv);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̍��ڂ̓��e��ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGECTRLITEM(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_changectrlitem(str1,argv,str3);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̍��ڂ�ǉ�����
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLITEM(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlitem(str1,argv,str3);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̍��ڂ��폜����
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELETECTRLITEM(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_deletectrlitem(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̍��ڂ�S�폜����
DENGAKUDLL_API HIDEDLL_NUMTYPE
RESETCTRLITEM(LPCSTR str1)
{
	try {
		return g_pSessionInstance->si_resetctrlitem(str1);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̗L���E������؂肩����
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENABLECTRL(HIDEDLL_NUMTYPE num, LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_enablectrl(num,argv);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̃t�H���g��ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLFONT(LPCSTR str1, LPCSTR str2)
{
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlfont(str1,argv);
	} catch (...) {
		return 0;
	}
}

//	���b�Z�[�W�{�b�N�X��\������
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWMESSAGE(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_showmessage(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	����ɓ����郁�b�Z�[�W�{�b�N�X��\������
DENGAKUDLL_API HIDEDLL_NUMTYPE
SHOWQUESTION(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	try {
		return g_pSessionInstance->si_showquestion(str1,str2,str3);
	} catch (...) {
		return 0;
	}
}

//	�_�C�A���O�̌��݂̓��e���f�[�^�t�@�C���ɕۑ�����
DENGAKUDLL_API HIDEDLL_NUMTYPE
SAVEDIALOG(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_savedialog(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	�ۑ������_�C�A���O�̃f�[�^�t�@�C������f�[�^��ǂݍ���
DENGAKUDLL_API HIDEDLL_NUMTYPE
LOADDIALOG(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_loaddialog(str1,str2);
	} catch (...) {
		return 0;
	}
}

//	�_�C�A���O�̃^�C�g����ݒ肷��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETDLGTITLE(LPCSTR str1)
{
	try {
		return g_pSessionInstance->si_setdlgtitle(str1);
	} catch (...) {
		return 0;
	}
}

//	�_�C�A���O�̃^�C�g����Ԃ�
DENGAKUDLL_API LPCSTR
GETDLGTITLE()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getdlgtitle();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�R���g���[���̏�Ԃ�Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLSTATE(LPCSTR str1)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlstate(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�R���g���[���̃e�L�X�g��Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLSTRING(LPCSTR str1)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlstring(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�R���g���[���� IME �̏�Ԃ�Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLIMESTATE(LPCSTR str1)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlimestate(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�R���g���[���̃\�[�g�̏�Ԃ�Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLSORT(LPCSTR str1)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlsort(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�R���g���[���̃t�H���g����Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLFONT(LPCSTR str1)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlfont(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�R���g���[���̍��ڃe�L�X�g��Ԃ�
DENGAKUDLL_API LPCSTR
GETCTRLITEM(LPCSTR str1, LPCSTR str2)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getctrlitem(str1,str2);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�_�C�A���O�f�[�^�t�@�C���̃V�O�l�`����Ԃ�
DENGAKUDLL_API LPCSTR
GETDLGSIGNATURE(LPCSTR str1)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getdlgsignature(str1);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�V�K�_�C�A���O�e���v���[�g�̍쐬
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWDIALOG(LPCSTR str1, HIDEDLL_NUMTYPE num1, LPCSTR str2)
{
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_newdialog(str1,(WORD)num1,argv);
	} catch (...) {
		return 0;
	}
}

//	�V�K�q�_�C�A���O�e���v���[�g�̍쐬
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWDLGPAGE(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	try {
		return g_pSessionInstance->si_newdlgpage(str1,(WORD)num1);
	} catch (...) {
		return 0;
	}
}

//	�ύX�ΏۂƂȂ�q�_�C�A���O�̑I��
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCURDLGPAGE(LPCSTR str1)
{
	try {
		return g_pSessionInstance->si_setcurdlgpage(str1);
	} catch (...) {
		return 0;
	}
}

//	�t�H�[�J�X�𓾂�R���g���[���̕ύX
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETFOCUSEDCTRL(LPCSTR str1)
{
	try {
		return g_pSessionInstance->si_setfocusedctrl(str1);
	} catch (...) {
		return 0;
	}
}

//	���ݕύX�ΏۂƂȂ��Ă���q�_�C�A���O�̖��O��Ԃ�
DENGAKUDLL_API LPCSTR
GETCURDLGPAGE()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getcurdlgpage();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	���݃t�H�[�J�X�𓾂Ă���R���g���[���̖��O��Ԃ�
DENGAKUDLL_API LPCSTR
GETFOCUSEDCTRL()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getfocusedctrl();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�V�K�R���g���[���̍쐬
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWCONTROL(LPCSTR str1, LPCSTR str2, LPCSTR str3)
{
	try {
		return g_pSessionInstance->si_newcontrol(str1,str2,str3);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̕���ݒ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLWIDTH(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	try {
		return g_pSessionInstance->si_setctrlwidth(str1,(WORD)num1);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̍�����ݒ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLHEIGHT(LPCSTR str1, HIDEDLL_NUMTYPE num1)
{
	try {
		return g_pSessionInstance->si_setctrlheight(str1,(WORD)num1);
	} catch (...) {
		return 0;
	}
}

//	�R���g���[���̒ʒm�R�[�h��ݒ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCTRLNOTIFY(LPCSTR str1, LPCSTR str2)
{
	try {
		RealCmdLineParser argv(str2);
		return g_pSessionInstance->si_setctrlnotify(str1,argv);
	} catch (...) {
		return 0;
	}
}

//	�V������̊J�n
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWCOLUMN(HIDEDLL_NUMTYPE num1)
{
	try {
		return g_pSessionInstance->si_newcolumn((WORD)num1);
	} catch (...) {
		return 0;
	}
}

//	�V�����y�[�W�̊J�n
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWPAGE(HIDEDLL_NUMTYPE num1)
{
	try {
		return g_pSessionInstance->si_newpage((WORD)num1);
	} catch (...) {
		return 0;
	}
}

