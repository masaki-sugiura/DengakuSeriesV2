// $Id: ddeserv.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	ddeserv.h
 *	DDE サーバ本体のクラス定義
 */

#ifndef DENGAKUSERVER_CLASSES_DDESERVER
#define DENGAKUSERVER_CLASSES_DDESERVER

#include "session.h"
#include "resource.h"
#include "ddeobjs.h"
#include "convdata.h"
#include "cmdname.h"
#include "ctrlname.h"
#include "file.h"
#include "common.h"

class DDEServerInfo;
class CmdLineParser;

//	こういうことはちゃんとやっといて欲しい :-< ＞Ｍ＄
//	HDDE_FALSE, HDDE_NULL, HDDE_FNOTPROCESSED (= 0) は
//	なくても文句は言われないけど…(^^;
#define	HDDE_TRUE			(reinterpret_cast<HDDEDATA>(TRUE))
#define	HDDE_FALSE			(reinterpret_cast<HDDEDATA>(FALSE))
#define	HDDE_FACK			(reinterpret_cast<HDDEDATA>(DDE_FACK))
#define	HDDE_FNOTPROCESSED	(reinterpret_cast<HDDEDATA>(DDE_FNOTPROCESSED))
#define	HDDE_NULL			(static_cast<HDDEDATA>(NULL))

//	メッセージハンドラのパラメータリスト
#define MH_ARGS (const HCONV, const HSZ, const HSZ, const HDDEDATA)

/*
	DDE サーバの実体となるクラス

	DdeServer クラスはクラスメンバとクラスメソッドのみから成る。
*/
class DdeServer {
public:
	//	サーバの初期化/終了、開始/停止
	static BOOL init(DDEServerInfo&); // DDE サーバの初期化
	static BOOL uninit(); // DDE サーバの終了処理

	//	状態取得メソッド
	static BOOL isActive() { return m_bActive; }
	static BOOL isStrictEmulation() { return m_bStrictEmulation; }

	//	トピック名がサポートされたものかどうか
	static BOOL isSupportedTopic(const HSZ);

	//	メンバ値取得メソッド
	static DWORD getddeInst() { return m_ddeInst; }
	static HINSTANCE gethInst() { return m_hInstance; }
	static HWND gethwndDlg() { return m_hwndDlg; }
	static const DdeString& getServiceName()
	{
		return *m_pdsServiceName;
	}

	//	ConvData クラスを扱うメソッド
	static int getConvNum()
	{
		return m_ConvDataList.itemNum();
	}
	static int addConvData(const HCONV hconv, const HSZ hszTn);
	static void deleteConvData(ConvData* pCd = NULL);
	static ConvData* getConvData(const HCONV);
	static ConvData* isStoredCD(const HSZ);

	static void notifyPostAdvise(ConvData* pCd);

	//	DdemlCallback 関数のメッセージハンドラの実体(ログ無効)
	static HDDEDATA regist MH_ARGS;
	static HDDEDATA unregist MH_ARGS;
	static HDDEDATA connect MH_ARGS;
	static HDDEDATA connectconfirm MH_ARGS;
	static HDDEDATA error MH_ARGS;
	static HDDEDATA disconnect MH_ARGS;
	static HDDEDATA execute MH_ARGS;
	static HDDEDATA request MH_ARGS;
	static HDDEDATA poke MH_ARGS;
	static HDDEDATA advstart MH_ARGS;
	static HDDEDATA advstop MH_ARGS;
	static HDDEDATA advreq MH_ARGS;

	//	DdemlCallback 関数のメッセージハンドラの実体(ログ有効)
	static HDDEDATA lregist MH_ARGS;
	static HDDEDATA lunregist MH_ARGS;
	static HDDEDATA lconnect MH_ARGS;
	static HDDEDATA lconnectconfirm MH_ARGS;
	static HDDEDATA lerror MH_ARGS;
	static HDDEDATA ldisconnect MH_ARGS;
	static HDDEDATA lexecute MH_ARGS;
	static HDDEDATA lrequest MH_ARGS;
	static HDDEDATA lpoke MH_ARGS;
	static HDDEDATA ladvstart MH_ARGS;
	static HDDEDATA ladvstop MH_ARGS;
	static HDDEDATA ladvreq MH_ARGS;

	static BOOL isLogging(); // ログが有効かどうか

private:
	static BOOL m_bActive; // サーバが動作中かどうか
	static BOOL m_bStrictEmulation; // マクロサーバ互換モードかどうか
	static HINSTANCE m_hInstance; // モジュールインスタンスハンドル
	static DWORD m_ddeInst; // DDE サーバのインスタンスハンドル
	static HWND m_hwndDlg; // メインウィンドウのハンドル
	static Auto_Ptr<DdeString> m_pdsServiceName; // 基本サービス名
	static const StringBuffer m_sbTopicNames; // トピック名のリスト
	static LinkList<ConvData> m_ConvDataList; // DDE 通信データのリスト
	static Auto_Ptr<File> m_pfLogFile; // ログファイル
	static Auto_Ptr<StringBuffer> m_psbLogFileName; // ログファイル名

	//	コマンド名->関数アドレスを取得するメソッド
	static PFNDDECMD_EXECUTE getExecCmd(const StringBuffer& name);
	static PFNDDECMD_REQUEST getReqCmd(const StringBuffer& name);
	static PFNDDECMD_POKE getPokeCmd(const StringBuffer& name);

	//	ログファイル関連のメソッド
	static BOOL startLog(const StringBuffer*); // ログファイルの作成
	static void stopLog(); // ログファイルを閉じる
	static void writeLog(LPCSTR); // ログにデータを書き込む
	//	特定の通信の結果をログに書き込む
	static void writeLog(const HCONV, LPCSTR);
	static void writeArgvToLog(const HCONV, CmdLineParser&, StringBuffer&);
};

#endif

