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
			// スキップ検索のチェックが外された
			hData = DdeExecute("enablectrl 0 skip");
        } else if (strReturn == __TEXT("1")) {
			// スキップ検索がチェックされた
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
			// field がチェックされた
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
			// 通常 がチェックされた
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

	// グループボックスで囲まれる部分(子ダイアログ)
	hData = DdePoke(__TEXT("newdlgpage"), "page1,38");
	hData = DdePoke(__TEXT("newcontrol"), "check,skipsearch,スキップ検索をする(TXK)");
	hData = DdePoke(__TEXT("default"), "0");
	hData = DdePoke(__TEXT("notify"), "11");
	hData = DdePoke(__TEXT("newcontrol"), "text,,");	//(ダミー)
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,スキップ文字・記号");
	hData = DdePoke(__TEXT("part"), "16");
	hData = DdePoke(__TEXT("newcontrol"), "combo,skip,");
	for (int i = 0; i < 6; i++) {
		char buf[2] = { 'A' + i, 0 };
		hData = DdePoke(__TEXT("item"), buf);
    }
	hData = DdePoke(__TEXT("part"), "16");
	hData = DdePoke(__TEXT("newcontrol"), "text,,");	//(ダミー)

	hData = DdePoke(__TEXT("newdlgpage"), "page2,38");
	hData = DdePoke(__TEXT("newcontrol"), "check,field,行の検索フィールドを指定する(app)");
	hData = DdePoke(__TEXT("default"), "0");
	hData = DdePoke(__TEXT("notify"), "12");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(ダミー)
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,区切り記号");
	hData = DdePoke(__TEXT("part"), "10");
	hData = DdePoke(__TEXT("newcontrol"), "edit,mark,\",\"");
	hData = DdePoke(__TEXT("imestate"), "2"); // IME を OFF
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(ダミー)
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(ダミー)
	hData = DdePoke(__TEXT("part"), "6");
	hData = DdePoke(__TEXT("newcontrol"), "text,,不検索範囲の行頭からの記号数");
	hData = DdePoke(__TEXT("part"), "25");
	hData = DdePoke(__TEXT("newcontrol"), "edit,number,0");
	hData = DdePoke(__TEXT("imestate"), "2"); // IME を OFF
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(ダミー)

	// 親ダイアログ
	hData = DdePoke(__TEXT("newdialog"), "40,§ 前後行表示・２行渡り検索・検索語強調 §     Ver.3.20");
	hData = DdePoke(__TEXT("newcontrol"), "text,,検索語");
	hData = DdePoke(__TEXT("part"), "7");
	hData = DdePoke(__TEXT("newcontrol"), "combo,search,");
	for (int i = 0; i < 21; i++) {
		char buf[2] = { 'A' + i, 0 };
		hData = DdePoke(__TEXT("item"), buf);
    }
	hData = DdePoke(__TEXT("part"), "30");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(ダミー)
	hData = DdePoke(__TEXT("newcontrol"), "text,,前取得行数");
	hData = DdePoke(__TEXT("part"), "10");
	hData = DdePoke(__TEXT("newcontrol"), "edit,prev,1");
	hData = DdePoke(__TEXT("default"), "2"); // IME を OFF
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(ダミー)
	hData = DdePoke(__TEXT("part"), "3");
	hData = DdePoke(__TEXT("newcontrol"), "text,,後取得行数");
	hData = DdePoke(__TEXT("part"), "10");
	hData = DdePoke(__TEXT("newcontrol"), "edit,next,1");
	hData = DdePoke(__TEXT("default"), "2"); // IME を OFF
	hData = DdePoke(__TEXT("part"), "4");
	hData = DdePoke(__TEXT("newcontrol"), "text,,"); //(ダミー)

	hData = DdePoke(__TEXT("newcontrol"), "radio,search_option,検索方法");
	hData = DdePoke(__TEXT("item"), "通　常");
	hData = DdePoke(__TEXT("item"), "正規表現");
	hData = DdePoke(__TEXT("item"), "あいまい（正規表現を含む）検索");
	hData = DdePoke(__TEXT("default"), "1");
	hData = DdePoke(__TEXT("notify"), "13");

	hData = DdePoke(__TEXT("newcontrol"), "radio,searchfile,検索対象ファイル");
	hData = DdePoke(__TEXT("item"), "このファイル");
	hData = DdePoke(__TEXT("item"), "現在開いているファイル全部");
	hData = DdePoke(__TEXT("item"), "フォルダー内のファイル全部");
	hData = DdePoke(__TEXT("item"), "サブフォルダーを含むファイル全部");
	hData = DdePoke(__TEXT("default"), "1");

	hData = DdePoke(__TEXT("newcontrol"), "text,,フォルダの選択");
	hData = DdePoke(__TEXT("newcontrol"), "combo,folder,");
	for (int i = 0; i < 11; i++) {
		char buf[2] = { 'A' + i, 0 };
		hData = DdePoke(__TEXT("item"), buf);
    }
	hData = DdePoke(__TEXT("part"), "34");
	hData = DdePoke(__TEXT("newcontrol"), "refdirbutton,folderref,");
	hData = DdePoke(__TEXT("notify"), "14");
	hData = DdePoke(__TEXT("item"), "フォルダを選んで下さい。");
	hData = DdePoke(__TEXT("item"), "");
	hData = DdePoke(__TEXT("item"), "0");

	hData = DdePoke(__TEXT("newcontrol"), "text,,ファイル選択（ ; で区切る）");
	hData = DdePoke(__TEXT("part"), "18");
	hData = DdePoke(__TEXT("newcontrol"), "edit,objectfile,");
	hData = DdePoke(__TEXT("default"), "2"); // IME を OFF

	hData = DdePoke(__TEXT("newcolumn"), "40");

	hData = DdePoke(__TEXT("newcontrol"), "text,,区切り線");
	hData = DdePoke(__TEXT("newcontrol"), "list,sep,");
	hData = DdePoke(__TEXT("item"), "-");
	hData = DdePoke(__TEXT("item"), "--");
	hData = DdePoke(__TEXT("item"), "---");
	hData = DdePoke(__TEXT("item"), "空　行");
	hData = DdePoke(__TEXT("height"), "4");
	hData = DdePoke(__TEXT("default"), "1");

	hData = DdePoke(__TEXT("newcontrol"), "group,,スキップ（行渡り）検索");
	hData = DdePoke(__TEXT("item"), "page1"); // 中に表示する子ダイアログの名前を指定
	hData = DdePoke(__TEXT("newcontrol"), "group,,フィールド検索");
	hData = DdePoke(__TEXT("item"), "page2");

	hData = DdePoke(__TEXT("newcontrol"), "check,color,結果の強調表示");
	hData = DdePoke(__TEXT("default"), "0");
	hData = DdePoke(__TEXT("part"), "17");

	hData = DdePoke(__TEXT("newcontrol"), "check,numbering,出力データに番号をつける");
	hData = DdePoke(__TEXT("default"), "0");

	hData = DdePoke(__TEXT("newcontrol"), "check,stealth,途中経過の表示");
	hData = DdePoke(__TEXT("default"), "1");
	hData = DdePoke(__TEXT("part"), "17");

	hData = DdePoke(__TEXT("newcontrol"), "check,checkline,出力データのチェック");
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
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UninitializeDde();

	OnOK();
}

void CDdeClientTestAppDlg::OnBnClickedShowdialog()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	PostMessage(WM_USER_SHOWDIALOG);
}
