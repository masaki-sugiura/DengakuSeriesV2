// DdeClientTestAppDlg.cpp : 実装ファイル
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


// CDdeClientTestAppDlg ダイアログ

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


// CDdeClientTestAppDlg メッセージ ハンドラ

BOOL CDdeClientTestAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	if (InitializeDde()) {
//		PostMessage(WM_USER_SHOWDIALOG);
	}

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CDdeClientTestAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
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
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UninitializeDde();

	OnOK();
}

void CDdeClientTestAppDlg::OnBnClickedShowdialog()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	PostMessage(WM_USER_SHOWDIALOG);
}
