//	$Id: optmap.h,v 1.1 2002/04/17 16:23:31 sugiura Exp $
/*
 *	optmap.h
 *	オプション文字と対応するフラグ値を管理するクラス
 */

#ifndef DENGAKUSERIES_CLASSES_OPTMAP
#define DENGAKUSERIES_CLASSES_OPTMAP

#include "strbuf.h"
#include "array.h"

#define OPT_INVALID  ((DWORD)-1)

// オプション文字と対応するフラグ値を管理するクラス
class OptMap {
public:
	// opts  : オプション文字の並び
	// flags : opts の文字に対応するフラグ値の配列へのポインタ
	//         要素数は最低 opts.length() 個必要
	OptMap(const StringBuffer& opts, DWORD* flags)
		: m_opts(opts),
		  m_flags(opts.length())
	{
		m_flags.copy(0, flags, m_opts.length());
	}

	// 文字 ch に対応するフラグ値を返す
	DWORD getFlag(TCHAR ch) const
	{
		int idx = m_opts.find(ch);
		return (idx >= 0) ? m_flags[idx] : OPT_INVALID;
	}

private:
	const StringBuffer m_opts;
	Array<DWORD> m_flags;
};

// デフォルトオプション定義
extern OptMap defOptMap;

// omap に基づいてオプション文字を解析する関数
// ※第１引数の最初の文字は無条件に無視される(オプション指定子とみなす)
BOOL GetFlags(const StringBuffer&, DWORD&, const OptMap& omap = defOptMap);

#endif
