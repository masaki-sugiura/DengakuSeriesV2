//	$Id: fs_func.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
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
	return g_pSessionInstance->si_setcurdir(RemoveQuote(str));
}

//	UNIX like cp
DENGAKUDLL_API HIDEDLL_NUMTYPE
COPY(LPCSTR strOrg, LPCSTR strDest)
{
	RealCmdLineParser argv(strOrg);
	argv.addArgv(RemoveQuote(strDest));
	return g_pSessionInstance->si_copy(argv);
}

//	str1 �̖����� str2 ��ǉ�
DENGAKUDLL_API HIDEDLL_NUMTYPE
APPENDCOPY(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_appendcopy(RemoveQuote(str1),
											RemoveQuote(str2),FALSE);
}

//	str1 �̖����� str2 ��ǉ�(EOF �͏���)
DENGAKUDLL_API HIDEDLL_NUMTYPE
APPENDCOPYTEXT(LPCSTR str1, LPCSTR str2)
{
	return g_pSessionInstance->si_appendcopy(RemoveQuote(str1),
											RemoveQuote(str2),TRUE);
}

//	�t�@�C���̍폜
DENGAKUDLL_API HIDEDLL_NUMTYPE
REMOVE(LPCSTR str)
{
	RealCmdLineParser argv(str);	//	const CmdLineParser& �ł͂Ȃ��̂�
	return g_pSessionInstance->si_remove(argv);
}

//	�t�@�C���̈ړ�(���O�ύX)
DENGAKUDLL_API HIDEDLL_NUMTYPE
MOVE(LPCSTR strOrg, LPCSTR strDest)
{
	RealCmdLineParser argv(strOrg);
	argv.addArgv(RemoveQuote(strDest));
	return g_pSessionInstance->si_move(argv);
}

//	�f�B���N�g���̍쐬
DENGAKUDLL_API HIDEDLL_NUMTYPE
MKDIR(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_mkdir(argv);
}

//	�f�B���N�g���̍폜
DENGAKUDLL_API HIDEDLL_NUMTYPE
RMDIR(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_rmdir(argv);
}

//	�����l�̕ύX
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETATTRIBUTE(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_setattribute(argv);
}

//	�^�C���X�^���v�̕ύX
DENGAKUDLL_API HIDEDLL_NUMTYPE
TOUCH(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_touch(argv);
}

//	���ʂ� run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
RUN(LPSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_run(argv,SW_SHOW);
}

//	�A�C�R����Ԃ� run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
ICONRUN(LPSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_run(argv,SW_SHOWMINNOACTIVE);
}

//	�ő剻��Ԃ� run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
MAXRUN(LPSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_run(argv,SW_SHOWMAXIMIZED);
}

//	�s����Ԃ� run app
DENGAKUDLL_API HIDEDLL_NUMTYPE
HIDERUN(LPSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_run(argv,SW_HIDE);
}

//	�t�@�C���E�f�B���N�g���̍ċA�I��
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMPATH(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_enumpath(argv);
}

//	�t�@�C���̗�
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMFILE(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_enumfile(argv);
}

//	�f�B���N�g���̗�
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMDIR(LPCSTR str)
{
	RealCmdLineParser argv(str);
	return g_pSessionInstance->si_enumdir(argv);
}

//	�J�����g�f�B���N�g����Ԃ�
DENGAKUDLL_API LPCSTR
GETCURDIR()
{
	*g_pStrBuffer = g_pSessionInstance->si_getcurdir();
	return *g_pStrBuffer;
}

//	�t�@�C�����ǎ��p���ǂ�����Ԃ�
DENGAKUDLL_API LPCSTR
ISREADONLY(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_isreadonly(argv);
	return *g_pStrBuffer;
}

//	�t�@�C���̃T�C�Y��Ԃ�
DENGAKUDLL_API LPCSTR
SIZEOF(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_sizeof(argv);
	return *g_pStrBuffer;
}

//	�^�C���X�^���v��Ԃ�
DENGAKUDLL_API LPCSTR
TIMESTAMPOF(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_timestampof(argv);
	return *g_pStrBuffer;
}

//	�^�C���X�^���v�����郋�[���Ő��l�ɂ������̂�Ԃ�
DENGAKUDLL_API LPCSTR
TIMECOUNTOF(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_timecountof(argv);
	return *g_pStrBuffer;
}

//	�t�@�C���̑����l��Ԃ�
DENGAKUDLL_API LPCSTR
ATTRIBUTEOF(LPCSTR str)
{
	RealCmdLineParser argv(str);
	*g_pStrBuffer = g_pSessionInstance->si_attributeof(argv);
	return *g_pStrBuffer;
}

DENGAKUDLL_API LPCSTR
EXISTFILE(LPSTR str)
{
	*g_pStrBuffer = g_pSessionInstance->si_getlongname(RemoveQuote(str),TRUE);
	return *g_pStrBuffer;
}

//	ENUM***() �Ŏ擾�����t�@�C���E�f�B���N�g������Ԃ�
DENGAKUDLL_API LPCSTR
FINDNEXT()
{
	*g_pStrBuffer = g_pSessionInstance->si_findnext();
	return *g_pStrBuffer;
}

//	���ݎg�p�\�ȃh���C�u��Ԃ�
DENGAKUDLL_API LPCSTR
GETDRIVES()
{
	*g_pStrBuffer = g_pSessionInstance->si_getdrives();
	return *g_pStrBuffer;
}

//	�t�@�C�����������O�t�@�C�����ɕϊ�����
DENGAKUDLL_API LPCSTR
GETLONGNAME(LPCSTR str)
{
	*g_pStrBuffer = g_pSessionInstance->si_getlongname(RemoveQuote(str),FALSE);
	return *g_pStrBuffer;
}

