//	$Id: si_common.cpp,v 1.5 2006-06-16 15:43:57 sugiura Exp $
/*
 *	si_common.cpp
 *	共通サービスの関数
 */

#include "misc.h"
#include "session.h"
#include "cmdline.h"
// 中途半端な実装のため削除
// #include "evalexpr.h"
#include "str_tbl.h"
#include "thread.h"

StringBuffer
SessionInstance::si_getlibversion()
{
	return GetString(STR_VERSION);
}

StringBuffer
SessionInstance::si_getosinfo(CmdLineParser& params)
{
	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!::GetVersionEx(&osi)) return nullStr;
	LPCSTR type = NULL;
	switch (osi.dwPlatformId) {
	case VER_PLATFORM_WIN32s:
		type = "32s";
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		type = "9X";
		break;
	case VER_PLATFORM_WIN32_NT:
		type = "NT";
		break;
	}
	switch (params.itemNum()) {
	case 0:
		return StringBuffer(type)
				.append((TCHAR)',')
				.append((int)osi.dwMajorVersion)
				.append((TCHAR)'.').append((int)osi.dwMinorVersion)
				.append((TCHAR)',')
				.append((int)osi.dwBuildNumber)
				.append((TCHAR)',')
				.append(osi.szCSDVersion);
	case 1:
		{
			StringBuffer ret(32);
			const StringBuffer& opt = params.getArgvStr(0);
			if (!opt.compareTo("major",FALSE)) {
				ret.append((int)osi.dwMajorVersion);
			} else if (!opt.compareTo("minor",FALSE)) {
				ret.append((int)osi.dwMinorVersion);
			} else if (!opt.compareTo("type",FALSE)) {
				ret.append(type);
			} else if (!opt.compareTo("build",FALSE)) {
				ret.append((int)osi.dwBuildNumber);
			} else if (!opt.compareTo("extra",FALSE)) {
				ret.append(osi.szCSDVersion);
			}
			return ret;
		}
	default:
		{
			const StringBuffer& opt = params.getArgvStr(0);
			if (opt.length() < 2 ||
				!isopthead(opt[0]) || opt[1] != 'f') return nullStr;
			StringBuffer ret(32);
			for (LPCSTR popt = params.getArgv(1); *popt != '\0'; popt++) {
				if (*popt == '%') {
					//	Fomat specification
					switch (*++popt) {
					case 'M':	//	OS Major
						ret.append((int)osi.dwMajorVersion);
						continue;
					case 'm':	//	OS Minor
						ret.append((int)osi.dwMinorVersion);
						continue;
					case 't':	//	OS Type
						ret.append(type);
						continue;
					case 'b':	//	Build Number
						ret.append((int)osi.dwBuildNumber);
						continue;
					case 'e':	//	Extra String
						ret.append(osi.szCSDVersion);
						continue;
					}
				}
				//	Not a format specification.
				ret.append(*popt);
			}
			return ret;
		}
	}
	// not reached.
}

#if 0
StringBuffer
SessionInstance::si_calc(const StringBuffer& fmt, const StringBuffer& expr)
{
	return EvalExpression::eval(fmt,expr);
}
#endif

StringBuffer
SessionInstance::si_getcaretpos()
{
	GUITHREADINFO guiThreadInfo;
	guiThreadInfo.cbSize = sizeof(guiThreadInfo);
	if (!::GetGUIThreadInfo(0, &guiThreadInfo)) {
		return "";
	}

	POINT ptCaret;
	ptCaret.x = guiThreadInfo.rcCaret.right;
	ptCaret.y = guiThreadInfo.rcCaret.bottom;
	::ClientToScreen(guiThreadInfo.hwndCaret, &ptCaret);

	TCHAR buf[32];
	wsprintf(buf, "%d,%d", ptCaret.x, ptCaret.y);

	return buf;
}

StringBuffer
SessionInstance::si_getcursorpos()
{
	POINT ptCursor;

	::GetCursorPos(&ptCursor);

	TCHAR buf[32];
	wsprintf(buf, "%d,%d", ptCursor.x, ptCursor.y);

	return buf;
}

int
SessionInstance::si_sleep(int nTime)
{
	::Sleep((DWORD)nTime);
	return 1;
}

#define BTYPE_ALL	(MB_OK|MB_OKCANCEL|MB_ABORTRETRYIGNORE|MB_YESNO|MB_YESNOCANCEL|MB_RETRYCANCEL)
#define ITYPE_ALL	(MB_ICONEXCLAMATION|MB_ICONINFORMATION|MB_ICONQUESTION|MB_ICONSTOP)
#define DTYPE_ALL	(MB_DEFBUTTON1|MB_DEFBUTTON2|MB_DEFBUTTON3|MB_DEFBUTTON4)

#define ELEMENTS(a)	(sizeof(a) / sizeof(a[0]))

struct FlagEntry {
	LPCSTR	m_pszName;
	UINT	m_uValue;
};

static FlagEntry feButton[] = {
	{ "ok", MB_OK },
	{ "okcancel", MB_OKCANCEL },
	{ "abortretryignore", MB_ABORTRETRYIGNORE },
	{ "yesno", MB_YESNO },
	{ "yesnocancel", MB_YESNOCANCEL },
	{ "retrycancel", MB_RETRYCANCEL }
};

static FlagEntry feIcon[] = {
	{ "none", 0 },
	{ "critical", MB_ICONSTOP },
	{ "question", MB_ICONQUESTION },
	{ "exclamation", MB_ICONEXCLAMATION },
	{ "information", MB_ICONINFORMATION }
};

