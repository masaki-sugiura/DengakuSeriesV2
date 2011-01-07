//	$Id: si_menu.cpp,v 1.8 2011-01-07 16:08:38 sugiura Exp $
/*
 *	si_menu.cpp
 *	���j���[�\���֐�
 */

#include "session.h"
#include "cmdline.h"
#include "menu.h"
#include "str_tbl.h"

//	�_�~�[�E�B���h�E�̃E�B���h�E�N���X��
LPCSTR pszClassName = "DengakuDLLWndClass";

const StringBuffer errorStr  = "!";
const StringBuffer cancelStr = "0";

//	�|�b�v�A�b�v���j���[�̐e�E�B���h�E�̃R�[���o�b�N�֐�
LRESULT CALLBACK
UserMenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE) {
		lParam = (LPARAM)((LPCREATESTRUCT)lParam)->lpCreateParams;
		::SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)lParam);
		//	�E�B���h�E�S�̂𕢂��悤�ɃT�C�Y�ύX
		RECT rct;
		::GetWindowRect(::GetDesktopWindow(),&rct);
		::SetWindowPos(hWnd,HWND_TOPMOST,0,0,rct.right,rct.bottom,
						SWP_HIDEWINDOW|SWP_NOREDRAW);
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	LPSHOWMENUPROCARGS
		lpsmpa = reinterpret_cast<LPSHOWMENUPROCARGS>(
									::GetWindowLongPtr(hWnd,GWLP_USERDATA));

	switch (uMsg) {
	case WM_USER_SHOWUSERMENU:
		{
			//	�|�b�v�A�b�v���j���[�̕\��
			if (lpsmpa->m_bShowAtCursor) {
				POINT pt;
				::GetCursorPos(&pt);
				lpsmpa->m_mnPosInfo.x += pt.x;
				lpsmpa->m_mnPosInfo.y += pt.y;
			}
			lpsmpa->m_wFirstShow = TRUE; // WM_INITMENUPOPUP �Q��
			HWND hwndTop = ::GetForegroundWindow();
			if (hwndTop != NULL) ::UpdateWindow(hwndTop);
			//	�E�B���h�E������(����������(^^;)
			::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,
						SWP_SHOWWINDOW|SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE);
			::SetForegroundWindow(hWnd);
			//	���j���[�̕\��
			lpsmpa->m_wSelectedID
				= (WORD)::TrackPopupMenu(lpsmpa->m_hMenu,
										TPM_RETURNCMD,
										lpsmpa->m_mnPosInfo.x,
										lpsmpa->m_mnPosInfo.y,
										0,hWnd,NULL);
			//	��ʂ̃N���A
			::InvalidateRect(hWnd,NULL,FALSE);
			::UpdateWindow(hWnd);
			//	�E�B���h�E���B��
			::SetWindowPos(hWnd,HWND_TOP,0,0,0,0,
						SWP_HIDEWINDOW|SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE);
			::DestroyWindow(hWnd);
			::SetForegroundWindow(hwndTop);
		}
		break;

	case WM_INITMENUPOPUP:
		//	���j���[���\������钼�O�Ƀ��j���[���ڂ��\�z
		if (lpsmpa != NULL) {
			lpsmpa->m_pSessionInstance->getUserMenu()
					.createMenu(reinterpret_cast<HMENU>(wParam));
			if (lpsmpa->m_wFirstShow) {
				// ���j���[���\������čŏ��̍��ڂ�I����Ԃɂ���
				// (���J�[�\���L�[�̃G�~�����[�V����(���))
				::PostMessage(hWnd, WM_KEYDOWN, VK_DOWN, 1);
				::PostMessage(hWnd, WM_KEYUP, VK_DOWN, 1);
				lpsmpa->m_wFirstShow = FALSE;
			}
		}
		break;

	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		//	���j���[�n���h���̍폜
		if (lpsmpa != NULL)
			lpsmpa->m_pSessionInstance->getUserMenu().deleteMenu();
		::PostQuitMessage(0L);
		break;

	default:
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 0;
}

