// DdeClientTestAppDlg.h : ヘッダー ファイル
//

#pragma once

#include <ddeml.h>

#ifdef	_UNICODE
#define	CODE_PAGE	CP_WINUNICODE
#else
#define	CODE_PAGE	CP_WINANSI
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
class CDdeClientTestAppDlg : public CDialog
{
// コンストラクション
public:
	CDdeClientTestAppDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_DDECLIENTTESTAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON		m_hIcon;
	DWORD		m_dwIdInst;
	HCONV		m_hConv;
	static CDdeClientTestAppDlg* m_pThis;
	CList<CString>		m_lstNotifies;
	CEvent		m_evtNotify;
	CCriticalSection	m_csNotifyLock;
	DdeStringHandle*	m_pdshService;
	DdeStringHandle*	m_pdshTopic;

	BOOL InitializeDde();
	void UninitializeDde();

	BOOL DdeControlAdviseLoop(BOOL bStart);

	HDDEDATA DdeExecute(LPCSTR pszCommand);
	HDDEDATA DdeRequest(LPCSTR pszCommand, CString& strReturn);
	HDDEDATA DdePoke(LPCTSTR pszCommand, LPCSTR pszData);

	BOOL CreateDengakuDialog();

	void OnDdeAdvData(HDDEDATA hData);
	
	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LRESULT OnShowDialog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndDialog(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnDdeExecute(WPARAM, LPARAM);
	afx_msg LRESULT OnDdeRequest(WPARAM, LPARAM);
	afx_msg LRESULT OnDdePoke(WPARAM, LPARAM);
	afx_msg LRESULT OnDdeAdvice(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

	static HDDEDATA CALLBACK DdeClientCallback(UINT uType,
											   UINT uFmt,
											   HCONV hconv,
											   HSZ hsz1,
											   HSZ hsz2,
											   HDDEDATA hdata,
											   ULONG_PTR dwData1,
											   ULONG_PTR dwData2);

	static UINT WINAPIV MacroThreadProc(LPVOID pParam);

	UINT MacroThreadProcMain();

	void WaitNotify(CString& strNotify, DWORD dwTimeout);

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedShowdialog();
};
