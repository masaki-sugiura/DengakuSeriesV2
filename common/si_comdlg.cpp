// $Id: si_comdlg.cpp,v 1.2 2002-02-15 17:46:08 sugiura Exp $
/*
 *	si_comdlg.cpp
 *	�R�����_�C�A���O�\���֐�
 */

#include "session.h"
#include "dirlist.h"
#include "cmdline.h"
#include "seldir.h"
#include "misc.h"
#include "colortbl.h"

#define FILEDLG_BUFSIZE (11 * MAX_PATH)

//	�c�k�k�ƃT�[�o�̈����̏����̈Ⴂ���z�����邽�߂̃��\�b�h
//	�����L�^�i�C(ToT)
void
SessionInstance::reorderArgv_getfilename(
	StringBuffer& argv0,
	StringBuffer& argv1) const
{
	//	nothing to do (for DLL)
}

StringBuffer
SessionInstance::getFileNameByDlg(
	HWND hwndOwner,
	const StringBuffer& _title,
	const StringBuffer& _inifile,
	CmdLineParser& filters)
{
	LPSTR buf = NULL;
	try {
		buf = new TCHAR[sizeof(OPENFILENAME) + FILEDLG_BUFSIZE];
	} catch (exception&) {
		return nullStr;
	}

	::ZeroMemory(buf,sizeof(OPENFILENAME) + FILEDLG_BUFSIZE);

	LPSTR pszFileName = buf + sizeof(OPENFILENAME),
		  pszFilter	  = buf + sizeof(OPENFILENAME) + MAX_PATH;

	//	���̃��\�b�h�̈Ӗ��́c��L�Q��
	StringBuffer title = _title, inifile = _inifile;
	this->reorderArgv_getfilename(title,inifile);

	PathName inipath;
	if (inifile.length() <= 0) {	//	�t�H���_�w��Ȃ�
		//	���݂̃t�H���_���g�p
		inipath.reset(m_DirList.getCurrentDir());
	} else if (m_DirList.getPathName(inifile,inipath,TRUE)) {
		if (!inipath.isDirectory()) {	//	�p�X���̓t�@�C��
			lstrcpy(pszFileName,inipath.getBaseName());
			inipath.delPath(1);
		}
	} else {	//	�t�@�C�������������A�܂��̓t�H���_�����s��
		lstrcpy(pszFileName,inipath.getBaseName());
		inipath.delPath(1);
		if (!inipath.isDirectory()) {
			*pszFileName = '\0';
			inipath.reset(m_DirList.getCurrentDir());
		}
	}

	//	�t�@�C���t�B���^�̐ݒ�
	int	exp_num = 0;
	LPCSTR exp_list[64];
	::ZeroMemory(exp_list,sizeof(exp_list));
	if (filters.initSequentialGet() > 0) {
		while (exp_num < 64 &&
			   (exp_list[exp_num] = filters.getNextArgv()) != NULL)
			exp_num++;
	}
	if (exp_num < 64)
		exp_list[exp_num++] = "�S�Ẵt�@�C�� (*.*)";	//	���X�g�̍Ō�ɒǉ�

	LPSTR pfbuf = pszFilter;
	LPCSTR av, phead, ptop, pend;
	int i = 0, slen, flen;
	while (i < exp_num && (av = exp_list[i++]) != NULL) {
		//	"explanation(filters)" �� "filters" �������擾
		ptop = pend = NULL;
		for (phead = av; *phead != '\0'; phead++) {
			switch (*phead) {
			case '(':
				ptop = phead + 1;
				break;
			case ')':
				pend = phead;
				break;
			}
		}
		slen = phead - av;	//	"explanation(filters)" �̒���
		//	�t�B���^�w�肪�Ȃ�����
		if (ptop == NULL || pend == NULL || (flen = pend - ptop) < 3)
			continue;
		//	�o�b�t�@�T�C�Y������Ȃ�
		if (MAX_PATH - (pfbuf - pszFilter) <= slen + flen + 3) break;
		::CopyMemory(pfbuf,av,slen);
		pfbuf[slen] = '\0';
		::CopyMemory(pfbuf += slen + 1, ptop, flen);
		pfbuf[flen] = '\0';
		pfbuf += flen + 1;
	}

	//	OPENFILENAME �\���̂̏�����
	OPENFILENAME* pofn = (OPENFILENAME*)buf;
	pofn->lStructSize		=	sizeof(OPENFILENAME);
	pofn->hwndOwner			=	hwndOwner;
	pofn->lpstrFilter		=	pszFilter;
	pofn->nFilterIndex		=	1;
	pofn->lpstrFile			=	pszFileName;
	pofn->nMaxFile			=	FILEDLG_BUFSIZE - MAX_PATH;
	pofn->lpstrTitle		=	title;
	pofn->Flags				=	OFN_HIDEREADONLY|OFN_EXPLORER|OFN_NOCHANGEDIR|
								OFN_ALLOWMULTISELECT;
	pofn->lpstrInitialDir	=	inipath;

	//	�_�C�A���O�̕\��
	StringBuffer ret = nullStr;
	if (::GetOpenFileName(pofn)) {
		LPSTR bufPtr = pszFileName;
		//	�����t�@�C����I�������ꍇ
		while (*((bufPtr += lstrlen(bufPtr)) + 1) != '\0')
			*bufPtr = '|';
		ret = pszFileName;
	}

	delete [] buf;

	return ret;
}

