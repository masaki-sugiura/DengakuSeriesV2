//	$Id: opencomdlg.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	opencomdlg.h
 *	コモンダイアログを表示するための関数
 */
//	filename:	opencomdlg.h

#ifndef	DENGAKUSERVER_OPENCOMDLG
#define	DENGAKUSERVER_OPENCOMDLG

#include <windows.h>

class ConvData;
class CmdLineParser;

typedef void (*PFNOPENCOMDLGPROC)(HWND, ConvData&, CmdLineParser&);

// 「ファイルを開く」、「フォルダの指定」ダイアログを表示する関数への引数
typedef struct {
	HWND m_hwndOwner;
	ConvData* m_pConvData;
	CmdLineParser* m_pArgv;
	PFNOPENCOMDLGPROC m_pfnComDlg;
} OPENCOMDLGPROCARGS, *LPOPENCOMDLGPROCARGS;

//	「ファイルを開く」コモンダイアログを開く＆ファイル名の取得
void GetFileNameByDlg(HWND, ConvData&, CmdLineParser&);
//	「フォルダの指定」コモンダイアログを開く＆フォルダ名の取得
void GetDirNameByDlg(HWND, ConvData&, CmdLineParser&);
//	「色の指定」コモンダイアログを開く＆色指定("#RRGGBB")の取得
void GetColorByDlg(HWND, ConvData&, CmdLineParser&);
//	「フォントの指定」コモンダイアログを開く＆フォント指定の取得
void GetFontByDlg(HWND, ConvData&, CmdLineParser&);

#endif
