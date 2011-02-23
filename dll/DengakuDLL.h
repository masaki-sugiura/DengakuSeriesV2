//	$Id: DengakuDLL.h,v 1.2 2002/01/16 16:31:04 sugiura Exp $
/*
 *	DengakuDLL.h
 *	���C���w�b�_�[�t�@�C��
 */

#ifndef DENGAKUDLL_DLLMAIN
#define DENGAKUDLL_DLLMAIN

#include "session.h"

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
extern Auto_Ptr<SessionInstance> g_pSessionInstance;
//	LPSTR ��Ԃ��֐����g�p���镶����o�b�t�@�ւ̃|�C���^
extern StringBuffer g_strBuffer;
//	DLL �̃o�[�W����������
extern const StringBuffer g_versionStr;

#endif
