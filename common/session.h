//	$Id: session.h,v 1.2 2001-11-22 13:37:09 sugiura Exp $
/*
 *	session.h
 *	セッションインスタンスの基底クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_SESSION
#define	DENGAKUSERIES_CLASSES_SESSION

#include "auto_ptr.h"
#include "linklist.h"
#include "dlgdata.h"
#include "dirlist.h"
#include "menu.h"
#include "recfind.h"
#include "env_var.h"
#include "spi_mngr.h"
#include "sharedbuf.h"
#include "tokenizer.h"
#include "thread.h"
#include "bregexp_mngr.h"

class PathName;
class CmdLineParser;

/*
	個々のセッションを処理するクラス

	コーディングの注意：
		・再入可能コードであること
*/
class SessionInstance {
public:
	SessionInstance(HINSTANCE, BOOL bTrueCD = TRUE);
	virtual ~SessionInstance();

	//	メンバ取得メソッド
	HINSTANCE getInstanceHandle() const { return m_hInstance; }
	DlgFrame& getDlgFrame() { return m_DlgFrame; }
	MenuList& getUserMenu() { return m_UserMenu; }

	//	フルパス名を生成する
	BOOL getPathName(const StringBuffer& file, PathName& buf,
					BOOL bValid) const
	{ return m_DirList.getPathName(file,buf,bValid); }

	//	Server で override される
	virtual void setNotify(const StringBuffer& msg)
	{ m_sharedBuf.set(msg); }
	virtual BOOL getNotify(StringBuffer& buf, DWORD wait)
	{ return m_sharedBuf.get(buf,wait); }
	virtual void resetNotify()
	{ m_sharedBuf.reset(); }

	//	メニュー表示メソッド群
	BOOL showMenuThread(const StringBuffer& menuname, POINT* pAt = NULL);
	void sendMenuResult(LPSHOWMENUPROCARGS);
	DWORD waitMenuThread(DWORD waittime = INFINITE);
	DWORD stopMenuThread(DWORD waittime = INFINITE);

	//	その他
	StringBuffer si_getlibversion();
	StringBuffer si_getosinfo(CmdLineParser&);
//	StringBuffer si_calc(const StringBuffer&, const StringBuffer&);

	//	グローバル変数操作系
	int si_setvar(const StringBuffer&, const StringBuffer&);
	StringBuffer si_getvar(const StringBuffer&);
	int si_delvar(const StringBuffer&);
	int si_existvar(const StringBuffer&);
	int si_enumvar();
	StringBuffer si_nextvar();
	int si_hasmorevars();

	//	Susie Plugin 操作系
	int si_loadspi(const StringBuffer&);
	int si_freespi();
	StringBuffer si_getspiinfo(int);
	int si_issupportedpic(const StringBuffer&);
	StringBuffer si_getpicinfo(const StringBuffer&);

	//	ファイル・フォルダ操作系
	int si_setcurdir(const StringBuffer&);
	int si_copy(CmdLineParser&);
	int si_appendcopy(const StringBuffer&, const StringBuffer&, BOOL);
	int si_rename(const StringBuffer&, const StringBuffer&);
	int si_delete(const StringBuffer&);
	int si_remove(CmdLineParser&);
	int si_move(CmdLineParser&);
	int si_mkdir(CmdLineParser&);
	int si_rmdir(CmdLineParser&);
	int si_setattribute(CmdLineParser&);
	int si_touch(CmdLineParser&);
	int si_run(CmdLineParser&, int);
	int si_enumpath(CmdLineParser&);
	int si_enumfile(CmdLineParser&);
	int si_enumdir(CmdLineParser&);
	StringBuffer si_getcurdir();
	StringBuffer si_isreadonly(CmdLineParser&);
	StringBuffer si_sizeof(CmdLineParser&);
	StringBuffer si_timestampof(CmdLineParser&);
	StringBuffer si_timecountof(CmdLineParser&);
	StringBuffer si_attributeof(CmdLineParser&);
//	StringBuffer si_existfile(const StringBuffer&);
	StringBuffer si_findnext();
	StringBuffer si_getdrives();
	StringBuffer si_getlongname(const StringBuffer&, BOOL);

	//	文字列処理系
	int si_atoi(const StringBuffer&);
	StringBuffer si_itoa(int, int radix = 0);
	int si_stricmp(const StringBuffer&, const StringBuffer&);
	int si_strcount(const StringBuffer&, const StringBuffer&);
	StringBuffer si_gsub(const StringBuffer&, const StringBuffer&,
						const StringBuffer&, int);
	StringBuffer si_tolower(const StringBuffer&);
	StringBuffer si_toupper(const StringBuffer&);
	int si_strstr(const StringBuffer&, const StringBuffer&);
	int si_strrstr(const StringBuffer&, const StringBuffer&);
	int si_strlen(const StringBuffer&);
	StringBuffer si_leftstr(const StringBuffer&, int);
	StringBuffer si_midstr(const StringBuffer&, int, int);
	StringBuffer si_rightstr(const StringBuffer&, int);
	StringBuffer si_tolower2(const StringBuffer&);
	StringBuffer si_toupper2(const StringBuffer&);
	int si_strstr2(const StringBuffer&, const StringBuffer&);
	int si_strrstr2(const StringBuffer&, const StringBuffer&);
	int si_strlen2(const StringBuffer&);
	StringBuffer si_leftstr2(const StringBuffer&, int);
	StringBuffer si_midstr2(const StringBuffer&, int, int);
	StringBuffer si_rightstr2(const StringBuffer&, int);
	StringBuffer si_ltrim(const StringBuffer&);
	StringBuffer si_rtrim(const StringBuffer&);
	StringBuffer si_gettoken(const StringBuffer&, const StringBuffer&);
	int si_hasmoretokens();

