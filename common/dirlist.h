//	$Id: dirlist.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	dirlist.h
 *	各ドライブ毎のカレントディレクトリの管理を行う
 */

#ifndef	DENGAKUSERIES_CLASSES_FILESYSTEM
#define	DENGAKUSERIES_CLASSES_FILESYSTEM

#include "strbuf.h"

class PathName;

#define	ISPATHINVALID			0x00000000	/* 無効なパス名 */

#define	ISPATHSPECIFIED			0x00000001	/* ...dir1\dir2\foo */
#define	ISPATHFROMROOT			0x00000002	/* [header]\dir1\dir2\foo */
#define	ISDRIVESPECIFIED		0x00000010	/* X:... */
#define	ISSERVERSPECIFIED		0x00000020	/* \\computer... */
#define	ISSHARENAMESPECIFIED	0x00000040	/* \\computer\share... */

#define	ISPATHLOCAL				0x00000100	/* X:\.. 型のパス */
#define	ISPATHREMOTE			0x00000200	/* \\computer\share\.. 型のパス */

#define	INVALIDPATH		ISPATHINVALID
#define	ABSOLUTEPATH	(ISPATHLOCAL|ISDRIVESPECIFIED|ISPATHSPECIFIED|ISPATHFROMROOT)
#define	DRIVERELPATH	(ISPATHLOCAL|ISDRIVESPECIFIED|ISPATHSPECIFIED)
#define	ROOTPATH		(ISPATHLOCAL|ISPATHSPECIFIED|ISPATHFROMROOT)
#define	RELPATH			(ISPATHLOCAL|ISPATHSPECIFIED)


inline int
DRIVENUM(TCHAR ch)
{
	return IsCharLowerCase(ch) ? (ch - 'a' + 1) : (ch - 'A' + 1);
}

inline BOOL
ISDRIVELETTER(TCHAR ch)
{
	return IsCharAlphabet(ch);
}

//	全ドライブのカレントディレクトリを管理するクラス
class	DirList	{
public:
	DirList(BOOL bTrueCD = TRUE);
	~DirList();

	//	与えられたパスの種類を返す
	static int getPathType(const StringBuffer&);

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
