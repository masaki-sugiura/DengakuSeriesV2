//	$Id: session.cpp,v 1.5 2002-02-17 08:00:41 sugiura Exp $
/*
 *	session.cpp
 *	SessionInstance クラスのコア部分の実装
 */

#include "session.h"

SessionInstance::SessionInstance(HINSTANCE hInstance, BOOL bTrueCD)
	:	m_hInstance(hInstance),	//	インスタンスハンドル
		m_dwLastError(0L), // DdeExecute() の LastError
		m_DirList(bTrueCD), // DirList クラス
		m_pSPIManager(NULL), // SPI_Manager クラスへのポインタ
		m_pEnvManager(NULL), // EnvManager クラスへのポインタ
		m_pFileOpResult(NULL), // ファイル操作系関数の結果を保持するクラスへのポインタ
		m_pEnumerator(NULL), // enum* の結果を保持するクラスへのポインタ
		m_pStrToken(NULL), // gettoken の結果を保持するクラスへのポインタ
		m_pMenuThread(NULL), // メニュー表示スレッドへのポインタ
		m_pDlgThread(NULL), // ダイアログ表示スレッドへのポインタ
		m_pBRegExp_Manager(NULL)
{
	m_DlgFrame.setSessionInstance(this);
}

SessionInstance::~SessionInstance()
{
	//	prevent thread stop before closing window
	if (m_pMenuThread.ptr() != NULL) this->stopMenuThread();
	if (m_pDlgThread.ptr() != NULL) this->si_enddialog(0); // no wait
}