	//	メニュー表示系
	int si_newmenu(const StringBuffer&, BOOL bExtended = TRUE);
	int si_addmenuitem(const StringBuffer&, const StringBuffer&,
						const StringBuffer&, const StringBuffer&);
	int si_changemenuitem(const StringBuffer&, int,
						const StringBuffer&, const StringBuffer&);
	int si_deletemenuitem(const StringBuffer&, int);

	//	コモンダイアログ表示系
	StringBuffer getFileNameByDlg(HWND, const StringBuffer&,
								const StringBuffer&, CmdLineParser&);
	StringBuffer getDirNameByDlg(HWND, const StringBuffer&,
								const StringBuffer&, int);
	StringBuffer getColorByDlg(HWND, const StringBuffer&, const StringBuffer&);
	StringBuffer getFontByDlg(HWND, const StringBuffer&, CmdLineParser&);

	//	ダイアログ表示系
	int si_showdialog(HWND, BOOL);
	int si_enddialog(DWORD waittime = INFINITE);
	int si_setctrlstate(const StringBuffer&, CmdLineParser&);
	int si_setctrlstring(const StringBuffer&, const StringBuffer&);
	int si_setctrlimestate(const StringBuffer&, int);
	int si_changectrlitem(const StringBuffer&, CmdLineParser&,
						const StringBuffer&);
	int si_setctrlitem(const StringBuffer&, CmdLineParser&,
						const StringBuffer&);
	int si_deletectrlitem(const StringBuffer&, const StringBuffer&);
	int si_resetctrlitem(const StringBuffer&);
	int si_enablectrl(BOOL, CmdLineParser&);
	int si_setctrlfont(const StringBuffer&, CmdLineParser&);
	int si_showmessage(const StringBuffer&, const StringBuffer&);
	int si_showquestion(const StringBuffer&, const StringBuffer&,
						const StringBuffer&);
	int si_savedialog(const StringBuffer&, const StringBuffer&);
	int si_loaddialog(const StringBuffer&, const StringBuffer&);
	int si_setdlgtitle(const StringBuffer&);
	StringBuffer si_getdlgtitle();
	StringBuffer si_getctrlstate(const StringBuffer&);
	StringBuffer si_getctrlstring(const StringBuffer&);
	StringBuffer si_getctrlimestate(const StringBuffer&);
	StringBuffer si_getctrlfont(const StringBuffer&);
	StringBuffer si_getctrlitem(const StringBuffer&, const StringBuffer&);
	StringBuffer si_getdlgsignature(const StringBuffer&);
	int si_newdialog(const StringBuffer&, WORD, CmdLineParser&);
	int si_newdlgpage(const StringBuffer&, WORD);
	int si_setcurdlgpage(const StringBuffer&);
	StringBuffer si_getcurdlgpage();
	int si_newcontrol(const StringBuffer&, const StringBuffer&,
						const StringBuffer&);
	int si_setctrlwidth(const StringBuffer&, WORD);
	int si_setctrlheight(const StringBuffer&, WORD);
	int si_setctrlnotify(const StringBuffer&, CmdLineParser&);
	int si_newcolumn(WORD);
	int si_newpage(WORD);

	// BRegexp系
	int si_bregexp_load(const StringBuffer&);
	int si_bregexp_free();
	StringBuffer si_bregexp_errormsg();
	StringBuffer si_bregexp_version();
	StringBuffer si_bregexp_match(const StringBuffer&, const StringBuffer&);
	StringBuffer si_bregexp_subst(const StringBuffer&, const StringBuffer&);
	StringBuffer si_bregexp_trans(const StringBuffer&, const StringBuffer&);
	int si_bregexp_split(const StringBuffer&, const StringBuffer&, int);
	StringBuffer si_bregexp_getnext();
	int si_bregexp_hasmoreresults();

protected:
	HINSTANCE m_hInstance; // インスタンスハンドル
	DWORD m_dwLastError; // 最後の Execute コマンドのエラー値
	DlgFrame m_DlgFrame; // ユーザー定義ダイアログのクラス
	DirList m_DirList; // ドライブ毎のカレントフォルダのリスト
	MenuList m_UserMenu; // ユーザー定義メニューのクラス
	SPI_Manager m_SPIManager; // Susie Plugin をコントロールするクラス
	Auto_Ptr<EnvManager> m_pEnvManager; // 共有変数をコントロールするクラス
	Auto_Ptr<FindData> m_pFindData; // "enum*" コマンドで使うクラス
	Auto_Ptr<Tokenizer> m_pStrToken; // gettoken コマンドで使うクラス
	Auto_Ptr<Thread> m_pMenuThread; // メニュー表示で使うスレッド
	Auto_Ptr<Thread> m_pDlgThread; // ダイアログ表示で使うスレッド
	Auto_Ptr<BRegExp_Manager> m_pBRegExp_Manager; // BRegexp をコントロールするクラス

	SharedBuffer<StringBuffer> m_sharedBuf;

	SessionInstance(const SessionInstance&);
	SessionInstance& operator=(const SessionInstance&);

	//	田楽ＤＬＬとサーバの違い(引数の順序)をそれぞれの実装で
	//	吸収できないため、しかたなく用意するメソッド
	virtual void reorderArgv_getfilename(StringBuffer& title,
										StringBuffer& inipath) const;
};

//	メニューの返り値
extern const StringBuffer errorStr;
extern const StringBuffer cancelStr;

#endif
