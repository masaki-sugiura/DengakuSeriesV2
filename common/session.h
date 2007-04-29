//	$Id: session.h,v 1.24 2007-04-29 16:11:35 sugiura Exp $
/*
 *	session.h
 *	�Z�b�V�����C���X�^���X�̊��N���X
 */

#ifndef	DENGAKUSERIES_CLASSES_SESSION
#define	DENGAKUSERIES_CLASSES_SESSION

#include "auto_ptr.h"
#include "linklist.h"
#include "dlgdata.h"
#include "dirlist.h"
#include "menu.h"
#include "enumerator.h"
#include "recfind.h"
#include "env_var.h"
#include "spi_mngr.h"
#include "sharedbuf.h"
#include "tokenizer.h"
#include "thread.h"
#include "bregexp_mngr.h"
#include "colortbl.h"
#include "misc.h"

class PathName;
class CmdLineParser;

/*
	�X�̃Z�b�V��������������N���X

	�R�[�f�B���O�̒��ӁF
		�E�ē��\�R�[�h�ł��邱��
*/
class SessionInstance {
public:
	SessionInstance(HINSTANCE, BOOL bTrueCD = TRUE);
	virtual ~SessionInstance();

	//	�����o�擾���\�b�h
	HINSTANCE getInstanceHandle() const { return m_hInstance; }
	DlgFrame& getDlgFrame() { return m_DlgFrame; }
	MenuList& getUserMenu() { return m_UserMenu; }
	ColorTable& getColorTable() { return m_ColorTbl; }

	//	�t���p�X���𐶐�����
	BOOL getPathName(const StringBuffer& file, PathName& buf,
					BOOL bValid) const
	{ return m_DirList.getPathName(file,buf,bValid); }

	//	Server �� override �����
	virtual void setNotify(const StringBuffer& msg)
	{
		DEBUG_OUTPUT(("Enter setNotify(%s)", (LPCSTR)msg));
		m_sharedBuf.set(msg);
		DEBUG_OUTPUT(("Leave setNotify(%s)", (LPCSTR)msg));
	}
	virtual BOOL getNotify(StringBuffer& buf, DWORD wait)
	{
		DEBUG_OUTPUT(("Enter getNotify()"));
		BOOL bRet = m_sharedBuf.get(buf,wait);
		DEBUG_OUTPUT(("Leave getNotify(%s)", (LPCSTR)buf));
		return bRet;
	}
	virtual void resetNotify()
	{ m_sharedBuf.reset(); }

	//	���j���[�\�����\�b�h�Q
	BOOL showMenuThread(const StringBuffer& menuname,
						BOOL bShowAtCursor,
						POINT pt);
	void sendMenuResult(LPSHOWMENUPROCARGS);
	DWORD waitMenuThread(DWORD waittime = INFINITE);
	DWORD stopMenuThread(DWORD waittime = INFINITE);

	//	���̑�
	StringBuffer si_getlibversion();
	StringBuffer si_getosinfo(CmdLineParser&);
//	StringBuffer si_calc(const StringBuffer&, const StringBuffer&);
	StringBuffer si_getcaretpos();
	StringBuffer si_getcursorpos();
	int si_sleep(int);
	StringBuffer si_msgbox(const StringBuffer&, CmdLineParser&, const StringBuffer&);

	//	�O���[�o���ϐ�����n
	int si_setvar(const StringBuffer&, const StringBuffer&);
	StringBuffer si_getvar(const StringBuffer&);
	int si_delvar(const StringBuffer&);
	int si_existvar(const StringBuffer&);
	int si_enumvar();
	StringBuffer si_nextvar();
	int si_hasmorevars();

	//	Susie Plugin ����n
	int si_loadspi(const StringBuffer&);
	int si_freespi();
	StringBuffer si_getspiinfo(int);
	int si_issupportedpic(const StringBuffer&);
	StringBuffer si_getpicinfo(const StringBuffer&);

	//	�t�@�C���E�t�H���_����n
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
	StringBuffer si_fileopresult();
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
	StringBuffer si_getshortname(const StringBuffer&, BOOL);

	//	�����񏈗��n
	int si_atoi(const StringBuffer&);
	StringBuffer si_itoa(int, int radix = 0);
	int si_stricmp(const StringBuffer&, const StringBuffer&);
	int si_strcount(const StringBuffer&, const StringBuffer&);
	StringBuffer si_gsub(const StringBuffer&, const StringBuffer&,
						const StringBuffer&, int);
	StringBuffer si_reverse(const StringBuffer&);
	StringBuffer si_tolower(const StringBuffer&);
	StringBuffer si_toupper(const StringBuffer&);
	StringBuffer si_tohankaku(const StringBuffer&);
	StringBuffer si_tozenkakuhira(const StringBuffer&);
	StringBuffer si_tozenkakukata(const StringBuffer&);
	int si_strstr(const StringBuffer&, const StringBuffer&);
	int si_strrstr(const StringBuffer&, const StringBuffer&);
	int si_strlen(const StringBuffer&);
	StringBuffer si_leftstr(const StringBuffer&, int);
	StringBuffer si_midstr(const StringBuffer&, int, int);
	StringBuffer si_rightstr(const StringBuffer&, int);
	StringBuffer si_reverse2(const StringBuffer&);
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

