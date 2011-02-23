//	$Id: tokenizer.h,v 1.3 2002/06/16 14:56:09 sugiura Exp $
/*
 *	tokenizer.h
 *	������̃g�[�N�����N���X
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
	int hasMoreTokens() const
	{
//		return m_bEnd ? 0 : (m_nLength - m_nCurPos + 1);
		return ~((int)(m_bEnd << 31) >> 31) & (m_nLength - m_nCurPos + 1);
	}

private:
	const StringBuffer m_strTokenized;
	StringBuffer m_strSepChars;
	int m_nLength;
	int m_nCurPos;
	BOOL m_bEnd; // �Ōオ sepchar ���� m_nCurPos == m_nLength �ł� has.. �� TRUE
	BOOL m_bSkipNull;

	Tokenizer(const Tokenizer&);
	Tokenizer& operator=(const Tokenizer&);
};

#endif
