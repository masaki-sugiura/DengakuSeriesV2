//	$Id: cs_handler.cpp,v 1.6 2007-05-13 17:02:41 sugiura Exp $
/*
 *	cs_handler.cpp
 *	共通サービスの実装
 */

#include "ddeserv.h"
#include "convdata.h"
#include "cmdline.h"
#include "common.h"
#include "misc.h"

int
ConvData::On_exit(CmdLineParser& params)
{
	HWND hDlg = DdeServer::gethwndDlg();	//	メインウィンドウのハンドル
	if (hDlg == NULL) return FALSE;
	LPARAM lprm = reinterpret_cast<LPARAM>(this);
	if (DdeServer::isStrictEmulation() || DdeServer::getConvNum() < 2) {
		lprm = 0;	//	全通信を終了
	} else if (params.itemNum() > 1) {
		LPCSTR av = params.getArgv(1);
		if (isopthead(*av) && *++av=='a' && *++av=='\0')
			lprm = 0;
	}
	//	ちゃんと同期取らないとちょっと危険かも(^^;
	::PostMessage(hDlg,WM_USER_DDE_STOP,0,lprm);
	return TRUE;
}

StringBuffer
ConvData::On_result(CmdLineParser& params)
{
	if (m_dwLastError == 0) return nullStr;
	if (params.itemNum() > 0) {
		LPCSTR av = params.getArgv(0);
		if (!isopthead(*av) || *++av != 's' || *++av != '\0') return nullStr;
		//	システムエラーメッセージを取得
		LPVOID	lpMsgBuf;
		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
						FORMAT_MESSAGE_FROM_SYSTEM|
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						m_dwLastError,
						MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
						reinterpret_cast<LPSTR>(&lpMsgBuf),
						0,
						NULL);
		StringBuffer ret = reinterpret_cast<LPCSTR>(lpMsgBuf);
		::LocalFree(lpMsgBuf);
		return ret;
	} else {
		//	エラー値をそのまんま(^^;出力
		return StringBuffer(nullStr).append(m_dwLastError);
	}
	//	not reached.
}

StringBuffer
ConvData::On_keepalive(CmdLineParser& params)
{
	if (params.itemNum() > 0) {
		//	change keep alive state
		if (ival(params.getArgv(0))) {
			//	set active
			if (m_sbKeepName.length() == 0) {
				//	to guarantee the name of keepalive is unique.
				m_sbKeepName.append((DWORD)m_hConv);
			}
		} else {
			//	set inactive
			m_sbKeepName.reset();
		}
	}
	return m_sbKeepName;
}

StringBuffer
ConvData::On_getlibversion(CmdLineParser&)
{
	return this->si_getlibversion();
}

StringBuffer
ConvData::On_getversion(CmdLineParser&)
{
//	return GetServerString(SVR_STR_VERSION);
	return getVersion();
}

StringBuffer
ConvData::On_getosinfo(CmdLineParser& params)
{
	return si_getosinfo(params);
}

StringBuffer
ConvData::On_getcaretpos(CmdLineParser&)
{
	return this->si_getcaretpos();
}

StringBuffer
ConvData::On_getcursorpos(CmdLineParser&)
{
	return this->si_getcursorpos();
}

int
ConvData::On_sleep(CmdLineParser& params)
{
	return this->si_sleep(ival(params.getArgv(0)));
}

StringBuffer
ConvData::On_msgbox(CmdLineParser& params)
{
	RealCmdLineParser argv(params.getRawData());
	argv.delArgv(0);
	argv.delArgv(-1);
	return this->si_msgbox(params.getArgvStr(0), argv, params.getArgvStr(-1));
}

