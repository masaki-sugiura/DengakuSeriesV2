//	$Id: si_common.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	si_common.cpp
 *	共通サービスの関数
 */

#include "session.h"
#include "cmdline.h"
// 中途半端な実装のため削除
// #include "evalexpr.h"
#include "str_tbl.h"

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