StringBuffer
SessionInstance::getDirNameByDlg(
	HWND hwndOwner,
	const StringBuffer& title,
	const StringBuffer& inidir,
	int flags)
{
	//	�����t�H���_���̎擾
	PathName pnInidir;
	if (!m_DirList.getPathName(inidir,pnInidir,TRUE)) {
		//	�s���ȃp�X�����n���ꂽ->���݂̃t�H���_�����g�p
		pnInidir.reset(m_DirList.getCurrentDir());
	} else if (!pnInidir.isDirectory()) {
		//	�p�X���̓t�@�C����������->�t�@�C�����̍폜
		pnInidir.delPath(1);
	}

	return SelectDirByDlg(m_hInstance).doModal(hwndOwner,flags,title,pnInidir);
}

UINT CALLBACK
ChooseColorProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG) {
		CHOOSECOLOR* pcc = reinterpret_cast<CHOOSECOLOR*>(lParam);
		::SetWindowText(hDlg,(LPCSTR)pcc->lCustData);
	}
	return 0;
}

StringBuffer
SessionInstance::getColorByDlg(
	HWND hwndOwner,
	const StringBuffer& title,
	CmdLineParser& inicolors)
{
	COLORREF cr[17];
	inicolors.initSequentialGet();
	int i = 0;
	while (i < 17) {
		const StringBuffer& arg = inicolors.getNextArgvStr();
		if (arg.length() == 0) break;
		if (arg.charAt(0) == '#') {
			int len = arg.length(), head = 0;
			while (i < 17 && head < len) {
				cr[i++] = ColorTable::colorStrToColorRef(arg.extract(head,7));
				head += 7;
			}
		} else {
			cr[i++] = m_ColorTbl.getColorRef(arg);
		}
	}
	while (i < 17) cr[i++] = 0x00FFFFFF;

	CHOOSECOLOR	cc;
	cc.lStructSize	=	sizeof(CHOOSECOLOR);
	cc.hwndOwner	=	hwndOwner;
	cc.hInstance	=	NULL;
	cc.rgbResult	=	cr[0];
	cc.lpCustColors	=	cr + 1;
	cc.Flags		=	CC_ANYCOLOR|CC_RGBINIT|CC_ENABLEHOOK;
	cc.lCustData	=	(LPARAM)(LPCSTR)title;
	cc.lpfnHook		=	(LPCCHOOKPROC)ChooseColorProc;

	if (!::ChooseColor(&cc)) return nullStr;
	StringBuffer ret(ColorTable::colorRefToColorStr(cc.rgbResult));
	for (i = 1; i < 17; i++) {
		if (cr[i] != 0x00FFFFFF) {
			ret.append(ColorTable::colorRefToColorStr(cr[i]));
		}
	}
	return ret;
}

