// $Id: convdata.h,v 1.2 2002-02-20 16:48:40 sugiura Exp $
/*
 *	convdata.h
 *	DDE 通信毎に用意するリソース管理とコマンド処理を行うクラスの定義
 */

#ifndef DENGAKUSERVER_CLASSES_CONVDATA
#define DENGAKUSERVER_CLASSES_CONVDATA

#include "session.h"
#include "ddeobjs.h"
#include "opencomdlg.h"

class ConvData;
class CmdLineParser;

//	DDE コマンドを処理する ConvData::On_??() へのポインタ
typedef	int (ConvData::*PFNDDECMD_EXECUTE)(CmdLineParser&);
typedef StringBuffer (ConvData::*PFNDDECMD_REQUEST)(CmdLineParser&);
typedef PFNDDECMD_EXECUTE PFNDDECMD_POKE;

// 個々の DDE 通信を処理するクラス
class ConvData : public SessionInstance {
public:
	ConvData(HINSTANCE, DWORD, HCONV, const StringBuffer&);
	~ConvData();

	//	メンバ取得メソッド
	HCONV gethConv() const { return m_hConv; }
	void sethConv(HCONV hconv) { m_hConv = hconv; }

	const DdeString& getAdvTopicName() const { return m_dsAdvTopicName; }
	const DdeString& setAdvTopicName(HSZ hszTopicName)
	{
		return (m_dsAdvTopicName = DdeString(m_ddeInst,hszTopicName));
	}
	const DdeString& getAdvItemName() const { return m_dsAdvItemName; }

	//	オブジェクトの寿命の制御
	BOOL isKeepAlive() const { return m_sbKeepName.length() > 0; }
	const StringBuffer&	getKeepName() const { return m_sbKeepName; }
	void setKeepName(const StringBuffer &kn) { m_sbKeepName = kn; }

	//	アドバイスループ処理
	HDDEDATA ddeAdvStart(HSZ, HSZ);
	HDDEDATA ddeAdvStop(HSZ, HSZ);
	HDDEDATA ddeAdvReq(HSZ, HSZ);

	void setNotify(const StringBuffer&);

	//	コマンド実行
	HDDEDATA ddeExecute(PFNDDECMD_EXECUTE, CmdLineParser&);
	HDDEDATA ddeRequest(PFNDDECMD_REQUEST, CmdLineParser&, const DdeString&);
	HDDEDATA ddePoke(PFNDDECMD_POKE, CmdLineParser&);

	//	コマンドハンドラ
#define	DECLARE_DDECMD_HEADER
#define	DECLARE_DDECMD_EXECUTE(name) int On_##name (CmdLineParser&);
#define	DECLARE_DDECMD_REQUEST(name) StringBuffer On_##name (CmdLineParser&);
#define	DECLARE_DDECMD_POKE(name)    int On_##name (CmdLineParser&);
#define	DECLARE_DDECMD_FINAL
#define	DECLARE_DDECMD_FOOTER
#include "cmd.h"
#undef	DECLARE_DDECMD_HEADER
#undef	DECLARE_DDECMD_EXECUTE
#undef	DECLARE_DDECMD_REQUEST
#undef	DECLARE_DDECMD_POKE
#undef	DECLARE_DDECMD_FINAL
#undef	DECLARE_DDECMD_FOOTER

private:
	DWORD m_ddeInst; //	DDE サーバのインスタンスハンドル
	HCONV m_hConv; // 通信ハンドル
	StringBuffer m_sbKeepName; // KeepAlive 時のオブジェクト識別名
	DdeString m_dsAdvTopicName; // アドバイスループのトピック名
	DdeString m_dsAdvItemName; // アドバイスループのアイテム名
	HANDLE m_hAdvEvent;	// アドバイスデータのアクセス同期を取る
						// イベントオブジェクトのハンドル
	DWORD m_dwLastError; //	最後の Execute コマンドのエラー値

	Auto_Ptr<Thread> m_pComDlgThread; // コモンダイアログを表示するスレッド

	ConvData(const ConvData&);
	ConvData& operator=(const ConvData&);

	int do_opencomdlg(CmdLineParser&, PFNOPENCOMDLGPROC);
};

// コモンダイアログ、メニューの親ウィンドウの作成＆コマンド実行
BOOL CreateDummyDlg(DLGPROC, LONG, WPARAM, LPARAM);

#endif

