//	$Id: main.cpp,v 1.3 2002-02-20 16:48:40 sugiura Exp $
/*
 *	main.cpp
 *	�c�y�T�[�o�{��
 */

#include "resource.h"
#include "common.h"
#include "ddeserv.h"
#include "ddesinfo.h"
#include "misc.h"

#include <commctrl.h>

/*
    �R�}���h���C�����߁�DDEServerInfo�\���̂ւ̃f�[�^�̊i�[�����s����֐�

	�T�|�[�g���Ă���R�}���h���C���I�v�V�����͈ȉ��̒ʂ�
	    /h                  :   �A�C�R�����B��
	    /e                  :   �}�N���T�[�o�݊����[�h
	    /s ServiceName      :   ��{�T�[�r�X���� ServiceName �ɕς���
	    /l [LogFileName]    :   �T�[�o�̃��O���擾

    �����s�t�@�C������ macserv.exe �������ꍇ�A�܂��͊�{�T�[�r�X����
      "HideMacroServer" �������ꍇ�A�}�N���T�[�o�݊����[�h���L���ɂȂ�B
*/
static BOOL
ParseCmdLine(DDEServerInfo& dsi)
{
//	Windows �v���O�����ł� __argc, __argv �ŃR���\�[���v���O�����̂悤��
//	�R�}���h���C�����擾�ł���c�炵��(^^;�B
//	�ŁA����(^^;�̂ŕʖ����`
#ifdef __GNUC__
extern int _argc;
extern char** _argv;
#define ac _argc
#define av _argv
#else
#define	ac __argc
#define	av __argv
#endif
//	�s���ȃp�����[�^�̃`�F�b�N
	if (ac < 1) return FALSE;

//	�t���p�X�̎��s�t�@�C�������擾
	dsi.m_sbModuleName.resize(MAX_PATH);
	::GetModuleFileName(dsi.m_hInst,dsi.m_sbModuleName.getBufPtr(),MAX_PATH);
	if (dsi.m_sbModuleName.length() <= 0) return FALSE;

//	���s�t�@�C������ "macserv.exe" �Ȃ�}�N���T�[�o�݊����[�h���n�m�ɂ���
	LPCSTR ptr = lstrrchr((LPCSTR)dsi.m_sbModuleName,'\\');
	if (ptr != NULL && lstrcmpi(++ptr,GetServerString(SVR_STR_MSEXENAME)) == 0)
		dsi.m_bStrictEmulation = TRUE;

//	�I�v�V�����̉��
	//	/s, /l �I�v�V�����̎��̕�������i�[����o�b�t�@�ւ̃|�C���^
	StringBuffer* ppopt = NULL;
	for (char** pptr = av; *pptr != NULL; pptr++) {
		switch (**pptr) {
		case '/':	//	�I�v�V��������("/" �� "-" ���g�p�\)
		case '-':
			switch (*++*pptr) {
			case 'h':	//	�A�C�R�����B��
				dsi.m_bHideIcon = TRUE;
				ppopt = NULL;
				break;
			case 's':	//	��{�T�[�r�X���̐ݒ�
				if (!dsi.m_bStrictEmulation) ppopt = &dsi.m_sbServiceName;
				break;
			case 'l':	//	�T�[�o�̃��O�����
				if (!dsi.m_psbServerLogFileName) {
					try {
						dsi.m_psbServerLogFileName
							= new StringBuffer(nullStr);
					} catch (exception&) {
						//	no operation.
					}
				}
				ppopt = dsi.m_psbServerLogFileName;
				break;
			case 'e':	//	�}�N���T�[�o�݊����[�h���n�m
				dsi.m_bStrictEmulation = TRUE;
				ppopt = NULL;
				break;
			default:	//	���ߕs�\�ȃI�v�V����
				ppopt = NULL;
			}
			//	/s, /l �I�v�V�����w�莞�� "/sServicename" �̗l��
			//	�����������Ă���ꍇ�ւ̑Ώ�
			if (ppopt != NULL && *++*pptr) {
				*ppopt = *pptr;
				ppopt = NULL;
			}
			break;

		default:	//	"/?"("-?") �łȂ�������̏ꍇ
			if (ppopt != NULL) {
				//	���O�� "/s" �܂��� "/l" �������ꍇ�ւ̑Ώ�
				*ppopt = *pptr;
				ppopt = NULL;
			}
		}
	}

	//	��{�T�[�r�X���̐ݒ�
	if (dsi.m_bStrictEmulation)
		//	�}�N���T�[�o�̊�{�T�[�r�X��(="HideMacroServer")
		dsi.m_sbServiceName = GetServerString(SVR_STR_MSSERVICENAME);
	else if (dsi.m_sbServiceName.length() <= 0)
		//	�f�t�H���g�̊�{�T�[�r�X��(="DengakuServer")�ɐݒ�
		dsi.m_sbServiceName = GetServerString(SVR_STR_DSSERVICENAME);

	//	�f�t�H���g�̃��O�t�@�C�����̐ݒ�
	if (dsi.m_psbServerLogFileName != NULL &&
		dsi.m_psbServerLogFileName->length() <= 0) {
		StringBuffer& lfn = *dsi.m_psbServerLogFileName;
		lfn = dsi.m_sbModuleName;
		int sep = lfn.rfind((TCHAR)'\\');
		lfn.setlength(++sep);
		lfn.append(dsi.m_sbServiceName).append(".log");
	}

	//	�~���[�e�b�N�X�I�u�W�F�N�g�̖��O�̐���
	dsi.m_sbMutexName = GetServerString(SVR_STR_DSMUTEXNAME);
	dsi.m_sbMutexName.append(dsi.m_sbServiceName);

	//	���C���E�B���h�E�̃^�C�g���̐���
	dsi.m_sbWndTitle = GetServerString(SVR_STR_SERVERNAME);
	dsi.m_sbWndTitle.append(" - ").append(dsi.m_sbServiceName);

	return TRUE;
}

