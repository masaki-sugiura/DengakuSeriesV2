//	$Id: tokenizer.h,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	tokenizer.h
 *	文字列のトークン化クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_TOKENIZER
#define	DENGAKUSERIES_CLASSES_TOKENIZER

#include "strbuf.h"

class Tokenizer {
public:
	Tokenizer(const StringBuffer& str, const StringBuffer& seps = nullStr,
			BOOL skip_null = FALSE);

	void setSeparatorChars(const StringBuffer& seps);
	const StringBuffer&	getSeparatorChars() const { return m_strSepChars; }

	StringBuffer getNextToken();
	int hasMoreTokens() const { return m_bEnd ? 0 : (m_nLength - m_nCurPos + 1); }

private:
	const StringBuffer m_strTokenized;
	StringBuffer m_strSepChars;
	int m_nLength;
	int m_nCurPos;
	BOOL m_bEnd; // 最後が sepchar だと m_nCurPos == m_nLength でも has.. は TRUE
	BOOL m_bSkipNull;

	Tokenizer(const Tokenizer&);
	Tokenizer& operator=(const Tokenizer&);
};

#endif
