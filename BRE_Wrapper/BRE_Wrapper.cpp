//	$Id: BRE_Wrapper.cpp,v 1.5 2005-01-24 15:52:48 sugiura Exp $
/*
 *	BRE_Wrapper.cpp
 *	BRE_WRAP.DLL �{�́��c�k�k�֐�
 */

#include "BRE_Wrapper.h"
#include "BRegExp_Mngr.h"

//	BRE_Wrapper �̃o�[�W����������
LPCSTR lpszBRE_Wrapper_Version = "1.01";

//	DLL �̃C���X�^���X�n���h��
Auto_Ptr<BRegExp_Manager> g_pBRegExp_Manager = NULL;

//	LPSTR ��Ԃ��֐����g�p���镶����o�b�t�@
StringBuffer g_strBuffer;

//	DLL �G���g���|�C���g
extern "C" BOOL APIENTRY
DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (g_pBRegExp_Manager.ptr() != NULL) {
			//	�L�蓾�Ȃ��Ǝv�����ǈꉞ
			return FALSE;
		}
		try {
			//	�R���X�g���N�^���� BREGEXP DLL �� LoadLibrary() ����
			//	MSDN �ɂ́u�댯�Ȃ̂ł��ȁI�v�Ə����Ă��邪�c
			g_pBRegExp_Manager = new BRegExp_Manager(BREGEXP_FILENAME);
		} catch (...) {
			return FALSE;
		}
		break;

	case DLL_PROCESS_DETACH:
		if (g_pBRegExp_Manager.ptr() != NULL)
			g_pBRegExp_Manager = NULL; // ������ FreeLibrary() ���c(����)
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

BRE_WRAPPER_API LPCSTR
BRE_BREWVERSION()
{
	return lpszBRE_Wrapper_Version;
}

BRE_WRAPPER_API LPCSTR
BRE_ERRORMSG()
{
	g_strBuffer = g_pBRegExp_Manager->getErrorMessage();
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_VERSION()
{
	g_strBuffer = g_pBRegExp_Manager->bRegexpversion();
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_MATCH(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = make_result(g_pBRegExp_Manager->bMatch(pattern,str));
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_SUBST(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = g_pBRegExp_Manager->bSubst(pattern,str);
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_TRANS(LPCSTR pattern, LPCSTR str)
{
	g_strBuffer = g_pBRegExp_Manager->bTrans(pattern,str);
	return g_strBuffer;
}

BRE_WRAPPER_API int
BRE_SPLIT(LPCSTR pattern, LPCSTR str, int limit)
{
	return g_pBRegExp_Manager->bSplit(pattern, str, limit);
}

BRE_WRAPPER_API LPCSTR
BRE_GETNEXT()
{
	g_strBuffer = make_result(g_pBRegExp_Manager->getNextResult());
	return g_strBuffer;
}

BRE_WRAPPER_API LPCSTR
BRE_POSTOSTR(LPCSTR pstr)
{
	DWORD pos = make_dword_from_pos(pstr);
	if (pos == -1) return nullStr;
	g_strBuffer = g_pBRegExp_Manager->posToString(pos);
	return g_strBuffer;
}

BRE_WRAPPER_API int
BRE_HASMORERESULTS()
{
	return (int)g_pBRegExp_Manager->hasMoreResults();
}

