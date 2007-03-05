// DdeClientTestAppDlg.h : ヘッダー ファイル
//

#pragma once

#include <ddeml.h>

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

	DECLARE_MESSAGE_MAP()

	static HDDEDATA CALLBACK DdeClientCallback(UINT uType,
											   UINT uFmt,
											   HCONV hconv,
											   HSZ hsz1,
											   HSZ hsz2,
											   HDDEDATA hdata,
											   ULONG_PTR dwData1,
											   ULONG_PTR dwData2);
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedShowdialog();
};
