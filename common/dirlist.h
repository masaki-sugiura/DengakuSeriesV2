//	$Id: dirlist.h,v 1.2 2002/01/16 15:57:23 sugiura Exp $
/*
 *	dirlist.h
 *	各ドライブ毎のカレントディレクトリの管理を行う
 */

#ifndef	DENGAKUSERIES_CLASSES_FILESYSTEM
#define	DENGAKUSERIES_CLASSES_FILESYSTEM

#include "strbuf.h"

class PathName;

//	全ドライブのカレントディレクトリを管理するクラス
class	DirList	{
public:
	DirList(BOOL bTrueCD = TRUE);
	~DirList();

	//	与えられたパス名をフルパス名に変換する
	BOOL getPathName(const StringBuffer& name,
					PathName& buf,
					BOOL bChkValid = FALSE) const;

	//	カレントディレクトリの取得
	LPCSTR getCurrentDir(int drive = 0) const;
	//	カレントディレクトリの変更
	LPCSTR setCurrentDir(const StringBuffer& dirname);

private:
	StringBuffer* m_ppsbCurDir[32];	//	カレントディレクトリの配列
	int m_nCurDrive; //	カレントドライブのインデックス
	BOOL m_bTrueCD; // setCurrentDir() で実際にディレクトリを移動するか

	DirList(const DirList&);
	DirList& operator=(const DirList&);

	//	相対パスを解決する
	static BOOL resolveRelPath(int, const StringBuffer&, PathName&);
};

#endif