UINT CALLBACK
ChooseFontProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG) {
		CHOOSEFONT* pcf = reinterpret_cast<CHOOSEFONT*>(lParam);
		::SetWindowText(hDlg,(LPCSTR)pcf->lCustData);
	}
	return 0;
}

StringBuffer
SessionInstance::getFontByDlg(
	HWND hwndOwner,
	const StringBuffer& title,
	CmdLineParser& argv)
{
	LPCSTR pstrFontFace = NULL, pstrFontSize = NULL,
		   pstrFontColor = NULL, pstrFontType = NULL;
	if (argv.initSequentialGet() > 0) {
		pstrFontFace  = argv.getNextArgv();
		pstrFontSize  = argv.getNextArgv();
		pstrFontColor = argv.getNextArgv();
		pstrFontType  = argv.getNextArgv();
	}

	if (pstrFontFace == NULL) pstrFontFace = "";
	if (pstrFontSize == NULL || *pstrFontSize == '\0') pstrFontSize = "12";
	if (pstrFontColor == NULL || *pstrFontColor == '\0')
		pstrFontColor = "#000000";
	if (pstrFontType == NULL) pstrFontType = "";

	LOGFONT lf;
	CHOOSEFONT cf;
	::ZeroMemory(&cf,sizeof(CHOOSEFONT));
	cf.lStructSize	=	sizeof(CHOOSEFONT);
	cf.hwndOwner	=	hwndOwner;
	cf.lpLogFont	=	&lf;
	cf.iPointSize	=	0;
	cf.Flags		=	CF_SCREENFONTS|CF_EFFECTS|CF_ENABLEHOOK|
						CF_INITTOLOGFONTSTRUCT;
	cf.rgbColors	=	m_ColorTbl.getColorRef(pstrFontColor);
	cf.lCustData	=	(LPARAM)(LPCSTR)title;
	cf.lpfnHook		=	(LPCFHOOKPROC)ChooseFontProc;
	cf.nFontType	=	SCREEN_FONTTYPE|PRINTER_FONTTYPE|SIMULATED_FONTTYPE;

	if (hwndOwner != NULL) {
		HDC	hDC = ::GetDC(hwndOwner);
		lf.lfHeight = - MulDiv(ival(pstrFontSize),
								::GetDeviceCaps(hDC,LOGPIXELSY),72);
		::ReleaseDC(cf.hwndOwner,hDC);
	} else {
		lf.lfHeight = 12;
	}
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = lf.lfUnderline = lf.lfStrikeOut = FALSE;
	while (*pstrFontType != '\0') {
		switch (*pstrFontType++) {
		case 'b':
			lf.lfWeight = FW_BOLD;
			break;
		case 'i':
			lf.lfItalic = TRUE;
			break;
		case 'u':
			lf.lfUnderline = TRUE;
			break;
		case 's':
			lf.lfStrikeOut = TRUE;
			break;
		default:
			break;
		}
	}
	lf.lfWidth = lf.lfEscapement = lf.lfOrientation = 0;
	lf.lfCharSet		=	DEFAULT_CHARSET;
	lf.lfOutPrecision	=	OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	=	CLIP_DEFAULT_PRECIS;
	lf.lfQuality		=	DEFAULT_QUALITY;
	lf.lfPitchAndFamily	=	DEFAULT_PITCH|FF_DONTCARE;
	lstrcpyn(lf.lfFaceName, pstrFontFace, LF_FACESIZE-1);

	if (!::ChooseFont(&cf)) return nullStr;

	StringBuffer ret(lf.lfFaceName,-1,40);
	ret.append((TCHAR)',').append(cf.iPointSize/10).append((TCHAR)',');
	ret.append(ColorTable::colorRefToColorStr(cf.rgbColors));
	ret.append((TCHAR)',');
	if (lf.lfWeight != FW_NORMAL) ret.append((TCHAR)'b');
	if (lf.lfItalic) ret.append((TCHAR)'i');
	if (lf.lfUnderline) ret.append((TCHAR)'u');
	if (lf.lfStrikeOut) ret.append((TCHAR)'s');

	return ret;
}

