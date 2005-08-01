//	$Id: common.cpp,v 1.39.2.1 2005-08-01 16:29:56 sugiura Exp $
/*
 *	common.cpp
 *	���̑��T�[�o���L�̒�`
 */

#include "common.h"

LPCSTR svr_strings[] = {
	"",
	"2.80",
	"�c�y(�ł񂪂�)�T�[�o",
	"DengakuServer",
	"HideMacroServer",
	"macserv.exe",
	"DengakuServerClass",
	"DGSvrMutex",
	"AllService\n"
		"CommonService\n"
		"FileService\n"
		"StringService\n"
		"MenuService\n"
		"DlgService\n"
		"ComDlgService\n"
		"SusieService\n"
		"GlobVarService\n"
		"BregexpService\n"
		"ColorService\n"
};

LPCSTR msg_strings[] = {
	"�c�y�T�[�o�̋N���Ɏ��s���܂����B",
	"�c�y�T�[�o�̏������Ɏ��s���܂����B",
	"�c�y�T�[�o�̒�~�Ɏ��s���܂����B",
	"�_�C�A���O�̍쐬�Ɏ��s���܂����B"
};

const StringBuffer existStr = "!";

#ifdef _DEBUG
DWORD
OutputErrorMsgToDebugTerminal(LPCSTR header)
{
	DWORD dwLastError = ::GetLastError();
	if (dwLastError != 0L) {
		static TCHAR buf[1024];
		wsprintf(buf,"%s:[%d] ",header,dwLastError);
		int	len = lstrlen(buf);
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dwLastError,
						MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
						buf+len,
						1024-len,
						NULL);
		::OutputDebugString(buf);
	}
	return dwLastError;
}
#endif
