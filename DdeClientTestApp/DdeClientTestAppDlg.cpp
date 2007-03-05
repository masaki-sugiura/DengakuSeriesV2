// DdeClientTestAppDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "DdeClientTestApp.h"
#include "DdeClientTestAppDlg.h"

#include <ddeml.h>

#define	WM_USER_SHOWDIALOG	(WM_APP+100)
#define	WM_USER_ENDDIALOG	(WM_APP+101)

#define	DDE_TRANSACTION_TIMEOUT	30

#ifdef	_UNICODE
#define	CODE_PAGE	CP_WINUNICODE
#else
#define	CODE_PAGE	CP_WINANSI
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class DdeStringHandle
{
public:
	DdeStringHandle(DWORD dwIdInst, LPCTSTR pszString)
		:	m_dwIdInst(dwIdInst)
	{
		m_hszString = ::DdeCreateStringHandle(dwIdInst, pszString, CODE_PAGE);
		ASSERT(m_hszString != NULL);
	}
	~DdeStringHandle()
	{
		if (m_hszString != NULL) {
			::DdeFreeStringHandle(m_dwIdInst, m_hszString);
		}
	}

	operator HSZ() { return m_hszString; }

private:
	DWORD	m_dwIdInst;
	HSZ		m_hszString;
};


// CDdeClientTestAppDlg �_�C�A���O

CDdeClientTestAppDlg* CDdeClientTestAppDlg::m_pThis;


CDdeClientTestAppDlg::CDdeClientTestAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDdeClientTestAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	ASSERT(m_pThis == NULL);

	m_pThis = this;
}

void CDdeClientTestAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDdeClientTestAppDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER_SHOWDIALOG, &CDdeClientTestAppDlg::OnShowDialog)
	ON_MESSAGE(WM_USER_ENDDIALOG, &CDdeClientTestAppDlg::OnEndDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDdeClientTestAppDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SHOWDIALOG, &CDdeClientTestAppDlg::OnBnClickedShowdialog)
END_MESSAGE_MAP()


// CDdeClientTestAppDlg ���b�Z�[�W �n���h��

BOOL CDdeClientTestAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	if (InitializeDde()) {
//		PostMessage(WM_USER_SHOWDIALOG);
	}

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CDdeClientTestAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CDdeClientTestAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDdeClientTestAppDlg::InitializeDde()
{
	m_dwIdInst = 0;
	UINT uRet = ::DdeInitialize(&m_dwIdInst, (PFNCALLBACK)DdeClientCallback, APPCMD_CLIENTONLY, 0);

	ASSERT(uRet == DMLERR_NO_ERROR);

	if (uRet != DMLERR_NO_ERROR) {
		return FALSE;
	}

	DdeStringHandle hszService(m_dwIdInst, __TEXT("DengakuServer")),
					hszTopic(m_dwIdInst, __TEXT("AllService"));

	if (hszService == NULL || hszTopic == NULL) {
		return FALSE;
	}

	m_hConv = ::DdeConnect(m_dwIdInst, hszService, hszTopic, NULL);

	return m_hConv != NULL;
}

void CDdeClientTestAppDlg::UninitializeDde()
{
	::DdeDisconnect(m_hConv);

	::DdeUninitialize(m_dwIdInst);
}

BOOL CDdeClientTestAppDlg::DdeControlAdviseLoop(BOOL bStart)
{
	DdeStringHandle hszItem(m_dwIdInst, __TEXT("dlgresult"));

	DWORD dwResult;
	HDDEDATA hData = ::DdeClientTransaction(NULL, 0, m_hConv, hszItem, CF_TEXT,
											bStart ? XTYP_ADVSTART : XTYP_ADVSTOP,
											DDE_TRANSACTION_TIMEOUT, &dwResult);

	ASSERT(hData != NULL);

	return (hData != NULL);
}

HDDEDATA CDdeClientTestAppDlg::DdeExecute(LPCSTR pszCommand)
{
	DWORD dwResult;
	HDDEDATA hData = ::DdeClientTransaction((LPBYTE)pszCommand, strlen(pszCommand) + 1, m_hConv, NULL, 0,
											XTYP_EXECUTE, DDE_TRANSACTION_TIMEOUT, &dwResult);

	return hData;
}

