//	$Id: fs_func.cpp,v 1.5 2004-11-16 17:03:51 sugiura Exp $
/*
 *	fs_func.cpp
 *	�t�@�C���T�[�r�X�̊֐�
 */

#include "DengakuDLL.h"
#include "cmdline.h"

//	�J�����g�f�B���N�g���̕ύX
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETCURDIR(LPCSTR str)
{
	try {
		return g_pSessionInstance->si_setcurdir(RemoveQuote(str));
	} catch (...) {
		return 0;
	}
}

//	UNIX like cp
DENGAKUDLL_API HIDEDLL_NUMTYPE
COPY(LPCSTR strOrg, LPCSTR strDest)
{
	try {
		RealCmdLineParser argv(strOrg);
		argv.addArgv(RemoveQuote(strDest));
		return g_pSessionInstance->si_copy(argv);
	} catch (...) {
		return 0;
	}
}

//	str1 �̖����� str2 ��ǉ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
APPENDCOPY(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_appendcopy(RemoveQuote(str1),
												RemoveQuote(str2),FALSE);
	} catch (...) {
		return 0;
	}
}

//	str1 �̖����� str2 ��ǉ�(EOF �͏���)
DENGAKUDLL_API HIDEDLL_NUMTYPE
APPENDCOPYTEXT(LPCSTR str1, LPCSTR str2)
{
	try {
		return g_pSessionInstance->si_appendcopy(RemoveQuote(str1),
												RemoveQuote(str2),TRUE);
	} catch (...) {
		return 0;
	}
}

//	�t�@�C���̍폜
DENGAKUDLL_API HIDEDLL_NUMTYPE
REMOVE(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);	//	const CmdLineParser& �ł͂Ȃ��̂�
		return g_pSessionInstance->si_remove(argv);
	} catch (...) {
		return 0;
	}
}

//	�t�@�C���̈ړ�(���O�ύX)
DENGAKUDLL_API HIDEDLL_NUMTYPE
MOVE(LPCSTR strOrg, LPCSTR strDest)
{
	try {
		RealCmdLineParser argv(strOrg);
		argv.addArgv(RemoveQuote(strDest));
		return g_pSessionInstance->si_move(argv);
	} catch (...) {
		return 0;
	}
}

//	�f�B���N�g���̍쐬
DENGAKUDLL_API HIDEDLL_NUMTYPE
MKDIR(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_mkdir(argv);
	} catch (...) {
		return 0;
	}
}

//	�f�B���N�g���̍폜
DENGAKUDLL_API HIDEDLL_NUMTYPE
RMDIR(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_rmdir(argv);
	} catch (...) {
		return 0;
	}
}

//	�����l�̕ύX
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETATTRIBUTE(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_setattribute(argv);
	} catch (...) {
		return 0;
	}
}

//	�^�C���X�^���v�̕ύX
DENGAKUDLL_API HIDEDLL_NUMTYPE
TOUCH(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_touch(argv);
	} catch (...) {
		return 0;
	}
}

//	�t�@�C������n�֐��̌���(-n �I�v�V�����w�莞�̂�)��Ԃ�
DENGAKUDLL_API LPCSTR
FILEOPRESULT()
{
	try {
		g_strBuffer = g_pSessionInstance->si_fileopresult();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	���ʂ� run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
RUN(LPSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_run(argv,SW_SHOW);
	} catch (...) {
		return 0;
	}
}

//	�A�C�R����Ԃ� run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
ICONRUN(LPSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_run(argv,SW_SHOWMINNOACTIVE);
	} catch (...) {
		return 0;
	}
}

//	�ő剻��Ԃ� run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
MAXRUN(LPSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_run(argv,SW_SHOWMAXIMIZED);
	} catch (...) {
		return 0;
	}
}

//	�s����Ԃ� run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
HIDERUN(LPSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_run(argv,SW_HIDE);
	} catch (...) {
		return 0;
	}
}

//	�t�@�C���E�f�B���N�g���̍ċA�I��
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMPATH(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_enumpath(argv);
	} catch (...) {
		return 0;
	}
}

//	�t�@�C���̗�
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMFILE(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_enumfile(argv);
	} catch (...) {
		return 0;
	}
}

//	�f�B���N�g���̗�
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMDIR(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		return g_pSessionInstance->si_enumdir(argv);
	} catch (...) {
		return 0;
	}
}

//	�J�����g�f�B���N�g����Ԃ�
DENGAKUDLL_API LPCSTR
GETCURDIR()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getcurdir();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�t�@�C�����ǎ��p���ǂ�����Ԃ�
DENGAKUDLL_API LPCSTR
ISREADONLY(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_isreadonly(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�t�@�C���̃T�C�Y��Ԃ�
DENGAKUDLL_API LPCSTR
SIZEOF(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_sizeof(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�^�C���X�^���v��Ԃ�
DENGAKUDLL_API LPCSTR
TIMESTAMPOF(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_timestampof(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�^�C���X�^���v�����郋�[���Ő��l�ɂ������̂�Ԃ�
DENGAKUDLL_API LPCSTR
TIMECOUNTOF(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_timecountof(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�t�@�C���̑����l��Ԃ�
DENGAKUDLL_API LPCSTR
ATTRIBUTEOF(LPCSTR str)
{
	try {
		RealCmdLineParser argv(str);
		g_strBuffer = g_pSessionInstance->si_attributeof(argv);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

DENGAKUDLL_API LPCSTR
EXISTFILE(LPSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getlongname(RemoveQuote(str),TRUE);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ENUM***() �Ŏ擾�����t�@�C���E�f�B���N�g������Ԃ�
DENGAKUDLL_API LPCSTR
FINDNEXT()
{
	try {
		g_strBuffer = g_pSessionInstance->si_findnext();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	���ݎg�p�\�ȃh���C�u��Ԃ�
DENGAKUDLL_API LPCSTR
GETDRIVES()
{
	try {
		g_strBuffer = g_pSessionInstance->si_getdrives();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�t�@�C�����������O�t�@�C�����ɕϊ�����
DENGAKUDLL_API LPCSTR
GETLONGNAME(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getlongname(RemoveQuote(str),FALSE);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	�t�@�C������DOS�t�@�C�����ɕϊ�����
DENGAKUDLL_API LPCSTR
GETSHORTNAME(LPCSTR str)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getshortname(RemoveQuote(str),FALSE);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