//	���C���E�B���h�E������������֐�
static BOOL
InitMainWindow(DDEServerInfo* pdsi)
{
	//	�E�B���h�E�^�C�g���̓o�^
	::SetWindowText(pdsi->m_hwndMain,pdsi->m_sbWndTitle);

	//	�A�v�����A�o�[�W�������
	StringBuffer name(GetServerString(SVR_STR_SERVERNAME),-1,16);
	name.append(" Ver.").append(GetServerString(SVR_STR_VERSION));
	::SetDlgItemText(pdsi->m_hwndMain,IDC_APPNAME,name);

	//	�^�X�N�g���C�ɃA�C�R����o�^����\���̂̏�����
	NOTIFYICONDATA* lptnid = &pdsi->m_NotifyIconData;
	lptnid->cbSize = sizeof(NOTIFYICONDATA);
	lptnid->hWnd = pdsi->m_hwndMain;
	lptnid->uID = 1;
	lptnid->uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	lptnid->uCallbackMessage = WM_USER_MENU;	//	MainWndProc �ւ� MSG
	lptnid->hIcon = (HICON)::LoadImage(pdsi->m_hInst,
										MAKEINTRESOURCE(IDI_ICON),
										IMAGE_ICON,
										16,16,0);
	lstrcpy(lptnid->szTip,pdsi->m_sbWndTitle);

	//	�E�B���h�E����ʒ����Ɉړ�
	CenteringWindow(pdsi->m_hwndMain, ::GetDesktopWindow(),
					SWP_NOACTIVATE|SWP_NOREDRAW|SWP_NOSIZE|SWP_HIDEWINDOW);
	::SetWindowPos(pdsi->m_hwndMain, HWND_TOPMOST, 0, 0, 0, 0,
					SWP_NOACTIVATE|SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE|SWP_HIDEWINDOW);

	return TRUE;
}