static UINT
SetFlag(LPCSTR type, UINT uFlags, UINT uFlagMask, const FlagEntry* pFlagEntry, int nFlagEntrySize)
{
	for (int i = 0; i < nFlagEntrySize; i++) {
		if (lstrcmp(pFlagEntry[i].m_pszName, type) == 0) {
			uFlags &= ~uFlagMask;
			uFlags |= pFlagEntry[i].m_uValue;
			break;
		}
	}

	return uFlags;
}

struct MsgThreadProcArgs {
	StringBuffer m_strMessage;
	StringBuffer m_strCaption;
	UINT m_uFlags;
	int m_nResult;
	DWORD m_dwTimeout;

	MsgThreadProcArgs(const StringBuffer& msg,
					  const StringBuffer& caption,
					  CmdLineParser& opt)
		: m_strMessage(msg),
		  m_strCaption(caption),
		  m_uFlags(0),
		  m_nResult(0),
		  m_dwTimeout(INFINITE)
	{
		StringBuffer strDefButton;
		int num = opt.initSequentialGet();
		while (num-- > 0) {
			const StringBuffer& s = opt.getNextArgvStr();
			if (isopthead(s[0])) {
				switch (s[1]) {
				case 'b':	// ボタンの種類
					if (num-- > 0) {
						m_uFlags = SetFlag(opt.getNextArgv(), m_uFlags, BTYPE_ALL,
										   feButton, ELEMENTS(feButton));
					}
					break;
				case 'i':	// アイコンの種類
					if (num-- > 0) {
						m_uFlags = SetFlag(opt.getNextArgv(), m_uFlags, ITYPE_ALL,
										   feIcon, ELEMENTS(feIcon));
					}
					break;
				case 'd':	// デフォルトボタンの種類
					if (num-- > 0) {
						// ボタンの種類オプションに依存するため、後で評価する
						strDefButton = opt.getNextArgvStr();
					}
					break;
#if 0
				case 's':	// システムモーダルにするかどうか
					m_uFlags &= ~(MB_APPLMODAL|MB_SYSTEMMODAL|MB_TASKMODAL);
					m_uFlags |= MB_SYSTEMMODAL;
					break;
#endif
				case 't':	// タイムアウト時間指定
					if (num-- > 0) {
						int nTimeout = ival(opt.getNextArgv());
						if (nTimeout <= 0) {
							m_dwTimeout = INFINITE;
						} else {
							m_dwTimeout = nTimeout * 1000;
						}
					}
					break;
				}
			}
		}

		// デフォルトボタンの処理
		if (strDefButton.length() > 0) {
			switch (m_uFlags & BTYPE_ALL) {
			case MB_OK:
				m_uFlags &= ~DTYPE_ALL;
				m_uFlags |= MB_DEFBUTTON1;
				break;
			case MB_OKCANCEL:
			case MB_RETRYCANCEL:
				if (strDefButton.compareTo("cancel") == 0) {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON2;
				} else {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON1;
				}
				break;
			case MB_YESNO:
				if (strDefButton.compareTo("no") == 0) {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON2;
				} else {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON1;
				}
				break;
			case MB_YESNOCANCEL:
				if (strDefButton.compareTo("cancel") == 0) {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON3;
				} else if (strDefButton.compareTo("no") == 0) {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON2;
				} else {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON1;
				}
				break;
			case MB_ABORTRETRYIGNORE:
				if (strDefButton.compareTo("ignore") == 0) {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON3;
				} else if (strDefButton.compareTo("retry") == 0) {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON2;
				} else {
					m_uFlags &= ~DTYPE_ALL;
					m_uFlags |= MB_DEFBUTTON1;
				}
				break;
			}
		}
	}
};

static LPCSTR
GetResultStr(int nResult)
{
	switch (nResult) {
	case -1:
		return "";
	case IDOK:
		return "ok";
	case IDCANCEL:
		return "cancel";
	case IDYES:
		return "yes";
	case IDNO:
		return "no";
	case IDABORT:
		return "abort";
	case IDRETRY:
		return "retry";
	case IDIGNORE:
		return "ignore";
	default:
		return "!";
	}
}

static DWORD WINAPI
MessageProc(LPVOID pvParam)
{
	GUITHREADINFO thInfo;
	thInfo.cbSize = sizeof(thInfo);
	BOOL bRet = ::GetGUIThreadInfo(0, &thInfo);

	MsgThreadProcArgs* pArgs = (MsgThreadProcArgs*)pvParam;
	
	pArgs->m_nResult = ::MessageBox(NULL, pArgs->m_strMessage, pArgs->m_strCaption, pArgs->m_uFlags);

	if (bRet) {
		::SetForegroundWindow(thInfo.hwndActive);
		::SetFocusForced(thInfo.hwndFocus);
	}

	return 0;
}

StringBuffer
SessionInstance::si_msgbox(const StringBuffer& msg, CmdLineParser& opt, const StringBuffer& caption)
{
	MsgThreadProcArgs args(msg, caption, opt);

	if (args.m_dwTimeout == INFINITE) {
		//	direct call
		MessageProc(&args);
	} else {
		Thread thread(MessageProc, &args);

		if (!thread.run()) {
			return "!";
		}

		switch (thread.stop(args.m_dwTimeout)) {
		case WAIT_TIMEOUT:
			args.m_nResult = -1;
			break;
		case WAIT_OBJECT_0:
			break;
		default:
			return "!";
		}
	}

	return GetResultStr(args.m_nResult);
}

