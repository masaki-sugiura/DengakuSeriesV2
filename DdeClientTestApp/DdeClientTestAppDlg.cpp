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

LRESULT CDdeClientTestAppDlg::OnShowDialog(WPARAM wParam, LPARAM lParam)
{
	HDDEDATA hRet = DdePoke(__TEXT("newdialog"), "40, TestDialog");
//	ASSERT(hRet == (HDDEDATA)DDE_FACK);

	hRet = DdePoke(__TEXT("newcontrol"), "okcancel");
//	ASSERT(hRet == (HDDEDATA)DDE_FACK);

	if (!DdeControlAdviseLoop(TRUE)) {
		return 1;
	}

	char cmdbuf[32];
	sprintf_s(cmdbuf, sizeof(cmdbuf), "dialog %d", (int)this->GetSafeHwnd());
	hRet = DdeExecute(cmdbuf);

	return (LRESULT)hRet;
}

LRESULT CDdeClientTestAppDlg::OnEndDialog(WPARAM wParam, LPARAM lParam)
{
	HDDEDATA hRet = DdeExecute("enddialog");
//	ASSERT(hRet == (HDDEDATA)DDE_FACK);

	DdeControlAdviseLoop(FALSE);

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