/*
	���C���E�B���h�E�̃E�B���h�E�v���V�[�W��

	����Ă��邱�ƁF
		�E�A�C�R���̓o�^(����)
		�EDDE �T�[�o�̊J�n/�I��
		�E�A�C�R������̃��b�Z�[�W(WM_LBUTTONDBLCLK, WM_RBUTTONDOWN)�̏���
		�E���j���[����̃��b�Z�[�W�̏���

*/
BOOL CALLBACK
MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DDEServerInfo*
		pdsi = reinterpret_cast<DDEServerInfo*>(::GetWindowLong(hWnd,
																DWL_USER));

	switch (uMsg) {
	case WM_INITDIALOG:	//	���C���E�B���h�E�̏���������
		//	lParam : �T�[�o���̓������\���̂ւ̃|�C���^
		::SetWindowLong(hWnd,DWL_USER,lParam);
		pdsi = reinterpret_cast<DDEServerInfo*>(lParam);

		//	�E�B���h�E�n���h���̓o�^
		pdsi->m_hwndMain = hWnd;

		//	���C���E�B���h�E�̏������{��
		if (!InitMainWindow(pdsi)) {
			ErrorMessageBox(MSG_ERR_START);
			::DestroyWindow(hWnd);
			return TRUE;
		}

		//	�^�X�N�g���C�ɃA�C�R����o�^
		if (!pdsi->m_bHideIcon)
			::Shell_NotifyIcon(NIM_ADD,&pdsi->m_NotifyIconData);

		//	DDE �T�[�o�̊J�n
		if (!DdeServer::init(*pdsi)) {
			ErrorMessageBox(MSG_ERR_START);
			::DestroyWindow(hWnd);
		}
		return FALSE;


	case WM_COMMAND:	//	�R���g���[��/���j���[����̃��b�Z�[�W����
		switch (LOWORD(wParam)) {
		case IDOK:	//	�u�n�j�v�{�^���������ꂽ
			//	���C���E�B���h�E���ĂщB��
			::ShowWindow(hWnd,SW_HIDE);
			break;

		case IDM_ABOUT:	//	���j���[�́u�c�y�T�[�o�ɂ��āv���I�����ꂽ
			//	��{�T�[�r�X����\��
			::SetDlgItemText(hWnd,IDC_SERVICENAME,pdsi->m_sbServiceName);
			//	���݊m�����Ă��� DDE �ʐM�̐���\��
			::SetDlgItemInt(hWnd,IDC_CONVNUM,DdeServer::getConvNum(),FALSE);
			//	���C���E�B���h�E��\��
			::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,
							SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
			break;

		case IDM_HELP:		//	���j���[�́u�w���v�v���I�����ꂽ
		case IDC_SHOWHELP:	//	�u�w���v�v�{�^���������ꂽ
			{
				//	�w���v�t�@�C�����̐���
				StringBuffer buf(pdsi->m_sbModuleName);
				buf.setlength(buf.length()-3);
				buf.append("hlp");
				//	�w���v�̌ďo��
				::WinHelp(hWnd,buf,HELP_FINDER,0L);
			}
			break;

		case IDM_QUIT:	//	���j���[�́u�I���v���I�����ꂽ
		case IDC_CLOSE:	//	�u�I���v�{�^���������ꂽ
			//	�I���{�^��/���j���[���ڂ�I��
			::DestroyWindow(hWnd);
			break;

		}
		break;

	case WM_CLOSE:	//	�I�����b�Z�[�W
		::DestroyWindow(hWnd);
		break;

	case WM_DESTROY:	//	�T�[�o�̏I��
		if (!pdsi->m_bHideIcon)
			::Shell_NotifyIcon(NIM_DELETE,&pdsi->m_NotifyIconData);
		if (!DdeServer::uninit()) ErrorMessageBox(MSG_ERR_STOP);
		::PostQuitMessage(0);
		break;

	case WM_USER_MENU:	//	�^�X�N�g���C�A�C�R������̃��b�Z�[�W
		switch (LOWORD(lParam)) {
		case WM_LBUTTONDBLCLK:	//	���_�u���N���b�N
			//	���C���E�B���h�E��\��
			::PostMessage(hWnd,WM_COMMAND,IDM_ABOUT,0L);
			break;

		case WM_RBUTTONDOWN:	//	�E�N���b�N
			{
				//	���j���[��\��
				POINT pt;
				::SetForegroundWindow(hWnd);
				::GetCursorPos(&pt);
				::TrackPopupMenu(
							::GetSubMenu(
									::LoadMenu(pdsi->m_hInst,
												MAKEINTRESOURCE(IDR_MENU)),
									0),
							TPM_RIGHTALIGN|TPM_RIGHTBUTTON,
							pt.x, pt.y, 0, hWnd, NULL);
			}
			break;
		}
		break;

	case WM_USER_DDE_STOP:	//	DDE �ʐM/�T�[�o�̏I��
		if (DdeServer::isActive()) {
			//	(ConvData*)lParam != NULL : ����̒ʐM�̏I��
			//					   = NULL : �S�ʐM�̏I��
			DdeServer::deleteConvData(reinterpret_cast<ConvData*>(lParam));
			if (!lParam) ::DestroyWindow(hWnd);	//	�T�[�o���I��
		}
		break;

	case WM_USER_SHOWICON:	//	�^�X�N�g���C�̃A�C�R���̕\��/��\���̐ؑ�
		if (static_cast<WPARAM>(pdsi->m_bHideIcon) != wParam) {
			pdsi->m_bHideIcon = wParam;
			::Shell_NotifyIcon(pdsi->m_bHideIcon ? NIM_DELETE : NIM_ADD,
								&pdsi->m_NotifyIconData);
		}
		break;

	case WM_USER_NOTIFYPOSTADV:	//	���̃v���Z�X���� DdePostAdvise() ���Ă�
		DdeServer::notifyPostAdvise((ConvData*)lParam);
		break;

	default:
		return FALSE;

	}
	return TRUE;
}

