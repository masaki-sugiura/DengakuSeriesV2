//	$Id: DengakuDLL.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	DengakuDLL.h
 *	���C���w�b�_�[�t�@�C��
 */

#ifndef DENGAKUDLL_DLLMAIN
#define DENGAKUDLL_DLLMAIN

#include "session.h"

// �ȉ��� ifdef �u���b�N�� DLL ����ȒP�ɃG�N�X�|�[�g������}�N�����쐬����W���I�ȕ��@�ł��B 
// ���� DLL ���̂��ׂẴt�@�C���̓R�}���h���C���Œ�`���ꂽ DENGAKUDLL_EXPORTS �V���{��
// �ŃR���p�C������܂��B���̃V���{���͂��� DLL ���g�p����ǂ̃v���W�F�N�g��ł�����`�łȂ�
// ��΂Ȃ�܂���B���̕��@�ł̓\�[�X�t�@�C���ɂ��̃t�@�C�����܂ނ��ׂẴv���W�F�N�g�� DLL 
// ����C���|�[�g���ꂽ���̂Ƃ��� DENGAKUDLL_API �֐����Q�Ƃ��A���̂��߂��� DLL �͂��̃}�N 
// ���Œ�`���ꂽ�V���{�����G�N�X�|�[�g���ꂽ���̂Ƃ��ĎQ�Ƃ��܂��B
#ifdef DENGAKUDLL_EXPORTS

#ifdef __cplusplus
#define DENGAKUDLL_API extern "C" __declspec(dllexport)
#else
#define DENGAKUDLL_API __declspec(dllexport)
#endif /* __cplusplus */

#else

#ifdef __cplusplus
#define DENGAKUDLL_API extern "C" __declspec(dllimport)
#else
#define DENGAKUDLL_API __declspec(dllimport)
#endif /* __cplusplus */

#endif /* DENGAKUDLL_EXPORTS */

//	int �ł� double �ł̗����ŃR�[�h���ė��p���邽�߂̃g���b�N
#ifndef HIDEDLL_DOUBLE
typedef int HIDEDLL_NUMTYPE;
#else
typedef double HIDEDLL_NUMTYPE;
#endif

#define HIDEDLL_TRUE  static_cast<HIDEDLL_NUMTYPE>(TRUE)
#define HIDEDLL_FALSE static_cast<HIDEDLL_NUMTYPE>(FALSE)

//	DLL �ł͗B��̃Z�b�V�����C���X�^���X�ւ̃|�C���^
extern SessionInstance*	g_pSessionInstance;
//	LPSTR ��Ԃ��֐����g�p���镶����o�b�t�@�ւ̃|�C���^
extern class StringBuffer *g_pStrBuffer;
//	DLL �̃o�[�W����������
extern const StringBuffer g_versionStr;

#endif
