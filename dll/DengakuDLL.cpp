//	$Id: DengakuDLL.cpp,v 1.25 2003-03-11 15:47:30 sugiura Exp $
/*
 *	DengakuDLL.cpp
 *	DLL �G���g���|�C���g�̒�`
 */

#include "DengakuDLL.h"
#include "auto_ptr.h"

#include <commctrl.h>

//	DLL �̃C���X�^���X�n���h��
Auto_Ptr<SessionInstance> g_pSessionInstance(NULL);

//	LPSTR ��Ԃ��֐����g�p���镶����o�b�t�@�̃|�C���^
StringBuffer g_strBuffer;

//	DLL �̃o�[�W����������
const StringBuffer g_versionStr = "2.51";

//	DLL �G���g���|�C���g
extern "C" BOOL APIENTRY
DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (g_pSessionInstance.ptr() != NULL) {
			//	�L�蓾�Ȃ��Ǝv�����ǈꉞ
			return FALSE;
		}
		try {
			//	DLL �ł͗B��̃Z�b�V�����C���X�^���X�̍\�z
			g_pSessionInstance = new SessionInstance(hInstance);
		} catch (...) {
			return FALSE;
		}
		//	�悭�Y���̂�(^^;�ŏ��ɌĂяo���Ă���
		::InitCommonControls();
		break;

	case DLL_PROCESS_DETACH:
		if (g_pSessionInstance.ptr() == NULL) {
			//	�L�蓾�Ȃ��Ǝv�����ǈꉞ
			return FALSE;
		}
		//	�Z�b�V�����C���X�^���X�̍폜
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