#ifdef __GNUC__

static LPCSTR
	pszClassName = "DengakuServerClass",
	pszCaption = "�c�y�T�[�o�ɂ���",
	pszFontName = "�l�r �o�S�V�b�N",
	pszOK = "OK",
	pszHelp = "�w���v",
	pszQuit = "�I��",
	pszTitle = "�c�y (�ł񂪂�) �T�[�o  Ver.2.00��",
	pszCopyRight = "Copyright(C) 1998-2002, M. Sugiura (sugiura@ceres.dti.ne.jp)",
	pszLabelConnect = "���݂� DDE �ʐM",
	pszLabelService = "�T�[�r�X���F",
	pszLabelNumber  = "�ʐM���F";

#define CLASSNAME_SIZE (19 * sizeof(WORD))
#define CAPTION_SIZE   (10 * sizeof(WORD))
#define FONTNAME_SIZE  ( 9 * sizeof(WORD))
#define OK_SIZE        ( 3 * sizeof(WORD))
#define HELP_SIZE      ( 4 * sizeof(WORD))
#define QUIT_SIZE      ( 3 * sizeof(WORD))
#define TITLE_SIZE     (25 * sizeof(WORD))
#define COPYRIGHT_SIZE (61 * sizeof(WORD))
#define CONNECT_SIZE   (11 * sizeof(WORD))
#define SERVICE_SIZE   ( 7 * sizeof(WORD))
#define NUMBER_SIZE    ( 5 * sizeof(WORD))

#define DLGTEMPL_SIZE 1024 * 256

#define IDC_STATIC (WORD)(-1)

