// DdeClientTestApp.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CDdeClientTestAppApp:
// このクラスの実装については、DdeClientTestApp.cpp を参照してください。
//

class CDdeClientTestAppApp : public CWinApp
{
public:
	CDdeClientTestAppApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CDdeClientTestAppApp theApp;