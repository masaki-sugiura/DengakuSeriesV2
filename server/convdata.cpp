// $Id: convdata.cpp,v 1.7 2007-04-29 16:11:35 sugiura Exp $
/*
 *	convdata.cpp
 *	ConvData クラスのコア部分の実装
 */

#include "misc.h"
#include "convdata.h"
#include "ddeserv.h"
#include "cmdline.h"
#include "common.h"

ConvData::ConvData(
	HINSTANCE hInstance,
	DWORD ddeInst,
	HCONV hconv,
	const StringBuffer &strTopicName)
	:	SessionInstance(hInstance,FALSE), // 基底クラス
		m_ddeInst(ddeInst), // DDE サーバのインスタンスハンドル
		m_hConv(hconv), // 通信ハンドル
		m_sbKeepName(nullStr), // KeepAlive 時の識別名
		m_dsAdvTopicName(ddeInst,strTopicName), // トピック名
		m_dsAdvItemName(ddeInst,"dlgresult"), // アドバイスループアイテム名
		m_dwLastError(0L), // DdeExecute() の LastError
		m_pComDlgThread(NULL) // コモンダイアログのスレッドへのポインタ
{
	// アドバイスデータへのアクセスの同期オブジェクト
	m_hAdvEvent	= ::CreateEvent(NULL,TRUE,FALSE,NULL);
}

ConvData::~ConvData()
{
	//	後始末
	::CloseHandle(m_hAdvEvent);
}

//	アドバイスループの開始
HDDEDATA
ConvData::ddeAdvStart(HSZ hsz1, HSZ hsz2)
{
	DEBUG_OUTPUT(("Enter ddeAdvStart()"));
	if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
		m_dsAdvItemName.compareTo(hsz2)  != 0) {
		DdeString ds1(m_ddeInst, hsz1), ds2(m_ddeInst, hsz2);
		DEBUG_OUTPUT(("topic = %s, item = %s", (LPCSTR)ds1, (LPCSTR)ds2));
		return HDDE_FALSE;
	}
	::SetEvent(m_hAdvEvent); // setAdvData() の呼出しを許可
	DEBUG_OUTPUT(("Leave ddeAdvStart()"));
	return HDDE_TRUE;
}

//	アドバイスループの停止
HDDEDATA
ConvData::ddeAdvStop(HSZ hsz1, HSZ hsz2)
{
	DEBUG_OUTPUT(("Enter ddeAdvStop()"));
	if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
		m_dsAdvItemName.compareTo(hsz2)  != 0) return HDDE_FALSE;
	::ResetEvent(m_hAdvEvent); // setAdvData() の呼出しを禁止
	DEBUG_OUTPUT(("Leave ddeAdvStop()"));
	return HDDE_TRUE;
}

//	アドバイスデータのセット
void
ConvData::setNotify(const StringBuffer& data)
{
	//	ddeAdvStart() が呼ばれるまで待機
	::WaitForSingleObject(m_hAdvEvent,INFINITE);
	try {
		//	アドバイスデータのセット
		this->SessionInstance::setNotify(data);
		//	データがセットされたことをクライアントに通知
		//	引数の後ろ２つは DdeString::operator HSZ() を呼んでいる
		::PostMessage(DdeServer::gethwndDlg(),WM_USER_NOTIFYPOSTADV,
						0,(LPARAM)this);
	} catch (...) {
		// nothing to do
	}
}

//	アドバイスデータをクライアントに渡す
HDDEDATA
ConvData::ddeAdvReq(HSZ hsz1, HSZ hsz2)
{
	try {
		if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
			m_dsAdvItemName.compareTo(hsz2)  != 0) return HDDE_NULL;
		//	通知コードの取得
		StringBuffer buf;
		this->getNotify(buf,INFINITE);
		//	メニュースレッドの停止…ここしかタイミングがないのかなぁ…？？
		if (m_pMenuThread.ptr() != NULL) this->stopMenuThread();
		if (m_pComDlgThread.ptr() != NULL) {
			m_pComDlgThread->stop();
			m_pComDlgThread = NULL;
		}
		//	クライアントに渡す DDE データを作成
		//	SendingDdeData は破棄時に DdeFreeDataHandle() を呼ばない
		//	(からローカルオブジェクトでもよい)
		DEBUG_OUTPUT(("Enter Create DDE Data"));
		HDDEDATA hData = SendingDdeData(m_ddeInst,
							m_dsAdvItemName.getHandle(),
							buf).getHandle();
		DEBUG_OUTPUT(("Leave Create DDE Data : hData = %08x", hData));

		return hData;
	} catch (...) {
		return HDDE_NULL;
	}
}

//	DDE Execute コマンドの実行
HDDEDATA
ConvData::ddeExecute(PFNDDECMD_EXECUTE pfnCmd, CmdLineParser& argv)
{
	try {
		m_dwLastError = 0L;
		::SetLastError(m_dwLastError);
		HDDEDATA ret = (this->*pfnCmd)(argv) ? HDDE_FACK : HDDE_FNOTPROCESSED;
		m_dwLastError = ::GetLastError();

		return ret;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

//	DDE Request コマンドの実行
HDDEDATA
ConvData::ddeRequest(
	PFNDDECMD_REQUEST pfnCmd,
	CmdLineParser& argv,
	const DdeString& dsItem)
{
	try {
		//	SendingDdeData は破棄時に DdeFreeDataHandle() を呼ばない
		//	(からローカルオブジェクトでもよい)
		return SendingDdeData(m_ddeInst,
							dsItem.getHandle(),
							(this->*pfnCmd)(argv)).getHandle();
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

//	DDE Poke コマンドの実行
HDDEDATA
ConvData::ddePoke(PFNDDECMD_POKE pfnCmd, CmdLineParser& argv)
{
	try {
		return (this->*pfnCmd)(argv) ? HDDE_FACK : HDDE_FNOTPROCESSED;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

