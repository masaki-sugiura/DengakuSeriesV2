//	$Id: ms_func.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	ms_func.cpp
 *	���j���[�\���֐�
 */

#include "DengakuDLL.h"

//	�|�b�v�A�b�v���j���[��\�����I�����ʂ�Ԃ�
DENGAKUDLL_API LPCSTR
MENU(LPCSTR menuname, HIDEDLL_NUMTYPE hWndHavingCaret)
{
	POINT pt;
	POINT* pAt = NULL;
	if (hWndHavingCaret != 0) {
		pAt = &pt;
		::GetCaretPos(pAt);
		::ClientToScreen((HWND)hWndHavingCaret,pAt);
	}

	//	showMenuThread() ���ł������R�[�h�����邪�A
	//	�����ĂȂ��悤�Ȃ̂ł����ɂ������...���ŁH
	HWND hwndTop = ::GetForegroundWindow();

	//	���j���[��\������X���b�h�̊J�n
	//	���j���[�̕\����ʃX���b�h�̃E�B���h�E�ɔC���闝�R�F
	//	���j���̃X���b�h���Ń��j���[��\���������
	//		�G�ۂ̃E�B���h�E���N���b�N�������_��
	//		�u�}�N�����I�����܂����H�v�Ƃł�B
	//		����͓����E�B���h�E���i���̃X���b�h�Łj�쐬���Ă����l�B
	//		�}�N�����쒆�̏G�ۂ̓��b�Z�[�W���[�v��(DispatchMessage �ɓn���O)��
	//		WM_LBUTTONDOWN ���t�b�N���Ă�����̂Ǝv����B
	if (!g_pSessionInstance->showMenuThread(menuname,pAt)) return errorStr;

	//	�X���b�h����~����܂ő҂�
	g_pSessionInstance->waitMenuThread();

	//	�Ԃ�l�̎擾
	*g_pStrBuffer = nullStr;
	g_pSessionInstance->getNotify(*g_pStrBuffer,INFINITE);

	//	�X���b�h�̔j��
	g_pSessionInstance->stopMenuThread();

	::SetForegroundWindow(hwndTop);

	return *g_pStrBuffer;
}

//	�V�K���j���[�̍\�z
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWMENU(LPCSTR menuname)
{
	return g_pSessionInstance->si_newmenu(menuname,TRUE);
}

//	���j���[���ڂ̒ǉ�(�����l����)
DENGAKUDLL_API HIDEDLL_NUMTYPE
ADDMENUITEMEX(LPCSTR menuname, LPCSTR title, LPCSTR name, LPCSTR opt)
{
	return g_pSessionInstance->si_addmenuitem(menuname,title,name,opt);
}

//	���j���[���ڂ̒ǉ�(�����l�Ȃ�)
DENGAKUDLL_API HIDEDLL_NUMTYPE
ADDMENUITEM(LPCSTR menuname, LPCSTR title, LPCSTR name)
{
	return g_pSessionInstance->si_addmenuitem(menuname,title,name,nullStr);
}

//	���j���[���ڂ̕ύX(�����l����)
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGEMENUITEMEX(LPCSTR name, HIDEDLL_NUMTYPE index, LPCSTR str, LPCSTR opt)
{
	return g_pSessionInstance->si_changemenuitem(name,index,str,opt);
}

//	���j���[���ڂ̕ύX(�����l�Ȃ�)
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGEMENUITEM(LPCSTR name, HIDEDLL_NUMTYPE index, LPCSTR str)
{
	return g_pSessionInstance->si_changemenuitem(name,index,str,nullStr);
}

//	���j���[���ڂ��폜
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELETEMENUITEM(LPCSTR menuname, HIDEDLL_NUMTYPE index)
{
	return g_pSessionInstance->si_deletemenuitem(menuname,index);
}

