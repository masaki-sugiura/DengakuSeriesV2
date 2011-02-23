//	$Id: seq_op.h,v 1.6 2002/04/17 16:23:31 sugiura Exp $
/*
 *	seq_op.h
 *	複数のファイル・フォルダ引数に対して順番に処理を行うための純粋基底クラス
 */

#ifndef DENGAKUSERIES_CLASSES_SEQ_OP
#define DENGAKUSERIES_CLASSES_SEQ_OP

#include "strbuf.h"
#include "linklist.h"
#include "auto_ptr.h"
#include "enumerator.h"

class CmdLineParser;
class DirList;
class PathName;

// SeqOpResult の要素の先頭文字及びファイル名とのセパレータ
#define SEQ_OP_RESULT_SUCCESS  (TCHAR)'0'
#define SEQ_OP_RESULT_FAILURE  (TCHAR)'1'
#define SEQ_OP_RESULT_CANCEL   (TCHAR)'2'
#define SEQ_OP_RESULT_SEPCHAR  (TCHAR)'|'

// FLAG_RETURNNUM フラグを指定した時の、個々の結果を保持するためのリスト
class SeqOpResult : public LinkList<StringBuffer>,
					public Enumerator<StringBuffer> {
public:
	SeqOpResult() : m_nSuccess(0), m_head(-1) {}

	// 要素が入っているかどうか -> Enumerator
	BOOL isValid() const { return this->itemNum() > 0; }

	// 要素を連続的に取得するためのメソッド -> Enumerator
	BOOL findNext()
	{
		return ++m_head < this->itemNum();
	}
	StringBuffer getValue() const
	{
		if (!this->isValid()) return nullStr;
		return *this->getItemByIndex(m_head);
	}

	int m_nSuccess; // 成功した処理の個数をカウントするカウンタ

private:
	int m_head; // 次の getValue() で返す要素のインデックス
};

// inline Add****() の実体
void AddEnumResult(SeqOpResult*, const StringBuffer&, const StringBuffer&, TCHAR);

// 成功した処理結果を追加する
inline void
AddSuccess(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	if (psor) psor->m_nSuccess++;
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_SUCCESS);
}

// 失敗した処理結果を追加する
inline void
AddFailure(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_FAILURE);
}

// キャンセルされた処理結果を追加する
inline void
AddCancel(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_CANCEL);
}


// 複数のファイル・フォルダ引数に対して連続的に処理を行うための純粋基底クラス
class SequentialOp {
public:
	SequentialOp(int, CmdLineParser&, DirList&, SeqOpResult* psor);
	virtual ~SequentialOp();

	// 処理を実行する
	// 複数の処理のいずれかが失敗したら FALSE, そうでなければ TRUE を返す
	int doOp();

protected:
	int m_srcIndex; // m_params 中の最初のソースファイルの位置
	CmdLineParser& m_params; // 引数のリスト
	DirList& m_DirList; // カレントフォルダのリスト
	SeqOpResult* m_pEnumResult; // 個々の結果を保存するリスト
								// (FLAG_RETURNNUM フラグ指定時のみ存在する)

	// ワイルドカード引数の展開結果をチェックするメソッド
	// FALSE を返すとそのファイル・フォルダは処理しないが、doOp() の戻り値には
	// 影響しない
	virtual BOOL precheck_wc(WIN32_FIND_DATA&);

	// ワイルドカードでない引数の正当性をチェックするメソッド
	// FALSE を返すとそのファイル・フォルダは処理せず、doOp() の戻り値は必ず
	// FALSE になる
	virtual BOOL precheck_normal(PathName&);

	// 実際の処理を行う(純粋仮想)メソッド
	// 戻り値は RO_SUCCESS か、RO_FAILED, RO_STOP のビット和のいずれか
	// 第２引数が NULL でない場合、処理結果を SeqOpResult に追加する必要がある
	virtual DWORD do_op(PathName&, SeqOpResult*) = 0;

private:
	// コピー禁止
	SequentialOp(const SequentialOp&);
	SequentialOp& operator=(const SequentialOp&);
};

// 複数のソース引数と唯一のデスティネーション引数を持つ処理を順番に実行する
// ための枠組みを提供する関数と、その処理を実際に行う関数の型定義
typedef BOOL (*PFN_FILETOFILE)(const PathName&, const PathName&, DWORD, SeqOpResult*);
int FileToFileOperation(PFN_FILETOFILE, DWORD, DirList&, CmdLineParser&, SeqOpResult*&);

#endif
