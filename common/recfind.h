//	$Id: recfind.h,v 1.3 2002-02-15 17:46:08 sugiura Exp $
/*
 *	recfind.h
 *	enum** 関数のためのパス検索クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_RECFINDFILE
#define	DENGAKUSERIES_CLASSES_RECFINDFILE

#include "strbuf.h"
#include "stack.h"
#include "pathname.h"
#include "enumpath.h"
#include "enumerator.h"

#define RECFIND_REVERSE		0x00001000	//	逆順にソート
#define RECFIND_RECURSIVE	0x00002000	//	再帰的に検索
#define RECFIND_SHOWDIR		0x00004000	//	ディレクトリも表示

// predeclaration
class DirList;

// パス検索クラス群の基底クラス
class FindData : public Enumerator<StringBuffer> {
public:
	FindData(const DirList& dirlist, const PathName&);
	virtual ~FindData();

	// virtual methods of Enumerator
	BOOL isValid() const;
	BOOL findNext();
	StringBuffer getValue() const;

	DWORD getAttributes();
	DWORD getSize();
	const FILETIME*	getTime();

protected:
	const DirList& m_DirList;
	PathName m_pnPathBuf, m_pnForGetAttrib;
	StringBuffer m_sbBaseName;
	EnumSortedPath* m_pCurrentPath;	//	cannot be Auto_Ptr, see source
	BOOL m_bGotAttrib;

	FindData(const FindData&);
	FindData& operator=(const FindData&);

	BOOL setPath(const StringBuffer&, const StringBuffer&, DWORD);
};

//	ファイル検索のためのクラス
class FindFile : public FindData {
public:
	FindFile(
		const DirList& dirlist,
		const PathName& path,
		const StringBuffer& filter,
		const StringBuffer& s_order,
		DWORD flags)
		: FindData(dirlist, path)
	{
		this->setPath(filter,s_order,ENUMPATH_FINDFILE|flags);
	}
};

//	ディレクトリ検索のためのクラス
class FindDir : public FindData {
public:
	FindDir(
		const DirList& dirlist,
		const PathName& path,
		const StringBuffer& filter,
		const StringBuffer& s_order,
		DWORD flags)
		: FindData(dirlist, path)
	{
		this->setPath(filter,s_order,ENUMPATH_FINDDIR|flags);
	}
};

//	（再帰）ファイル・ディレクトリ検索のための(抽象)基底クラス
class RecFindData : public FindData {
public:
	RecFindData(
		const DirList& dirlist,
		const PathName&,
		const StringBuffer&,
		const StringBuffer&,
		const StringBuffer&,
		DWORD);
	~RecFindData();

	BOOL findNext() = 0; // to be overridden by extended classes
	StringBuffer getValue() const;

protected:
	const StringBuffer m_sDirFilter;	//	ディレクトリ検索のフィルタ
	const StringBuffer m_sFileFilter;	//	ファイル検索のフィルタ
	const StringBuffer m_sSortOrder;	//	ソートオーダー
	DWORD m_flags;	//	ディレクトリを検索対象に含めるかどうか
	int	m_nIniLength;		//	m_pnPathBuf の長さの初期値
	BOOL m_bDirFirstTime;	//	派生クラスによって使い方が異なるのでソース参照
	Stack<EnumSortedPath> m_Stack;	//	EnumSortedPath* のスタック

	//	最初の検索の初期化
	BOOL initFileSearch();
	BOOL initDirSearch();
	virtual BOOL initSearch() = 0;

	//	スタックに検索データをプッシュ
	void pushPath();
	EnumSortedPath*	popPath();
};

class RecFindForward : public RecFindData {
public:
	RecFindForward(
		const DirList& dirlist,
		const PathName&,
		const StringBuffer&,
		const StringBuffer&,
		const StringBuffer&,
		DWORD);

	BOOL findNext();

protected:
	BOOL initSearch();
};

class RecFindBackward : public RecFindData {
public:
	RecFindBackward(
		const DirList& dirlist,
		const PathName&,
		const StringBuffer&,
		const StringBuffer&,
		const StringBuffer&,
		DWORD);

	BOOL findNext();

protected:
	BOOL initSearch();
};

#endif
