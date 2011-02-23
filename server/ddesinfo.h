//	$Id: ddesinfo.h,v 1.1.1.1 2001/10/07 14:41:22 sugiura Exp $
/*
 *	ddesinfo.h
 *	プロセス・DDE サーバの情報を集めたクラス(というより賢い構造体(^^;)
 */

#ifndef DENGAKUSERVER_CLASSES_DDESINFO
#define DENGAKUSERVER_CLASSES_DDESINFO

#include "strbuf.h"

class DDEServerInfo {
public:
	HINSTANCE m_hInst;	//	モジュールインスタンスハンドル
	HWND m_hwndMain;	//	メインウィンドウのウィンドウハンドル
	BOOL m_bStrictEmulation;	//	マクロサーバ互換モードかどうか
	BOOL m_bShareResources;	//	１つの ConvData オブジェクトを共有するかどうか
	BOOL m_bHideIcon;	//	アイコンが非表示かどうか
	NOTIFYICONDATA m_NotifyIconData;	//	アイコンを登録するための構造体
	StringBuffer m_sbServiceName;	//	基本サービス名
	StringBuffer m_sbMutexName;	//	複数起動を抑制するための
								//	ミューテックスオブジェクト名
	StringBuffer m_sbWndTitle;		//	メインウィンドウのタイトル
	StringBuffer m_sbModuleName;	//	実行ファイル名
	StringBuffer* m_psbServerLogFileName;	//	サーバのログファイル名

	DDEServerInfo(HINSTANCE hInst)
		:	m_hInst(hInst),
			m_hwndMain(NULL),
			m_bStrictEmulation(FALSE),
			m_bShareResources(FALSE),
			m_bHideIcon(FALSE),
			m_sbServiceName(nullStr),
			m_sbMutexName(nullStr),
			m_sbWndTitle(nullStr),
			m_sbModuleName(nullStr),
			m_psbServerLogFileName(NULL)
	{
		::ZeroMemory(&m_NotifyIconData,sizeof(NOTIFYICONDATA));
	}

	~DDEServerInfo()
	{
		delete m_psbServerLogFileName;
	}
};

#endif