static LPWSTR
AddDlgItem(
	LPWSTR pwStr,
	DWORD style,
	WORD x, WORD y, WORD cx, WORD cy, WORD id,
	WORD cls,
	LPCSTR title)
{
	LPDLGITEMTEMPLATE pdit = (LPDLGITEMTEMPLATE)pwStr;
	pdit->style = style;
	pdit->dwExtendedStyle = 0;
	pdit->x = x;
	pdit->y = y;
	pdit->cx = cx;
	pdit->cy = cy;
	pdit->id = id;
	pwStr += sizeof(DLGITEMTEMPLATE) / sizeof(WORD);
	*pwStr++ = 0xFFFF;
	*pwStr++ = cls;
	pwStr += StringBuffer(title).toUnicode(pwStr);
	if ((((DWORD)pwStr) & 0x3) != 0) *pwStr++ = 0;
	return pwStr;
}

static HWND
CreateMainDialog(HINSTANCE hInst, LPARAM lParam)
{
	Array<BYTE> pTemplate(DLGTEMPLSIZE);
	LPDLGTEMPLATE pdt = (LPDLGTEMPLATE)(BYTE*)pTemplate;

	pdt->style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
	pdt->dwExtendedStyle = 0;
	pdt->cdit = 10;
	pdt->x = 0;
	pdt->y = 0;
	pdt->cx = 229;
	pdt->cy = 109;
	LPWSTR pwStr = (LPWSTR)(pTemplate + sizeof(DLGTEMPLATE));
	*pwStr++ = 0; // no menu name
	pwStr += StringBuffer(pszClassName).toUnicode(pwStr);
	pwStr += StringBuffer(pszCaption).toUnicode(pwStr);
	*pwStr++ = 9;
	pwStr += StringBuffer(pszFontName).toUnicode(pwStr);
	if ((((DWORD)pwStr) & 0x3) != 0) *pwStr++ = 0;

	// OK button
	pwStr = AddDlgItem(pwStr,
						BS_DEFPUSHBUTTON | BS_PUSHBUTTON |
							WS_CHILD | WS_TABSTOP | WS_GROUP,
						57, 88, 50, 14, IDOK, 0x0080,
						pszOK);
	// Help button
	pwStr = AddDlgItem(pwStr,
						BS_PUSHBUTTON | WS_CHILD | WS_TABSTOP | WS_GROUP,
						114, 88, 50, 14, IDC_SHOWHELP, 0x0080,
						pszHelp);
	// Quit button
	pwStr = AddDlgItem(pwStr,
						BS_PUSHBUTTON | WS_CHILD | WS_TABSTOP | WS_GROUP,
						171, 88, 50, 14, IDC_CLOSE, 0x0080,
						pszQuit);
	// Title text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						33, 13, 108, 8, IDC_APPNAME, 0x0082,
						pszTitle);
	// Copyright text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						33, 23, 187, 8, IDC_STATIC, 0x0082,
						pszCopyRight);
	// Connect groupbox
	pwStr = AddDlgItem(pwStr,
						BS_GROUPBOX | WS_CHILD | WS_GROUP,
						7, 39, 215, 47, IDC_STATIC, 0x0080,
						pszLabelConnect);
	// Service title text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						13, 52, 41, 8, IDC_STATIC, 0x0082,
						pszLabelService);
	// Number title text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						26, 65, 28, 8, IDC_STATIC, 0x0082,
						pszLabelNumber);
	// Service text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						57, 52, 128, 8, IDC_SERVICENAME, 0x0082,
						"");
	// Number text
	pwStr = AddDlgItem(pwStr,
						WS_CHILD | WS_GROUP,
						57, 65, 128, 8, IDC_CONVNUM, 0x0082,
						"");

	HWND hDlg = ::CreateDialogIndirectParam(hInst,
											(LPCDLGTEMPLATE)pTemplate,
											NULL,
											(DLGPROC)MainWndProc,
											lParam);

	return hDlg;
}