	//	���j���[�\���n
	int si_newmenu(const StringBuffer&, BOOL bExtended = TRUE);
	int si_addmenuitem(const StringBuffer&, const StringBuffer&,
						const StringBuffer&, const StringBuffer&);
	int si_changemenuitem(const StringBuffer&, int,
						const StringBuffer&, const StringBuffer&);
	int si_deletemenuitem(const StringBuffer&, int);

	//	�R�����_�C�A���O�\���n
	StringBuffer getFileNameByDlg(HWND, const StringBuffer&,
								const StringBuffer&, CmdLineParser&);
	StringBuffer getDirNameByDlg(HWND, const StringBuffer&,
								const StringBuffer&, int);
	StringBuffer getColorByDlg(HWND, const StringBuffer&, CmdLineParser&);
	StringBuffer getFontByDlg(HWND, const StringBuffer&, CmdLineParser&);

	//	�_�C�A���O�\���n
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
	int si_setdlgimestate(int);
	StringBuffer si_getdlgimestate();
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
	int si_setfocusedctrl(const StringBuffer&);
	StringBuffer si_getcurdlgpage();
	StringBuffer si_getfocusedctrl();
	int si_newcontrol(const StringBuffer&, const StringBuffer&,
						const StringBuffer&);
	int si_setctrlwidth(const StringBuffer&, WORD);
	int si_setctrlheight(const StringBuffer&, WORD);
	int si_setctrlnotify(const StringBuffer&, CmdLineParser&);
	int si_setctrlsort(const StringBuffer&, CmdLineParser&);
	StringBuffer si_getctrlsort(const StringBuffer&);
	int si_newcolumn(WORD);
	int si_newpage(WORD);
	StringBuffer si_getdlgpos();
	int si_setdlgpos(int, int, const StringBuffer&, const StringBuffer&);
	StringBuffer si_getdlgsize();
	StringBuffer si_getctrlfocuseditem(const StringBuffer&);
	int si_setctrlfocuseditem(const StringBuffer&, const StringBuffer&);

	// BRegexp�n
	int si_bregexp_load(const StringBuffer&);
	int si_bregexp_free();
	StringBuffer si_bregexp_errormsg();
	StringBuffer si_bregexp_version();
	StringBuffer si_bregexp_match(const StringBuffer&, const StringBuffer&);
	StringBuffer si_bregexp_subst(const StringBuffer&, const StringBuffer&);
	StringBuffer si_bregexp_trans(const StringBuffer&, const StringBuffer&);
	int si_bregexp_split(const StringBuffer&, const StringBuffer&, int);
	StringBuffer si_bregexp_getnext();
	StringBuffer si_bregexp_postostr(const StringBuffer&);
	int si_bregexp_hasmoreresults();

	// �J���[�e�[�u������n
	int si_setcolorref(const StringBuffer&, const StringBuffer&);
	StringBuffer si_getcolorref(const StringBuffer&);
	StringBuffer si_getcolorname(const StringBuffer&);
	int si_loadcolorrefs(const StringBuffer&);
	int si_savecolorrefs(const StringBuffer&);

#if 0
	// ���t�E��������n
	StringBuffer si_datetime_time_to_str(const StringBuffer&, int);
	int si_datetime_str_to_time(const StringBuffer&);
	int si_datetime_gettime();
#endif

protected:
	HINSTANCE m_hInstance; // �C���X�^���X�n���h��
	DWORD m_dwLastError; // �Ō�� Execute �R�}���h�̃G���[�l
	DlgFrame m_DlgFrame; // ���[�U�[��`�_�C�A���O�̃N���X
	DirList m_DirList; // �h���C�u���̃J�����g�t�H���_�̃��X�g
	MenuList m_UserMenu; // ���[�U�[��`���j���[�̃N���X
	ColorTable m_ColorTbl; // �F���� COLORREF �Ƃ̑Ή��\
	Auto_Ptr<SPI_Manager> m_pSPIManager; // Susie Plugin ���R���g���[������N���X
	Auto_Ptr<EnvManager> m_pEnvManager; // ���L�ϐ����R���g���[������N���X
	Auto_Ptr< Enumerator<StringBuffer> > m_pFileOpResult; // �t�@�C���n�֐��̌��ʂ��i�[����N���X
	Auto_Ptr< FindData > m_pEnumerator; // "enum*" �R�}���h�Ŏg���N���X
	Auto_Ptr<Tokenizer> m_pStrToken; // gettoken �R�}���h�Ŏg���N���X
	Auto_Ptr<Thread> m_pMenuThread; // ���j���[�\���Ŏg���X���b�h
	Auto_Ptr<Thread> m_pDlgThread; // �_�C�A���O�\���Ŏg���X���b�h
	Auto_Ptr<BRegExp_Manager> m_pBRegExp_Manager; // BRegexp ���R���g���[������N���X

	SharedBuffer<StringBuffer> m_sharedBuf;

	SessionInstance(const SessionInstance&);
	SessionInstance& operator=(const SessionInstance&);
};

//	���j���[�̕Ԃ�l
extern const StringBuffer errorStr;
extern const StringBuffer cancelStr;

#endif
