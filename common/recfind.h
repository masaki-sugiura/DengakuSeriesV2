//	$Id: recfind.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
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

#define RECFIND_REVERSE		0x00001000	//	逆順にソート
#define RECFIND_RECURSIVE	0x00002000	//	再帰的に検索
#define RECFIND_SHOWDIR		0x00004000	//	ディレクトリも表示

// パス検索クラス群の基底クラス
class FindData {
public:
	FindData(const PathName&);
	virtual ~FindData();

	BOOL isValid() const;
	virtual BOOL findNext();

	virtual StringBuffer getBaseName() const;

	DWORD getAttributes()
	{
		return m_pnPathBuf.getAttributes();
	}

	DWORD getSize()
	{
		return m_pnPathBuf.getSize();
	}

	const FILETIME*	getTime()
	{
		return m_pnPathBuf.getTime();
	}

protected:
	PathName m_pnPathBuf;
	StringBuffer m_sbBaseName;
	EnumSortedPath* m_pCurrentPath;	//	cannot be Auto_Ptr, see source

	FindData(const FindData&);
	FindData& operator=(const FindData&);

	BOOL setPath(const StringBuffer&, const StringBuffer&, DWORD);
};

//	ファイル検索のためのクラス
class FindFile : public FindData {
public:
	FindFile(
		const PathName& path,
		const StringBuffer& filter,
		const StringBuffer& s_order,
		DWORD flags)
		: FindData(path)
	{
		this->setPath(filter,s_order,ENUMPATH_FINDFILE|flags);
	}
};

//	ディレクトリ検索のためのクラス
class FindDir : public FindData {
public:
	FindDir(
		const PathName& path,
		const StringBuffer& filter,
		const StringBuffer& s_order,
		DWORD flags)
		: FindData(path)
	{
		this->setPath(filter,s_order,ENUMPATH_FINDDIR|flags);
	}
};

//	（再帰）ファイル・ディレクトリ検索のための(抽象)基底クラス
class RecFindData : public FindData {
public:
	RecFindData(
		const PathName&,
		const StringBuffer&,
		const StringBuffer&,
		const StringBuffer&,
		DWORD);
	~RecFindData();

	BOOL findNext() = 0;
	StringBuffer getBaseName() const;

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