DWORD WINAPI
ShowMenuProc(LPVOID pThreadArgs)
{
	if (pThreadArgs == NULL) {
		::ExitThread(1);	//	�L�蓾�Ȃ��͂��I
	}

	LPSHOWMENUPROCARGS psmpa = (LPSHOWMENUPROCARGS)pThreadArgs;

	WNDCLASS wc;
	::ZeroMemory(&wc,sizeof(WNDCLASS));
	wc.hInstance		=	psmpa->m_pSessionInstance->getInstanceHandle();
	wc.lpszClassName	=	pszClassName;
	wc.style			=	CS_CLASSDC|CS_DBLCLKS;
	wc.lpfnWndProc		=	(WNDPROC)UserMenuProc;
	wc.hbrBackground	=	(HBRUSH)/*(COLOR_BTNFACE+1)*/ NULL;
	::RegisterClass(&wc);

	psmpa->m_wSelectedID = MENURESULT_ERR_ID_MASK; // error

	DWORD ret = (DWORD)-1;
	//	(������)�_�~�[�E�B���h�E�̍쐬
	HWND hwndDummy = ::CreateWindowEx(WS_EX_TRANSPARENT /* | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE */,
									pszClassName,
									"DengakuDLL DummyWindow",
									WS_POPUP,
									0,0,		//	�l�̓_�~�[
									100,100,	//	�l�̓_�~�[
									NULL,
									NULL,
									(HINSTANCE)wc.hInstance,
									psmpa);
	if (hwndDummy != NULL) {
		//	���j���[�̕\��
		::PostMessage(hwndDummy,WM_USER_SHOWUSERMENU,0,0);
		//	���b�Z�[�W���[�v
		MSG	msg;
		while (::GetMessage(&msg,NULL,0,0)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		ret = msg.wParam;
	}
	::UnregisterClass(pszClassName, (HINSTANCE)wc.hInstance);

	psmpa->m_pSessionInstance->sendMenuResult(psmpa);

	::ExitThread(ret);
	return (DWORD)-1;	//	�L�蓾�Ȃ��I�I
}

BOOL
SessionInstance::showMenuThread(const StringBuffer& menuname,
								BOOL bShowAtCursor,
								POINT pt)
{
	if (menuname.length() <= 0 || m_pMenuThread.ptr() != NULL) return FALSE;

	//	���j���[�\���X���b�h�ɓn�������\����
	LPSHOWMENUPROCARGS psmpa = new SHOWMENUPROCARGS;
	if (psmpa == NULL) return FALSE;
	::ZeroMemory(psmpa, sizeof(SHOWMENUPROCARGS));
	psmpa->m_pSessionInstance = this;

	//	���O�Ŏw�肳�ꂽ���j���[(�Ƃ��̃T�u���j���[)���\�z
	psmpa->m_hMenu = m_UserMenu.precreateMenuRecursive(menuname);
	if (psmpa->m_hMenu == NULL) {
		delete psmpa;
		return FALSE;
	}
	psmpa->m_bShowAtCursor = bShowAtCursor;
	psmpa->m_mnPosInfo = pt;

	//	���j���[��\������X���b�h�̊J�n
	//	���j���[�̕\����ʃX���b�h�̃E�B���h�E�ɔC���闝�R�F
	//	���j���̃X���b�h���Ń��j���[��\���������
	//	�G�ۂ̃E�B���h�E���N���b�N�������_�Łu�}�N�����I�����܂����H�v�Ƃł�B
	//	����͓����E�B���h�E���i���̃X���b�h�Łj�쐬���Ă����l�B
	//	�}�N�����쒆�̏G�ۂ̓��b�Z�[�W���[�v��(DispatchMessage �ɓn���O)��
	//	WM_LBUTTONDOWN ���t�b�N���Ă�����̂Ǝv����B
	m_pMenuThread
		= new Thread((LPTHREAD_START_ROUTINE)ShowMenuProc,(LPVOID)psmpa);
	if (m_pMenuThread.ptr() == NULL) return FALSE;
	return m_pMenuThread->run();
}

void
SessionInstance::sendMenuResult(LPSHOWMENUPROCARGS psmpa)
{
	if (m_pMenuThread.ptr() == NULL ||
		psmpa == NULL ||
		(psmpa->m_wSelectedID&MENURESULT_ERR_ID_MASK) != 0) {
		this->setNotify(errorStr);
		return;
	}

	//	LOBYTE(id)	:	����ID(1�`)
	//	HIBYTE(id)	:	(index of Menu in MenuList) + 1.
	WORD id = psmpa->m_wSelectedID;
	if (id == 0) {	//	�L�����Z��
		this->setNotify(cancelStr);
		return;
	}
	//	�G���[�łȂ�
	int	index = (int)HIBYTE(id) - 1;
	Menu* menu = psmpa->m_pSessionInstance->m_UserMenu.getItemByIndex(index);
	if (menu == NULL) {
		this->setNotify(errorStr);
		return;
	}
	//	�Ԃ�l�� "menuname:index"
	this->setNotify(StringBuffer(menu->getName())
								.append((TCHAR)':')
								.append((int)(UINT)LOBYTE(id)));
}

DWORD
SessionInstance::waitMenuThread(DWORD waittime)
{
	if (m_pMenuThread.ptr() == NULL) return THREAD_INVALID_EXITCODE;
	//	�X���b�h����~����܂ő҂�
	return m_pMenuThread->wait(waittime);
}

DWORD
SessionInstance::stopMenuThread(DWORD waittime)
{
	if (m_pMenuThread.ptr() == NULL) return THREAD_INVALID_EXITCODE;
	DWORD exitcode = m_pMenuThread->getExitCode();
	LPSHOWMENUPROCARGS psmpa = (LPSHOWMENUPROCARGS)m_pMenuThread->getProcArg();
	// clean up the background window of the menu
	HWND hwndBack = ::WindowFromPoint(psmpa->m_mnPosInfo);
	if (hwndBack != NULL) {
		::InvalidateRect(hwndBack,NULL,FALSE);
		::UpdateWindow(hwndBack);
//		::SetForegroundWindow(hwndBack);
	}
	delete psmpa;
	m_pMenuThread = NULL; // Thread ��j��
	return exitcode;
}

int
SessionInstance::si_newmenu(const StringBuffer& name, BOOL bExtended)
{
	if (name.length() <= 0) {
		return FALSE;
	}
	//	�V�������j���[�̊��蓖��
	return m_UserMenu.addMenu(new Menu(name, bExtended)) > 0;
}

int
SessionInstance::si_addmenuitem(
	const StringBuffer& name,
	const StringBuffer& title,
	const StringBuffer& submenu,
	const StringBuffer& opt)
{
	//	�����Ώۂ̃��j���[������
	Menu* menu = m_UserMenu.getMenu(name);
	if (menu == NULL) return FALSE;
	if (title.length() <= 0) {
		//	�Z�p���[�^�̑}��
		return menu->addItem(new MenuItem(MENUTYPE_SEPARATOR)) > 0;
	} else if (submenu.length() <= 0) {
		//	�ʏ�̍��ڂ̑}��
		return menu->addItem(new MenuItem(MENUTYPE_ITEM,title,nullStr,opt)) > 0;
	} else {
		//	�T�u���j���[�������ڂ̑}��
		return menu->addItem(new MenuItem(MENUTYPE_POPUP,title,submenu,opt)) > 0;
	}
}

int
SessionInstance::si_changemenuitem(
	const StringBuffer& name,
	int id,
	const StringBuffer& text,
	const StringBuffer& state)
{
	//	�����Ώۂ̃��j���[������
	Menu* menu = m_UserMenu.getMenu(name);
	if (menu == NULL) return FALSE;
	//	�����Ώۂ̃��j���[���ڂ�����
	MenuItem* item = menu->getItemByIndex(id - 1);
	if (item == NULL) return FALSE;
	item->setText(text);	//	���ڃe�L�X�g���X�V
	item->setState(state);	//	���ڂ̏�Ԃ��X�V
	return TRUE;
}

int
SessionInstance::si_deletemenuitem(const StringBuffer& name, int id)
{
	//	�����Ώۂ̃��j���[������
	Menu* menu = m_UserMenu.getMenu(name);
	if (menu == NULL) return FALSE;
	if (id <= 0) {
		//	�S�Ă̍��ڂ��폜
		while (menu->delItemByIndex(0) > 0)
			/* no operation */;
	} else {
		//	�w�荀�ڂ��폜
		menu->delItemByIndex(id - 1);
	}
	return TRUE;
}

