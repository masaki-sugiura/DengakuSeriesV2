//	$Id: rec_op.h,v 1.4 2003-07-06 16:27:46 sugiura Exp $
/*
 *	rec_op.h
 *	再帰ファイル操作のためのクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_RECURSIVEOP
#define	DENGAKUSERIES_CLASSES_RECURSIVEOP

#include "pathname.h"

class SeqOpResult;

//	基底クラス
class RecursiveOperation {
public:
	RecursiveOperation(const PathName&, const PathName&, DWORD, SeqOpResult*);
	virtual	~RecursiveOperation();

	int	doOperation();

protected:
	DWORD m_fConfirm;
	PathName m_OrgPathBuf;
	PathName m_DestDirBuf;
	LPWIN32_FIND_DATA m_pFindData;
	SeqOpResult* m_psor;

	RecursiveOperation(const RecursiveOperation&);
	RecursiveOperation& operator=(const RecursiveOperation&);

	virtual	int	recursiveDirOp();
	virtual	int	preDirOp();
	virtual	int	postDirOp();
	virtual	int	FiletoFile();
};

class RecursiveCopyDir : public RecursiveOperation {
public:
	RecursiveCopyDir(const PathName&, const PathName&, DWORD, SeqOpResult*);

protected:
	int	preDirOp();
	int	FiletoFile();
};

class RecursiveMoveDir : public RecursiveOperation {
public:
	RecursiveMoveDir(const PathName&, const PathName&, DWORD, SeqOpResult*);

protected:
	int	preDirOp();
	int	postDirOp();
	int	FiletoFile();
};

class RecursiveRemoveDir : public RecursiveOperation {
public:
	RecursiveRemoveDir(const PathName&, DWORD, SeqOpResult*);

protected:
	int	postDirOp();
	int	FiletoFile();
};

class RecursiveSetAttributes : public RecursiveOperation {
public:
	RecursiveSetAttributes(const PathName&, DWORD, SeqOpResult*);

protected:
	int	postDirOp();
	int	FiletoFile();
};

class RecursiveSetTime : public RecursiveOperation {
public:
	RecursiveSetTime(const PathName&, const FILETIME*, DWORD, SeqOpResult*);

protected:
	FILETIME m_ft;
	int	FiletoFile();
};

//	再帰処理系の関数の返り値
#define	RO_SUCCESS	0	/* 成功 */
#define	RO_FAILED	1	/* 処理の失敗 */
#define	RO_STOP		2	/* 処理の停止要求 */
#define RO_CANCELED 4   /* 処理の拒否 */

//	ファイル系の関数の動作フラグ
//		上書き時の動作
#define	FLAG_OVERRIDE_DEFAULT	0x00000000	/* デフォルトの動作 */
#define	FLAG_OVERRIDE_CONFIRM	0x00000001	/* 全てについて確認 */
#define	FLAG_OVERRIDE_NOTNEWER	0x00000002	/* 古いファイルのみ上書き */
#define	FLAG_OVERRIDE_FORCED	0x00000004	/* 強制的に上書き */
//		削除時の動作
#define	FLAG_REMOVE_DEFAULT		0x00000000	/* デフォルトの動作 */
#define	FLAG_REMOVE_CONFIRM		0x00000100	/* 全てについて確認 */
#define	FLAG_REMOVE_FORCED		0x00000200	/* 強制的に削除 */
#define FLAG_REMOVE_RECYCLE		0x00000400	/* ごみ箱へ移動 */
//		ディレクトリを再帰的に処理
#define	FLAG_RECURSIVE			0x80000000
//		処理したファイル・フォルダの数を返す
#define	FLAG_RETURNNUM			0x40000000
//		確認が必要な場合は全て "いいえ" を選択
#define FLAG_ANSWERNO			0x20000000

//	属性値フラグ
#define	ATTRIBUTE_FLAG_ARCHIVE		0x00010101	/* アーカイブ */
#define	ATTRIBUTE_FLAG_READONLY		0x00020202	/* 読取専用 */
#define	ATTRIBUTE_FLAG_HIDDEN		0x00040404	/* 隠し */
#define	ATTRIBUTE_FLAG_SYSTEM		0x00080808	/* システム */
#define	ATTRIBUTE_RECURSIVE			0x80000000	/* 再帰処理の実行 */
//	属性値マスク
#define	ATTRIBUTE_MASK_ADD			0x000000FF	/* 追加 */
#define	ATTRIBUTE_MASK_DEL			0x0000FF00	/* 削除 */
#define	ATTRIBUTE_MASK_REP			0x00FF0000	/* 置換 */

//	タイムスタンプ変更フラグ
#define	TOUCH_ACCESSTIME	0x00000001
#define	TOUCH_MODIFYTIME	0x00000002
#define	TOUCH_FILECREATE	0x00000004
#define	TOUCH_RECURSIVE		0x80000000

int	ConfirmOverRide(const PathName &org, const PathName &dest, DWORD flag);
int	ConfirmRemove(const PathName &org, DWORD flag);
int	SafetyCopyFile(const PathName &org, const PathName &dest, DWORD flag);
int	SafetyMoveFile(const PathName &org, const PathName &dest, DWORD flag);
int	SafetyRemoveFile(const PathName &file, DWORD flag);
int	SafetyRemoveDirectory(const PathName &dir, DWORD flag);
int	CopyPath(const PathName &org, const PathName &dest, DWORD flag, SeqOpResult*);
int	MovePath(const PathName &org, const PathName &dest, DWORD flag, SeqOpResult*);
int	RemovePath(const PathName &file, DWORD flag, SeqOpResult*);
int	SetPathAttributes(const PathName &file, DWORD flag, SeqOpResult*);
int	SetPathTime(const PathName &file, const FILETIME *ft, DWORD flag, SeqOpResult*);
int	RecursiveMakeDir(const PathName &dir);

#endif
