//	$Id: optmap.cpp,v 1.2 2003/07/06 16:27:46 sugiura Exp $
/*
 *	optmap.cpp
 *	OptMap クラスの実装
 */

#include "optmap.h"
#include "rec_op.h"

// デフォルトのオプション文字とフラグ値のテーブル
static DWORD defFlags[] = {
	FLAG_RECURSIVE,	//	再帰的に検索
	FLAG_RETURNNUM,	//	処理したファイル・フォルダの数を返す
	FLAG_ANSWERNO,	//	全ての確認に自動で No を返答する
	FLAG_OVERRIDE_FORCED | FLAG_REMOVE_FORCED,	//	強制的に処理
	FLAG_OVERRIDE_CONFIRM | FLAG_REMOVE_CONFIRM,	//	確認
	FLAG_OVERRIDE_NOTNEWER	//	新しいファイル・フォルダのみ
};

// デフォルトのオプションのための静的オブジェクト
OptMap defOptMap("rnNfiu", defFlags);

// オプションの解析を行う関数
BOOL
GetFlags(const StringBuffer& av, DWORD& fFlags, const OptMap& optMap)
{
	int len = av.length();
	for (int i = 1; i < len; i++) { // 最初の文字はオプション指定子
		DWORD fl = optMap.getFlag(av[i]);
		if (fl != OPT_INVALID) fFlags |= fl;
		else return FALSE;
	}
	return TRUE;
}

