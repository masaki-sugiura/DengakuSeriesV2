// $Id: convdata.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	convdata.cpp
 *	ConvData クラスのコア部分の実装
 */

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
	if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
		m_dsAdvItemName.compareTo(hsz2)  != 0) return HDDE_FALSE;
	::SetEvent(m_hAdvEvent); // setAdvData() の呼出しを許可
	return HDDE_TRUE;
}

//	アドバイスループの停止
HDDEDATA
ConvData::ddeAdvStop(HSZ hsz1, HSZ hsz2)
{
	if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
		m_dsAdvItemName.compareTo(hsz2)  != 0) return HDDE_FALSE;
	::ResetEvent(m_hAdvEvent); // setAdvData() の呼出しを禁止
	return HDDE_TRUE;
}

//	アドバイスデータのセット
void
ConvData::setNotify(const StringBuffer& data)
{
	//	ddeAdvStart() が呼ばれるまで待機
	::WaitForSingleObject(m_hAdvEvent,INFINITE);
	//	アドバイスデータのセット
	this->SessionInstance::setNotify(data);
	//	データがセットされたことをクライアントに通知
	//	引数の後ろ２つは DdeString::operator HSZ() を呼んでいる
	::PostMessage(DdeServer::gethwndDlg(),WM_USER_NOTIFYPOSTADV,
					0,(LPARAM)this);
}

//	アドバイスデータをクライアントに渡す
HDDEDATA
ConvData::ddeAdvReq(HSZ hsz1, HSZ hsz2)
{
	if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
		m_dsAdvItemName.compareTo(hsz2)  != 0) return HDDE_NULL;
	//	通知コードの取得
	StringBuffer buf = nullStr;
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
	return SendingDdeData(m_ddeInst,
						m_dsAdvItemName.getHandle(),
						buf).getHandle();
}

//	DDE Execute コマンドの実行
HDDEDATA
ConvData::ddeExecute(PFNDDECMD_EXECUTE pfnCmd, CmdLineParser& argv)
{
	m_dwLastError = 0L;
	::SetLastError(m_dwLastError);
	HDDEDATA ret = (this->*pfnCmd)(argv) ? HDDE_FACK : HDDE_FNOTPROCESSED;
	m_dwLastError = ::GetLastError();

	return ret;
}

//	DDE Request コマンドの実行
HDDEDATA
ConvData::ddeRequest(
	PFNDDECMD_REQUEST pfnCmd,
	CmdLineParser& argv,
	const DdeString& dsItem)
{
	//	SendingDdeData は破棄時に DdeFreeDataHandle() を呼ばない
	//	(からローカルオブジェクトでもよい)
	return SendingDdeData(m_ddeInst,
						dsItem.getHandle(),
						(this->*pfnCmd)(argv)).getHandle();
}

//	DDE Poke コマンドの実行
HDDEDATA
ConvData::ddePoke(PFNDDECMD_POKE pfnCmd, CmdLineParser& argv)
{
	return (this->*pfnCmd)(argv) ? HDDE_FACK : HDDE_FNOTPROCESSED;
}

