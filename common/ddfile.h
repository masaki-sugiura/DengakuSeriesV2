//	$Id: ddfile.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
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
	BOOL read(int*, LPCSTR, LPCSTR sec = NULL);
	BOOL read(WORD*, LPCSTR, LPCSTR sec = NULL);
	BOOL read(DWORD*, LPCSTR, LPCSTR sec = NULL);
	BOOL read(StringBuffer&, LPCSTR, LPCSTR sec = NULL);
	BOOL write(int, LPCSTR, LPCSTR sec = NULL);
	BOOL write(WORD, LPCSTR, LPCSTR sec = NULL);
	BOOL write(DWORD, LPCSTR, LPCSTR sec = NULL);
	BOOL write(LPCSTR, LPCSTR, LPCSTR sec = NULL);

private:
	const StringBuffer m_filename;
	StringBuffer m_secname;

	DlgDataFile(const DlgDataFile&);
	DlgDataFile& operator=(const DlgDataFile&);
};

#endif