HDDEDATA CDdeClientTestAppDlg::DdeRequest(LPCSTR pszCommand, CString& strReturn)
{
	DdeStringHandle hszCommand(m_dwIdInst, pszCommand);

	DWORD dwResult;
	HDDEDATA hData = ::DdeClientTransaction(NULL, 0, m_hConv, hszCommand, 0,
											XTYP_REQUEST, DDE_TRANSACTION_TIMEOUT, &dwResult);
	if (hData) {
		DWORD dwLength = ::DdeGetData(hData, NULL, 0, 0);
		char* buf = new char[dwLength];
		::DdeGetData(hData, (LPBYTE)buf, dwLength, 0);
		strReturn = buf;
		delete [] buf;
	}

	return hData;
}

HDDEDATA CDdeClientTestAppDlg::DdePoke(LPCTSTR pszCommand, LPCSTR pszData)
{
	DdeStringHandle hszCommand(m_dwIdInst, pszCommand);

	HDDEDATA hPokeData = ::DdeCreateDataHandle(m_dwIdInst, (LPBYTE)pszData, strlen(pszData) + 1, 0, hszCommand, CF_TEXT, 0);

	DWORD dwResult;
	HDDEDATA hData = ::DdeClientTransaction((LPBYTE)hPokeData, -1, m_hConv, hszCommand, 0,
											XTYP_POKE, DDE_TRANSACTION_TIMEOUT, &dwResult);

	return hData;
}

void CDdeClientTestAppDlg::OnDdeAdvData(HDDEDATA hData)
{
	DWORD dwLength = ::DdeGetData(hData, NULL, 0, 0);

	BYTE* pDst = new BYTE[dwLength];

	::DdeGetData(hData, pDst, dwLength, 0);

	if (strcmp((LPCSTR)pDst, "1") == 0 || strcmp((LPCSTR)pDst, "0") == 0) {
		PostMessage(WM_USER_ENDDIALOG);
	} else if (strcmp((LPCSTR)pDst, "11") == 0) {
		CString strReturn;
		hData = DdeRequest("getstate skipsearch", strReturn);
		if (strReturn == __TEXT("0")) {
			// �X�L�b�v�����̃`�F�b�N���O���ꂽ
			hData = DdeExecute("enablectrl 0 skip");
        } else if (strReturn == __TEXT("1")) {
			// �X�L�b�v�������`�F�b�N���ꂽ
			hData = DdeExecute("enablectrl 1 skip");
			hData = DdeExecute("setstate field 0");
			hData = DdeExecute("enablectrl 0 mark number");
			hData = DdeRequest("getstate search_option", strReturn);
			if (strReturn == __TEXT("1")) {
				hData = DdeExecute("setstate search_option 2");
			}
		}
	} else if (strcmp((LPCSTR)pDst, "12") == 0) {
		CString strReturn;
		hData = DdeRequest("getstate field", strReturn);
        if (strReturn == __TEXT("1")) {
			// field ���`�F�b�N���ꂽ
			hData = DdeExecute("enablectrl 1 mark number");
			hData = DdeExecute("setstate skipsearch 0");
			hData = DdeExecute("enablectrl 0 skip");
			hData = DdeRequest("getstate search_option", strReturn);
			if (strReturn == __TEXT("1")) {
				hData = DdeExecute("setstate search_option 2");
			}
		} else if (strReturn == __TEXT("0")) {
			hData = DdeExecute("enablectrl 0 mark number");
		}
	} else if (strcmp((LPCSTR)pDst, "13") == 0) {
		CString strReturn;
		hData = DdeRequest("getstate search_option", strReturn);
		if (strReturn == __TEXT("1")) {
			// �ʏ� ���`�F�b�N���ꂽ
			hData = DdeExecute("setstate skipsearch 0");
			hData = DdeExecute("setstate field 0");
			hData = DdeExecute("enablectrl 0 skip mark number");
        }
	} else if (strcmp((LPCSTR)pDst, "14") == 0) {
		CString strReturn;
		hData = DdeRequest("getstring folderref", strReturn);
		hData = DdeExecute(CString("setstring folder \"") + strReturn + "\"");
		hData = DdeExecute(CString("changeitem folderref \"") + strReturn + "\" 2");
	}

	delete [] pDst;
}

HDDEDATA CALLBACK CDdeClientTestAppDlg::DdeClientCallback(UINT uType,
														  UINT uFmt,
														  HCONV hconv,
														  HSZ hsz1,
														  HSZ hsz2,
														  HDDEDATA hdata,
														  ULONG_PTR dwData1,
														  ULONG_PTR dwData2)
{
	ASSERT(m_pThis);

	switch (uType) {
	case XTYP_ADVDATA:
		m_pThis->OnDdeAdvData(hdata);
		break;
	default:
		break;
	}

	return (HDDEDATA)DDE_FACK;
}

