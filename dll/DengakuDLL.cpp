//	$Id: DengakuDLL.cpp,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	DengakuDLL.cpp
 *	DLL �G���g���|�C���g�̒�`
 */

#include "DengakuDLL.h"
#include <commctrl.h>

//	DLL �̃C���X�^���X�n���h��
SessionInstance* g_pSessionInstance;

//	LPSTR ��Ԃ��֐����g�p���镶����o�b�t�@�̃|�C���^
StringBuffer* g_pStrBuffer;

//	DLL �̃o�[�W����������
const StringBuffer g_versionStr = "2.00��";

//	DLL �G���g���|�C���g
extern "C" BOOL APIENTRY
DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (g_pSessionInstance != NULL || g_pStrBuffer != NULL) {
			//	�L�蓾�Ȃ��Ǝv�����ǈꉞ
			return FALSE;
		}
		try {
			//	DLL �ł͗B��̃Z�b�V�����C���X�^���X�̍\�z
			g_pSessionInstance = new SessionInstance(hInstance);

			//	LPSTR ��Ԃ��֐��̂��߂ɕ�����o�b�t�@���m��
			g_pStrBuffer = new StringBuffer(nullStr);
		} catch (exception&) {
			return FALSE;
		}
		//	�悭�Y���̂�(^^;�ŏ��ɌĂяo���Ă���
		::InitCommonControls();
		break;

	case DLL_PROCESS_DETACH:
		if (g_pSessionInstance == NULL || g_pStrBuffer == NULL) {
			//	�L�蓾�Ȃ��Ǝv�����ǈꉞ
			return FALSE;
		}
		//	������o�b�t�@�̍폜
		delete g_pStrBuffer;
		g_pStrBuffer = NULL;
		//	�Z�b�V�����C���X�^���X�̍폜
		delete g_pSessionInstance;
		g_pSessionInstance = NULL;
		break;

/*
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
*/
    }
    return TRUE;
}

