// $Id: ddeserv.cpp,v 1.2 2002-02-19 15:34:22 sugiura Exp $
/*
 *	ddeserv.cpp
 *	DdeServer クラスの実装
 */

#include "common.h"
#include "ddeserv.h"
#include "ddesinfo.h"
#include "ddeobjs.h"
#include "cmdline.h"
#include "pdparser.h"

//	クラスメンバの初期化
BOOL DdeServer::m_bActive;
BOOL DdeServer::m_bStrictEmulation;
HINSTANCE DdeServer::m_hInstance;
HWND DdeServer::m_hwndDlg;
DWORD DdeServer::m_ddeInst;
Auto_Ptr<File> DdeServer::m_pfLogFile(NULL);
Auto_Ptr<StringBuffer> DdeServer::m_psbLogFileName(NULL);
Auto_Ptr<DdeString> DdeServer::m_pdsServiceName(NULL);
LinkList<ConvData> DdeServer::m_ConvDataList;
const StringBuffer
DdeServer::m_sbTopicNames(GetServerString(SVR_STR_TOPICNAMES));

//	Ddeml ライブラリが使用する DDE 通信を処理するコールバック関数
HDDEDATA CALLBACK
ddeCallBack(UINT uType, UINT uFmt, HCONV hconv,
			HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
			DWORD dwData1, DWORD dwData2)
{
	switch (uType) {
	case XTYP_REGISTER: // DDE サーバの開始
		return DdeServer::regist(hconv,hsz1,hsz2,hdata);

	case XTYP_UNREGISTER: // DDE サーバの終了
		return DdeServer::unregist(hconv,hsz1,hsz2,hdata);

	case XTYP_CONNECT: // DDE クライアントからの接続要求
		return DdeServer::connect(hconv,hsz1,hsz2,hdata);

	case XTYP_CONNECT_CONFIRM: // DDE 通信の確立
		//	通信データリストに登録
		return DdeServer::connectconfirm(hconv,hsz1,hsz2,hdata);

	case XTYP_ERROR: // DDE 通信エラー
		return DdeServer::error(hconv,hsz1,hsz2,hdata);

	case XTYP_DISCONNECT: // 特定の DDE 通信の終了
		return DdeServer::disconnect(hconv,hsz1,hsz2,hdata);

	case XTYP_EXECUTE: // DDE Execute コマンドの実行
		return DdeServer::execute(hconv,hsz1,hsz2,hdata);

	case XTYP_REQUEST: // DDE Request コマンドの実行
		return DdeServer::request(hconv,hsz1,hsz2,hdata);

	case XTYP_POKE: // DDE Poke コマンドの実行
		return DdeServer::poke(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVSTART: // Advice Loop の開始
		return DdeServer::advstart(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVSTOP: // Advice Loop の終了
		return DdeServer::advstop(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVREQ: // Advice Data の取得要求
		return DdeServer::advreq(hconv,hsz1,hsz2,hdata);

	default:
		break;
	}
	return HDDE_NULL;
}

//	Ddeml ライブラリが使用する DDE 通信を処理するコールバック関数(ログあり)
HDDEDATA CALLBACK
ddeCallBack_with_Log(
	UINT uType,
	UINT uFmt,
	HCONV hconv,
	HSZ hsz1,
	HSZ hsz2,
	HDDEDATA hdata,
	DWORD dwData1,
	DWORD dwData2)
{
	switch (uType) {
	case XTYP_REGISTER: // DDE サーバの開始
		return DdeServer::lregist(hconv,hsz1,hsz2,hdata);

	case XTYP_UNREGISTER: // DDE サーバの終了
		return DdeServer::lunregist(hconv,hsz1,hsz2,hdata);

	case XTYP_CONNECT: // DDE クライアントからの接続要求
		return DdeServer::lconnect(hconv,hsz1,hsz2,hdata);

	case XTYP_CONNECT_CONFIRM: // DDE 通信の確立
		//	通信データリストに登録
		return DdeServer::lconnectconfirm(hconv,hsz1,hsz2,hdata);

	case XTYP_ERROR: // DDE 通信エラー
		return DdeServer::lerror(hconv,hsz1,hsz2,hdata);

	case XTYP_DISCONNECT: // 特定の DDE 通信の終了
		return DdeServer::ldisconnect(hconv,hsz1,hsz2,hdata);

	case XTYP_EXECUTE: // DDE Execute コマンドの実行
		return DdeServer::lexecute(hconv,hsz1,hsz2,hdata);

	case XTYP_REQUEST: // DDE Request コマンドの実行
		return DdeServer::lrequest(hconv,hsz1,hsz2,hdata);

	case XTYP_POKE: // DDE Poke コマンドの実行
		return DdeServer::lpoke(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVSTART: // Advice Loop の開始
		return DdeServer::ladvstart(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVSTOP: // Advice Loop の終了
		return DdeServer::ladvstop(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVREQ: // Advice Data の取得要求
		return DdeServer::ladvreq(hconv,hsz1,hsz2,hdata);

	default:
		break;
	}
	return HDDE_NULL;
}

//	DDE サーバの初期化
BOOL
DdeServer::init(DDEServerInfo& dsi)
{
	if (m_ddeInst != 0) return m_bActive;

	//	その他のメンバの設定
	m_hInstance = dsi.m_hInst;
	m_hwndDlg = dsi.m_hwndMain;
	m_bStrictEmulation = dsi.m_bStrictEmulation;

	//	Ddeml ライブラリに ddeCallBack 関数を登録
	if (startLog(dsi.m_psbServerLogFileName)) {
		//	ログあり
		if (::DdeInitialize(&m_ddeInst,ddeCallBack_with_Log,
							APPCLASS_STANDARD,0) != DMLERR_NO_ERROR) {
			stopLog();
			return FALSE;
		}
	} else {
		//	ログなし
		if (::DdeInitialize(&m_ddeInst,ddeCallBack,
							APPCLASS_STANDARD,0) != DMLERR_NO_ERROR) {
			return FALSE;
		}
	}

	try {
		m_pdsServiceName = new DdeString(m_ddeInst,dsi.m_sbServiceName);
	} catch (...) {
		return FALSE;
	}

	//	Ddeml ライブラリに基本サービス名を登録(DDE サーバの開始)
	m_bActive = (BOOL)::DdeNameService(m_ddeInst,
										m_pdsServiceName->getHandle(),
										0L,DNS_REGISTER);

	return m_bActive;
}

//	DDE サーバの終了
BOOL
DdeServer::uninit()
{
	if (m_ddeInst == 0) return TRUE;

	if (m_bActive) {
		try {
			//	アクティブな通信を全て破棄
			if (m_ConvDataList.itemNum() > 0)
				DdeServer::deleteConvData(NULL);
		} catch (...) {
			// nothing to do
		}

		//	Ddeml ライブラリから基本サービス名の登録を削除
		::DdeNameService(m_ddeInst,
						m_pdsServiceName->getHandle(),
						0L,DNS_UNREGISTER);
		m_bActive = FALSE;
	}

	//	Ddeml ライブラリからコールバック関数の登録を削除
	BOOL bRet = ::DdeUninitialize(m_ddeInst);
	m_ddeInst = 0;

	//	ログの停止
	stopLog();

	return bRet;
}

//	トピック名が正しいかどうか
BOOL
DdeServer::isSupportedTopic(const HSZ hszTopicName)
{
	return m_sbTopicNames.find(DdeString(m_ddeInst,hszTopicName)) >= 0;
}

ConvData*
DdeServer::isStoredCD(const HSZ topicname)
{
	try {
		DdeString dsTopicName(m_ddeInst,topicname);
		const StringBuffer& sbTn = (const StringBuffer&)dsTopicName;
		m_ConvDataList.initSequentialGet();
		ConvData* cd;
		while ((cd = m_ConvDataList.getNextItem()) != NULL) {
			if (sbTn.compareTo(cd->getKeepName()) == 0) return cd;
		}
	} catch (...) {
		// nothing to do
	}
	return NULL;
}

//	通信クラスの生成、登録
int
DdeServer::addConvData(const HCONV hconv, const HSZ hszTn)
{
	ConvData* pCd = isStoredCD(hszTn);
	int	ret = -1;
	if (pCd) {
//		if (pCd->gethConv()) {}	//	Share? or not share?
		pCd->sethConv(hconv);
		pCd->setAdvTopicName(hszTn);
		ret = m_ConvDataList.itemNum();
	} else {
		try {
			pCd = new ConvData(m_hInstance,
								m_ddeInst,
								hconv,
								DdeString(m_ddeInst,hszTn));
		} catch (...) {
			return -1;
		}
		ret = m_ConvDataList.addItem(pCd);
	}
	//	getConvData() のため、通信データにポインタを登録
	::DdeSetUserHandle(hconv,QID_SYNC,reinterpret_cast<DWORD>(pCd));
	return ret;
}

static void
DelCD(ConvData* pCd, LinkList<ConvData>& cdlist)
{
	pCd->setKeepName(nullStr);
	HCONV hconv = pCd->gethConv();
	if (hconv != NULL) {
		if (DdeServer::isLogging())
			DdeServer::ldisconnect(hconv,NULL,NULL,HDDE_NULL);
		else
			DdeServer::disconnect(hconv,NULL,NULL,HDDE_NULL);
		::DdeDisconnect(hconv);
	} else {
		cdlist.delItemByPtr(pCd);
	}
}

//	サーバ側からの通信の終了
//	disconnect() を呼ぶのは必要ならログを取るため
void
DdeServer::deleteConvData(ConvData* pCd)
{
	if (m_ConvDataList.itemNum() <= 0) return;
	try {
		if (pCd != NULL) {
			//	特定の通信を終了
			DelCD(pCd,m_ConvDataList);
		} else {
			//	全通信を終了
			//	onDisconnect() は内部で m_pConvDataList->delItemByIndex(0) を
			//	呼んでいるので initSequentialGet() & getNextItem() のペアを
			//	使わずに一つ一つ処理する必要がある。
			while (m_ConvDataList.itemNum() > 0) {
				DelCD(m_ConvDataList.getItemByIndex(0),m_ConvDataList);
			}
		}
	} catch (...) {
		// nothing to do
	}
}

//	HCONV -> ConvData*
ConvData*
DdeServer::getConvData(const HCONV hconv)
{
	if (hconv == NULL) return NULL;
	//	ユーザーデータにポインタが登録されている (see addConvData())
	CONVINFO ci;
	ci.cb = sizeof(CONVINFO);
	ci.hUser = 0L;
	::DdeQueryConvInfo(hconv,QID_SYNC,&ci);
	return reinterpret_cast<ConvData*>(ci.hUser);
}

//	サーバ本体のプロセスから DdeAdvReq() を呼ぶためのメソッド
void
DdeServer::notifyPostAdvise(ConvData* pCd)
{
	//	時間が惜しいので最低限のチェックしかしない
	if (pCd != NULL) {
		::DdePostAdvise(m_ddeInst,
						pCd->getAdvTopicName(),
						pCd->getAdvItemName());
	}
}

//	ログの開始
BOOL
DdeServer::startLog(const StringBuffer *psbLogFileName)
{
	if (psbLogFileName == NULL) return FALSE;
	try {
		m_psbLogFileName = new StringBuffer(*psbLogFileName);
	} catch (...) {
		return FALSE;
	}
	m_pfLogFile = new File(*m_psbLogFileName,
							GENERIC_WRITE,
							OPEN_ALWAYS,
							FILE_FLAG_SEQUENTIAL_SCAN);
	if (!m_pfLogFile->isValid()) return FALSE;
	m_pfLogFile->setFilePointer(0,0,FILE_END);
	return TRUE;
}

//	ログの停止
void
DdeServer::stopLog()
{
	if (isLogging()) {
		m_pfLogFile->flushFileBuffers();
		m_pfLogFile = NULL;
		m_psbLogFileName = NULL;
	}
}

BOOL
DdeServer::isLogging()
{
	return m_pfLogFile.ptr() != NULL && m_pfLogFile->isValid();
}

//	ログファイルへの書き込み
void
DdeServer::writeLog(LPCSTR logdata)
{
	try {
		StringBuffer buf(40+lstrlen(logdata));
		buf.append((TCHAR)'[').append((ULONG)::GetTickCount()).append("] ")
			.append(logdata).append((TCHAR)'\n');
		if (m_pfLogFile->writeFile((UCHAR*)(LPCSTR)buf,buf.length())
			!= (DWORD)buf.length()) {
			DdeServer::stopLog();
		}
	} catch (...) {
		// nothing to do
	}
	//	これがあるとかなり速度が低下する(当たり前(^^;)
//	m_pfLogFile->flushFileBuffers();
}

//	ログファイルへの書き込み(特定の通信について)
void
DdeServer::writeLog(const HCONV hconv, LPCSTR logdata)
{
	try {
		StringBuffer buf("ConvID: ",-1,40+lstrlen(logdata));
		writeLog(buf.append((ULONG)hconv).append(" : ").append(logdata));
	} catch (...) {
	}
}

//	コマンド引数をログに書き込む
void
DdeServer::writeArgvToLog(
	const HCONV hconv,
	CmdLineParser& ha,
	StringBuffer& header)
{
	try {
		ha.initSequentialGet();
		if (header.length()) {
			writeLog(hconv,header.append(ha.getNextArgv()));
		}
		header.reset(" Argv: ");
		LPCSTR	pav;
		while ((pav = ha.getNextArgv()) != NULL) {
			writeLog(hconv,header.append(pav));
			header.setlength(7);
		}
	} catch (...) {
	}
}

//	DdemlCallBack() のメッセージハンドラ(ログ無効)
HDDEDATA
DdeServer::regist(const HCONV, const HSZ, const HSZ, const HDDEDATA)
{
	return HDDE_NULL; // Ddeml ignores it.
}

HDDEDATA
DdeServer::unregist(const HCONV, const HSZ, const HSZ, const HDDEDATA)
{
	return HDDE_NULL; // Ddeml ignores it.
}

//	通信要求
//	hsz1 : トピック名
//	hsz2 : 基本サービス名
HDDEDATA
DdeServer::connect(const HCONV, const HSZ hsz1, const HSZ hsz2, const HDDEDATA)
{
	if (m_pdsServiceName.ptr() == NULL ||
		m_pdsServiceName->compareTo(hsz2) != 0)
		return HDDE_FALSE;
	else if (isSupportedTopic(hsz1)) return HDDE_TRUE;
	return isStoredCD(hsz1) ? HDDE_TRUE : HDDE_FALSE;
}

//	通信の確立
//	hconv : 通信ハンドル
//	hsz1  : トピック名
HDDEDATA
DdeServer::connectconfirm(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ, // not used
	const HDDEDATA) // not used
{
	//	Ddeml ignores it.
	return addConvData(hconv,hsz1) > 0 ? HDDE_TRUE : HDDE_FALSE;
}

//	エラー
HDDEDATA
DdeServer::error(const HCONV, const HSZ, const HSZ, const HDDEDATA)
{
	::PostMessage(m_hwndDlg,WM_USER_DDE_STOP,0,0);
	return HDDE_NULL;	//	Ddeml ignores it.
}

//	通信の破棄
HDDEDATA
DdeServer::disconnect(const HCONV hconv, const HSZ, const HSZ, const HDDEDATA)
{
	try {
		//	リストから登録抹消
		ConvData* cd = getConvData(hconv);
		if (cd != NULL) {
			if (!cd->isKeepAlive())
				m_ConvDataList.delItemByPtr(cd);
			else
				cd->sethConv(NULL);
		}
	} catch (...) {
	}
	return HDDE_NULL;	//	Ddeml ignores it.
}

//	ddeexecute
HDDEDATA
DdeServer::execute(
	const HCONV hconv,
	const HSZ, // not used
	const HSZ, // not used
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		ReceivedDdeData rdd(hdata);
		RealCmdLineParser argv((const StringBuffer&)rdd);
		if (argv.itemNum() <= 0) return HDDE_FNOTPROCESSED;
		PFNDDECMD_EXECUTE pfnCmd = getExecCmd(argv.getArgvStr(0));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		argv.delArgv(0); // cmd name
		return cd->ddeExecute(pfnCmd,argv);
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::request(
	const HCONV hconv,
	const HSZ, // not used
	const HSZ hsz2,
	const HDDEDATA) // not used
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		DdeString dsItem(m_ddeInst,hsz2);
		RealCmdLineParser argv((const StringBuffer&)dsItem);
		if (argv.itemNum() <= 0) return HDDE_FNOTPROCESSED;
		PFNDDECMD_REQUEST pfnCmd = getReqCmd(argv.getArgvStr(0));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		argv.delArgv(0); // req name
		return cd->ddeRequest(pfnCmd,argv,dsItem);
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::poke(
	const HCONV hconv,
	const HSZ, // not used
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		PFNDDECMD_POKE pfnCmd = getPokeCmd(DdeString(m_ddeInst,hsz2));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		ReceivedDdeData rdd(hdata);
		PokeDataParser argv((const StringBuffer&)rdd);
		return cd->ddePoke(pfnCmd,argv);
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::advstart(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA) // not used
{
	ConvData* cd = getConvData(hconv);
	return cd != NULL ? cd->ddeAdvStart(hsz1,hsz2) : HDDE_FALSE;
}

HDDEDATA
DdeServer::advstop(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA) // not used
{
	ConvData* cd = getConvData(hconv);
	//	Ddeml ignores it.
	return cd != NULL ? cd->ddeAdvStop(hsz1,hsz2) : HDDE_FALSE;
}

HDDEDATA
DdeServer::advreq(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA) // not used
{
	ConvData* cd = getConvData(hconv);
	return cd != NULL ? cd->ddeAdvReq(hsz1,hsz2) : HDDE_NULL;
}

//	DdemlCallBack() のメッセージハンドラ(ログ有効)
HDDEDATA
DdeServer::lregist(
	const HCONV,
	const HSZ,
	const HSZ,
	const HDDEDATA)
{
	try {
		StringBuffer buf("Start Server : ",-1,32);
		writeLog(buf.append((LPCSTR)*m_pdsServiceName));
	} catch (...) {
	}
	return HDDE_NULL;	//	Ddeml ignores it.
}

HDDEDATA
DdeServer::lunregist(
	const HCONV,
	const HSZ,
	const HSZ,
	const HDDEDATA)
{
	try {
		writeLog("Stop Server");
	} catch (...) {
	}
	return HDDE_NULL;	//	Ddeml ignores it.
}

HDDEDATA
DdeServer::lconnect(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	return connect(hconv,hsz1,hsz2,hdata);
}

HDDEDATA
DdeServer::lconnectconfirm(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		HDDEDATA hret = connectconfirm(hconv,hsz1,hsz2,hdata);
		StringBuffer msg("Connect ",-1,32);
		msg.append(hret == HDDE_TRUE ? "Success" : "Failed");
		msg.append(": ").append((LPCSTR)DdeString(m_ddeInst,hsz1));
		writeLog(hconv,msg);
		return hret;	//	Ddeml ignores it.
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::lerror(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		writeLog("Unrecoverable error(s)!! Stop Server");
		return error(hconv,hsz1,hsz2,hdata);
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::ldisconnect(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		writeLog(hconv,"Disconnect");
		return disconnect(hconv,hsz1,hsz2,hdata);
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::lexecute(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		ReceivedDdeData rdd(hdata);
		RealCmdLineParser argv((const StringBuffer&)rdd);
		if (argv.itemNum() <= 0) return HDDE_FNOTPROCESSED;
		StringBuffer buf("Execute: ");
		writeArgvToLog(hconv,argv,buf);
		PFNDDECMD_EXECUTE pfnCmd = getExecCmd(argv.getArgvStr(0));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		argv.delArgv(0); // cmd name
		HDDEDATA hret = cd->ddeExecute(pfnCmd,argv);
		buf = hret == HDDE_FACK ? " Return: Success" : " Return: Failed";
		writeLog(hconv,buf);
		return hret;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::lrequest(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		DdeString dsItem(m_ddeInst,hsz2);
		RealCmdLineParser argv((const StringBuffer&)dsItem);
		if (argv.itemNum() <= 0) return HDDE_FNOTPROCESSED;
		StringBuffer buf("Request: ");
		writeArgvToLog(hconv,argv,buf);
		PFNDDECMD_REQUEST pfnCmd = getReqCmd(argv.getArgvStr(0));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		argv.delArgv(0); // req name
		HDDEDATA hret = cd->ddeRequest(pfnCmd,argv,dsItem);
		buf = " Return: ";
		if (hret != HDDE_FNOTPROCESSED) {
			buf.append((const StringBuffer&)TemporalDdeData(hret));
		} else {
			buf.append("Failed");
		}
		writeLog(hconv,buf);
		return hret;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::lpoke(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		DdeString dsItem(m_ddeInst,hsz2);
		PFNDDECMD_POKE pfnCmd = getPokeCmd(dsItem);
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		ReceivedDdeData rdd(hdata);
		PokeDataParser argv((const StringBuffer&)rdd);
		StringBuffer buf("Poke : ",-1,dsItem.length());
		writeLog(hconv,buf.append((LPCSTR)dsItem));
		buf = nullStr;
		writeArgvToLog(hconv,argv,buf);
		HDDEDATA hret = cd->ddePoke(pfnCmd,argv);
		buf = hret == HDDE_FACK ? " Return: Success" : " Return: Failed";
		writeLog(hconv,buf);
		return hret;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::ladvstart(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		HDDEDATA hret = advstart(hconv,hsz1,hsz2,hdata);
		writeLog(hconv,hret == HDDE_TRUE ? "Start Advice Success" :
										   "Start Advice Failed");
		return hret;
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::ladvstop(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		HDDEDATA hret = advstop(hconv,hsz1,hsz2,hdata);
		writeLog(hconv,hret == HDDE_TRUE ? "Stop Advice Success" :
										   "Stop Advice Ignored");
		return hret;
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::ladvreq(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		HDDEDATA hret = advreq(hconv,hsz1,hsz2,hdata);
		StringBuffer buf("Request Advice Data ",-1,16);
		if (hret != HDDE_NULL) {
			writeLog(hconv,buf.append("Success"));
			buf.reset(" Data: ");
			writeLog(hconv,buf.append((const StringBuffer&)TemporalDdeData(hret)));
		} else {
			writeLog(hconv,buf.append("Failed"));
			buf.reset(" Error code: ");
			writeLog(hconv,buf.append((ULONG)::DdeGetLastError(m_ddeInst)));
		}
		return hret;
	} catch (...) {
		return HDDE_NULL;
	}
}

