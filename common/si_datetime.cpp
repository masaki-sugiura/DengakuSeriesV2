//	$Id: si_datetime.cpp,v 1.2 2002/08/06 13:33:04 sugiura Exp $
/*
 *	si_datetime.cpp
 *	日付・時刻サービスの関数
 */

#include "session.h"
#include "cmdline.h"
#include "misc.h"

#include <time.h>

static void
DosFileTimeToTime(struct tm& tm, WORD date, WORD time)
{
	struct tm gtm;

	tm.tm_year  = ((date >>  9) & 0x7F) + 80;
	tm.tm_mon   = ((date >>  5) & 0x0F) - 1;
	tm.tm_mday  = ((date >>  0) & 0x1F);
	tm.tm_hour  = ((time >> 11) & 0x1F);
	tm.tm_min   = ((time >>  5) & 0x3F);
	tm.tm_sec   = ((time >>  0) & 0x1F) * 2;
	tm.tm_isdst = 0;
}

static int
TimeToDosFileTime(const struct tm& tm)
{
	WORD date, time;

	date = (((tm.tm_year - 80) & 0x7F) << 17) |
		   (((tm.tm_mon + 1) & 0x0F)   << 11) |
		   ((tm.tm_mday & 0x1F)        << 16) |
	time = (((tm.tm_year - 80) & 0x7F) << 9) |
		   (((tm.tm_mon + 1) & 0x0F)   << 5) |
		   ((tm.tm_mday & 0x1F)        << 0);

}

StringBuffer
SessionInstance::si_datetime_time_to_str(const StringBuffer& fmt, int time)
{
	struct tm tm;
	DosFileTimeToTime(&tm, HIWORD(time), LOWORD(time));

	return buf;
}

int
SessionInstance::si_str_to_time(const StringBuffer& str)
{
	// "YYYY/MM/DD#HH/MM/SS" 形式の時刻を time に変換
	FILETIME ft;

	if (!TimeStampToFileTime(str, &ft))
		return 0;

	WORD date, time;
	::FileTimeToDosDateTime(&ft, &date, &time);

	return (int)MAKE_LONG(time, date);
}

int
SessionInstance::si_datetime_gettime()
{
	return time(NULL);
}

