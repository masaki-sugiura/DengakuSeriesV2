//	$Id: si_datetime.cpp,v 1.1 2002-08-05 16:06:17 sugiura Exp $
/*
 *	si_datetime.cpp
 *	日付・時刻サービスの関数
 */

#include "session.h"
#include "cmdline.h"

#include <time.h>

StringBuffer
SessionInstance::si_datetime_format(const StringBuffer& fmt, int time)
{
	TCHAR buf[256];

	strftime(buf, 255, fmt, gmtime((long*)&time));

	return buf;
}

int
SessionInstance::si_datetime_gettime()
{
	return time(NULL);
}

int
SessionInstance::si_datetime_filetime(int filetime)
{
	struct tm gtm;
	WORD time = LOWORD(filetime), date = HIWORD(filetime);

	gtm.tm_year  = ((date >>  9) & 0x7F) + 80;
	gtm.tm_mon   = ((date >>  5) & 0x0F) - 1;
	gtm.tm_mday  = ((date >>  0) & 0x1F);
	gtm.tm_hour  = ((time >> 11) & 0x1F);
	gtm.tm_min   = ((time >>  5) & 0x3F);
	gtm.tm_sec   = ((time >>  0) & 0x1F) * 2;
	gtm.tm_isdst = 0;

	time_t gtime = mktime(&gtm);
	return mktime(localtime(&gtime));
}

