//	$Id: ddfile.h,v 1.2 2002-02-28 15:32:30 sugiura Exp $
/*
 *	ddfile.h
 *	ダイアログデータのデータファイルを扱うクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_DDFILE
#define	DENGAKUSERIES_CLASSES_DDFILE

#include "strbuf.h"

class PathName;

class DlgDataFile {
public:
	DlgDataFile(const PathName&);

	BOOL isValid();
	void reset();

	void setSecName(const StringBuffer&);

	//	2nd and 3rd args of all of functions are keyname and secname.
	BOOL read(int*, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL read(WORD*, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL read(DWORD*, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL read(StringBuffer&, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL write(int, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL write(WORD, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL write(DWORD, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL write(const StringBuffer&, const StringBuffer&, const StringBuffer& sec = nullStr);

private:
	const StringBuffer m_filename;
	StringBuffer m_secname;

	DlgDataFile(const DlgDataFile&);
	DlgDataFile& operator=(const DlgDataFile&);
};

#endif
