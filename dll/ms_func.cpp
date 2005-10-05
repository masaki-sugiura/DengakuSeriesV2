//	$Id: ms_func.cpp,v 1.6.2.1 2005-10-05 17:03:21 sugiura Exp $
/*
 *	ms_func.cpp
 *	���j���[�\���֐�
 */

#include "DengakuDLL.h"

static HWND
GetActualHavingCaretWindow(HWND hWndHavingCaretCandidate)
{
	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!::GetVersionEx(&osi)) {
		return hWndHavingCaretCandidate;
	}

	switch (osi.dwPlatformId) {
	case VER_PLATFORM_WIN32_NT:
		// �{���� WinNT4.0SP3�ȍ~�����A�߂�ǂ��̂�Win2K�ȍ~�Ƃ���
		if (osi.dwMajorVersion < 5) {
			return hWndHavingCaretCandidate;
		}
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		// Win98�ȍ~
		if (osi.dwMinorVersion == 0) {
			return hWndHavingCaretCandidate;
		}
		break;
	default:
		return hWndHavingCaretCandidate;
	}

	DWORD dwThreadID = ::GetWindowThreadProcessId(hWndHavingCaretCandidate, NULL);

	GUITHREADINFO guiInfo;
	guiInfo.cbSize = sizeof(guiInfo);
	if (::GetGUIThreadInfo(dwThreadID, &guiInfo)) {
		return guiInfo.hwndCaret;
	}

	return hWndHavingCaretCandidate;
}

static LPCSTR
show_menu(LPCSTR menuname, HIDEDLL_NUMTYPE hWndHavingCaret,
		  HIDEDLL_NUMTYPE x_offset, HIDEDLL_NUMTYPE y_offset)
{
	try {
		POINT pt;
		if (hWndHavingCaret > 0) {
			// �L�����b�g�ʒu�
			::GetCaretPos(&pt);
			HWND hWndActualHavingCaret = GetActualHavingCaretWindow((HWND)hWndHavingCaret);
			::ClientToScreen(hWndActualHavingCaret, &pt);
			pt.x += x_offset;
			pt.y += y_offset;
		} else {
			// ��Βl�w��(hWndHavingCaret==0�̏ꍇ�̓}�E�X�J�[�\���̈ʒu�ɂȂ�)
			pt.x = x_offset;
			pt.y = y_offset;
		}

		//	showMenuThread() ���ł������R�[�h�����邪�A
		//	�����ĂȂ��悤�Ȃ̂ł����ɂ������...���ŁH
		HWND hwndTop = ::GetForegroundWindow();
		if (hwndTop != NULL) ::UpdateWindow(hwndTop);

		//	���j���[��\������X���b�h�̊J�n
		//	���j���[�̕\����ʃX���b�h�̃E�B���h�E�ɔC���闝�R�F
		//	���j���̃X���b�h���Ń��j���[��\���������
		//		�G�ۂ̃E�B���h�E���N���b�N�������_��
		//		�u�}�N�����I�����܂����H�v�Ƃł�B
		//		����͓����E�B���h�E���i���̃X���b�h�Łj�쐬���Ă����l�B
		//		�}�N�����쒆�̏G�ۂ̓��b�Z�[�W���[�v��(DispatchMessage �ɓn���O)��
		//		WM_LBUTTONDOWN ���t�b�N���Ă�����̂Ǝv����B
		if (!g_pSessionInstance->showMenuThread(menuname,
												hWndHavingCaret == 0,
												pt)) {
			return errorStr;
		}

		//	�X���b�h����~����܂ő҂�
		g_pSessionInstance->waitMenuThread();

		//	�Ԃ�l�̎擾
		g_strBuffer = nullStr;
		g_pSessionInstance->getNotify(g_strBuffer,INFINITE);

		//	�X���b�h�̔j��
		g_pSessionInstance->stopMenuThread();

		if (hwndTop != NULL) ::SetForegroundWindow(hwndTop);

		return g_strBuffer;

	} catch (...) {
		return "";
	}
}

//	�|�b�v�A�b�v���j���[��\�����I�����ʂ�Ԃ�
DENGAKUDLL_API LPCSTR
MENU(LPCSTR menuname, HIDEDLL_NUMTYPE hWndHavingCaret)
{
	return show_menu(menuname, hWndHavingCaret, 0, 0);
}

//	�|�b�v�A�b�v���j���[��\�����I�����ʂ�Ԃ�
DENGAKUDLL_API LPCSTR
MENUEX(LPCSTR menuname, HIDEDLL_NUMTYPE hWndHavingCaret,
	   HIDEDLL_NUMTYPE x_offset, HIDEDLL_NUMTYPE y_offset)
{
	return show_menu(menuname, hWndHavingCaret, x_offset, y_offset);
}

//	�V�K���j���[�̍\�z
DENGAKUDLL_API HIDEDLL_NUMTYPE
NEWMENU(LPCSTR menuname)
{
	try {
		return g_pSessionInstance->si_newmenu(menuname,TRUE);
	} catch (...) {
//		::MessageBox(NULL, "some exception occured.", NULL, MB_OK);
		return 0;
	}
}

//	���j���[���ڂ̒ǉ�(�����l����)
DENGAKUDLL_API HIDEDLL_NUMTYPE
ADDMENUITEMEX(LPCSTR menuname, LPCSTR title, LPCSTR name, LPCSTR opt)
{
	try {
		return g_pSessionInstance->si_addmenuitem(menuname,title,name,opt);
	} catch (...) {
		return 0;
	}
}

//	���j���[���ڂ̒ǉ�(�����l�Ȃ�)
DENGAKUDLL_API HIDEDLL_NUMTYPE
ADDMENUITEM(LPCSTR menuname, LPCSTR title, LPCSTR name)
{
	try {
		return g_pSessionInstance->si_addmenuitem(menuname,title,name,nullStr);
	} catch (...) {
		return 0;
	}
}

//	���j���[���ڂ̕ύX(�����l����)
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGEMENUITEMEX(LPCSTR name, HIDEDLL_NUMTYPE index, LPCSTR str, LPCSTR opt)
{
	try {
		return g_pSessionInstance->si_changemenuitem(name,index,str,opt);
	} catch (...) {
		return 0;
	}
}

//	���j���[���ڂ̕ύX(�����l�Ȃ�)
DENGAKUDLL_API HIDEDLL_NUMTYPE
CHANGEMENUITEM(LPCSTR name, HIDEDLL_NUMTYPE index, LPCSTR str)
{
	try {
		return g_pSessionInstance->si_changemenuitem(name,index,str,nullStr);
	} catch (...) {
		return 0;
	}
}

//	���j���[���ڂ��폜
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELETEMENUITEM(LPCSTR menuname, HIDEDLL_NUMTYPE index)
{
	try {
		return g_pSessionInstance->si_deletemenuitem(menuname,index);
	} catch (...) {
		return 0;
	}
}

