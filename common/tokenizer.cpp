//	$Id: tokenizer.cpp,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	tokenizer.cpp
 *	文字列のトークン化クラス
 */

#include "strutils.h"
#include "tokenizer.h"

Tokenizer::Tokenizer(
	const StringBuffer& str,
	const StringBuffer& seps,
	BOOL skip_null)
	:	m_strTokenized(str),
		m_strSepChars(seps),
		m_nLength(m_strTokenized.length()),
		m_nCurPos(0),
		m_bEnd(FALSE),
		m_bSkipNull(skip_null)
{
	//	no operation
}

void
Tokenizer::setSeparatorChars(const StringBuffer& seps)
{
	m_strSepChars = seps;
}

StringBuffer
Tokenizer::getNextToken()
{
	if (m_bEnd) return nullStr;

	int	seplen = m_strSepChars.length(),
		oldpos = m_nCurPos;
	if (seplen >= 0) {
		for (int nextpos = m_nCurPos; nextpos < m_nLength; nextpos++) {
			TCHAR ch = m_strTokenized[nextpos];
			if (IsCharLeadByte(ch)) {
				//	２バイト文字
				TCHAR buf[3];
				buf[0] = ch;
				buf[1] = m_strTokenized[++nextpos];
				buf[2] = '\0';
				if (m_strSepChars.find(buf) >= 0) {
					m_nCurPos = ++nextpos;
					if (m_bSkipNull && nextpos - oldpos - 2 == 0) {
						oldpos += 2;
						continue;
					}
					return m_strTokenized.extract(oldpos,(nextpos-oldpos)-2);
				}
			} else {
				//	１バイト文字
				if (m_strSepChars.find(ch) >= 0) {
					m_nCurPos = ++nextpos;
					if (m_bSkipNull && nextpos - oldpos - 1 == 0) {
						oldpos++;
						continue;
					}
					return m_strTokenized.extract(oldpos,(nextpos-oldpos)-1);
				}
			}
		}
	}
	// ここに来ると言う事はもう文字列は残っていないはず
	m_bEnd = TRUE;
	m_nCurPos = m_nLength;
	return m_strTokenized.extract(oldpos);
}