#endif // __GNUC__

static void
CloseStdHandle(ULONG hStd)
{
	HANDLE hStdHandle;
	if ((hStdHandle = ::GetStdHandle(hStd)) != NULL) {
		::CloseHandle(hStdHandle);
		::SetStdHandle(hStd,NULL);
	}
}

int APIENTRY
WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine,
	int nCmdShow)
{
	int	ret_code = 0;	//	���^�[���R�[�h

	try {

	//	�T�[�o�����i�[����N���X�̐���
	Auto_Ptr<DDEServerInfo>	pDdeSI(new DDEServerInfo(hInstance));

	//	�R�}���h���C���̉��߁��T�[�r�X��etc.�̎擾
	if (!ParseCmdLine(*pDdeSI)) return 1;

	//	�����̃C���X�^���X�̋N����}�~���邽�� Mutex �I�u�W�F�N�g���쐬�B
	//	�A���AMutex �̖��O�͊�{�T�[�r�X�����ɌŗL�Ȃ̂ŁA
	//	�ʂ̊�{�T�[�r�X�������C���X�^���X�͕����N���ł���B
	HANDLE hMutex = ::CreateMutex(NULL,FALSE,pDdeSI->m_sbMutexName);
	if (::GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND hwndPrev = ::FindWindow(GetServerString(SVR_STR_DSCLASSNAME),
									 pDdeSI->m_sbWndTitle);
		//	�^�X�N�g���C�̃A�C�R���̏�Ԃ�ύX
		if (hwndPrev != NULL)
			::SendMessage(hwndPrev, WM_USER_SHOWICON, pDdeSI->m_bHideIcon, 0);

		//	���̃v���Z�X�͏I��
		::CloseHandle(hMutex);
		return 0;
	}

	//	���̏��������Ȃ��Ɛe�v���Z�X���e�t�@�C����
	//	���_�C���N�g���Ă���ꍇ�� DOS �A�v��������ɋN�����Ȃ��B
	//	���R�̓C�}�C�`�悭������Ȃ����Ώ��Ö@�I����
	//	�c�܂����傤���Ȃ��낤(^^;�B
	CloseStdHandle(STD_INPUT_HANDLE);
	CloseStdHandle(STD_OUTPUT_HANDLE);
	CloseStdHandle(STD_ERROR_HANDLE);

	//	�R�����R���g���[�����C�u�����̏�����
	::InitCommonControls();

	//	(����(^^;��)���C���E�B���h�E�̃E�B���h�E�N���X�̓o�^
	WNDCLASS wc;
	::ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style = CS_CLASSDC|CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)DefDlgProc;	//	���C���E�B���h�E��
	wc.cbWndExtra = DLGWINDOWEXTRA;			//	�_�C�A���O
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
	wc.lpszClassName = GetServerString(SVR_STR_DSCLASSNAME);

	HWND hwndDlg;
	if (::RegisterClass(&wc) &&
#ifdef __GNUC__
		(hwndDlg = CreateMainDialog(hInstance, (LPARAM)pDdeSI.ptr()))) {
#else
		(hwndDlg = ::CreateDialogParam(hInstance,
										MAKEINTRESOURCE(IDD_ABOUT),
										NULL,
										(DLGPROC)MainWndProc,
										(LPARAM)pDdeSI.ptr()))) {
#endif
		//	����܂����񑩂̃��b�Z�[�W���[�v
		MSG msg;
		while (::GetMessage(&msg,NULL,0,0)) {
			//	���C���E�B���h�E(�_�C�A���O)�ւ̃��b�Z�[�W�z��
			if (!::IsDialogMessage(hwndDlg,&msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		ret_code = msg.wParam;
	} else {
		ErrorMessageBox(MSG_ERR_START);
		ret_code = 1;
	}

	//	��n��
	::CloseHandle(hMutex);

	} catch (...) {
		return -1;
	}

	return ret_code;
}