BOOL CDdeClientTestAppDlg::CreateDengakuDialog()
{
	HDDEDATA hData;

	// �O���[�v�{�b�N�X�ň͂܂�镔��(�q�_�C�A���O)
	hData = DdePoke(__TEXT("newdlgpage"), "page1,38");
	hData = DdePoke(__TEXT("newcontrol"), "check,skipsearch,�X�L�b�v����������(TXK)");
	hData = DdePoke(__TEXT("default"), "0");
	hData = DdePoke(__TEXT("notify"), "11");
	hData = DdePoke(__TEXT("newcontrol"), "text,,");	//(�_�~�[)
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,�X�L�b�v�����E�L��");
	hData = DdePoke(__TEXT("part"), "16");
	hData = DdePoke(__TEXT("newcontrol"), "combo,skip,");
	for (int i = 0; i < 6; i++) {
		char buf[2] = { 'A' + i, 0 };
		hData = DdePoke(__TEXT("item"), buf);
    }
	hData = DdePoke(__TEXT("part"), "16");
	hData = DdePoke(__TEXT("newcontrol"), "text,,");	//(�_�~�[)

	hData = DdePoke(__TEXT("newdlgpage"), "page2,38");
	hData = DdePoke(__TEXT("newcontrol"), "check,field,�s�̌����t�B�[���h���w�肷��(app)");
	hData = DdePoke(__TEXT("default"), "0");
	hData = DdePoke(__TEXT("notify"), "12");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(�_�~�[)
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,��؂�L��");
	hData = DdePoke(__TEXT("part"), "10");
	hData = DdePoke(__TEXT("newcontrol"), "edit,mark,\",\"");
	hData = DdePoke(__TEXT("imestate"), "2"); // IME �� OFF
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(�_�~�[)
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(�_�~�[)
	hData = DdePoke(__TEXT("part"), "6");
	hData = DdePoke(__TEXT("newcontrol"), "text,,�s�����͈͂̍s������̋L����");
	hData = DdePoke(__TEXT("part"), "25");
	hData = DdePoke(__TEXT("newcontrol"), "edit,number,0");
	hData = DdePoke(__TEXT("imestate"), "2"); // IME �� OFF
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(�_�~�[)

	// �e�_�C�A���O
	hData = DdePoke(__TEXT("newdialog"), "40,�� �O��s�\���E�Q�s�n�茟���E�����ꋭ�� ��     Ver.3.20");
	hData = DdePoke(__TEXT("newcontrol"), "text,,������");
	hData = DdePoke(__TEXT("part"), "7");
	hData = DdePoke(__TEXT("newcontrol"), "combo,search,");
	for (int i = 0; i < 21; i++) {
		char buf[2] = { 'A' + i, 0 };
		hData = DdePoke(__TEXT("item"), buf);
    }
	hData = DdePoke(__TEXT("part"), "30");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(�_�~�[)
	hData = DdePoke(__TEXT("newcontrol"), "text,,�O�擾�s��");
	hData = DdePoke(__TEXT("part"), "10");
	hData = DdePoke(__TEXT("newcontrol"), "edit,prev,1");
	hData = DdePoke(__TEXT("default"), "2"); // IME �� OFF
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(�_�~�[)
	hData = DdePoke(__TEXT("part"), "3");
	hData = DdePoke(__TEXT("newcontrol"), "text,,��擾�s��");
	hData = DdePoke(__TEXT("part"), "10");
	hData = DdePoke(__TEXT("newcontrol"), "edit,next,1");
	hData = DdePoke(__TEXT("default"), "2"); // IME �� OFF
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(�_�~�[)

	hData = DdePoke(__TEXT("newcontrol"), "radio,search_option,�������@");
	hData = DdePoke(__TEXT("item"), "�ʁ@��");
	hData = DdePoke(__TEXT("item"), "���K�\��");
	hData = DdePoke(__TEXT("item"), "�����܂��i���K�\�����܂ށj����");
	hData = DdePoke(__TEXT("default"), "1");
	hData = DdePoke(__TEXT("notify"), "13");

	hData = DdePoke(__TEXT("newcontrol"), "radio,searchfile,�����Ώۃt�@�C��");
	hData = DdePoke(__TEXT("item"), "���̃t�@�C��");
	hData = DdePoke(__TEXT("item"), "���݊J���Ă���t�@�C���S��");
	hData = DdePoke(__TEXT("item"), "�t�H���_�[���̃t�@�C���S��");
	hData = DdePoke(__TEXT("item"), "�T�u�t�H���_�[���܂ރt�@�C���S��");
	hData = DdePoke(__TEXT("default"), "1");

	hData = DdePoke(__TEXT("newcontrol"), "text,,�t�H���_�̑I��");
	hData = DdePoke(__TEXT("newcontrol"), "combo,folder,");
	for (int i = 0; i < 11; i++) {
		char buf[2] = { 'A' + i, 0 };
		hData = DdePoke(__TEXT("item"), buf);
    }
	hData = DdePoke(__TEXT("part"), "34");
	hData = DdePoke(__TEXT("newcontrol"), "refdirbutton,folderref,");
	hData = DdePoke(__TEXT("notify"), "14");
	hData = DdePoke(__TEXT("item"), "�t�H���_��I��ŉ������B");
	hData = DdePoke(__TEXT("item"), "");
	hData = DdePoke(__TEXT("item"), "0");

	hData = DdePoke(__TEXT("newcontrol"), "text,,�t�@�C���I���i ; �ŋ�؂�j");
	hData = DdePoke(__TEXT("part"), "18");
	hData = DdePoke(__TEXT("newcontrol"), "edit,objectfile,");
	hData = DdePoke(__TEXT("default"), "2"); // IME �� OFF

	hData = DdePoke(__TEXT("newcolumn"), "40");

	hData = DdePoke(__TEXT("newcontrol"), "text,,��؂��");
	hData = DdePoke(__TEXT("newcontrol"), "list,sep,");
	hData = DdePoke(__TEXT("item"), "-");
	hData = DdePoke(__TEXT("item"), "--");
	hData = DdePoke(__TEXT("item"), "---");
	hData = DdePoke(__TEXT("item"), "��@�s");
	hData = DdePoke(__TEXT("height"), "4");
	hData = DdePoke(__TEXT("default"), "1");

	hData = DdePoke(__TEXT("newcontrol"), "group,,�X�L�b�v�i�s�n��j����");
	hData = DdePoke(__TEXT("item"), "page1"); // ���ɕ\������q�_�C�A���O�̖��O���w��
	hData = DdePoke(__TEXT("newcontrol"), "group,,�t�B�[���h����");
	hData = DdePoke(__TEXT("item"), "page2");

	hData = DdePoke(__TEXT("newcontrol"), "check,color,���ʂ̋����\��");
	hData = DdePoke(__TEXT("default"), "0");
	hData = DdePoke(__TEXT("part"), "17");

	hData = DdePoke(__TEXT("newcontrol"), "check,numbering,�o�̓f�[�^�ɔԍ�������");
	hData = DdePoke(__TEXT("default"), "0");

	hData = DdePoke(__TEXT("newcontrol"), "check,stealth,�r���o�߂̕\��");
	hData = DdePoke(__TEXT("default"), "1");
	hData = DdePoke(__TEXT("part"), "17");

	hData = DdePoke(__TEXT("newcontrol"), "check,checkline,�o�̓f�[�^�̃`�F�b�N");
	hData = DdePoke(__TEXT("default"), "0");

	hData = DdePoke(__TEXT("newcontrol"), "okcancel");

	return TRUE;
}

LRESULT CDdeClientTestAppDlg::OnShowDialog(WPARAM wParam, LPARAM lParam)
{
	CreateDengakuDialog();

	char cmdbuf[32];
	sprintf_s(cmdbuf, sizeof(cmdbuf), "dialog %d", (int)this->GetSafeHwnd());
	HDDEDATA hRet = DdeExecute(cmdbuf);

	if (!DdeControlAdviseLoop(TRUE)) {
		return 0;
	}

	return (LRESULT)hRet;
}

LRESULT CDdeClientTestAppDlg::OnEndDialog(WPARAM wParam, LPARAM lParam)
{
	DdeControlAdviseLoop(FALSE);

	HDDEDATA hRet = DdeExecute("enddialog");
//	ASSERT(hRet == (HDDEDATA)DDE_FACK);

	return 0;
}

void CDdeClientTestAppDlg::OnBnClickedOk()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	UninitializeDde();

	OnOK();
}

void CDdeClientTestAppDlg::OnBnClickedShowdialog()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	PostMessage(WM_USER_SHOWDIALOG);
}
