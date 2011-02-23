// $Id: pdparser.cpp,v 1.2 2002/04/03 16:13:14 sugiura Exp $
/*
 *	pdparser.cpp
 *	PokeDataParser クラスの実装
 */

#include "pdparser.h"

PokeDataParser::PokeDataParser(const StringBuffer& str)
	:	CmdLineParser(str)
{
	this->mergeArgv(str);
}

int
PokeDataParser::mergeArgv(const StringBuffer& sbCmdLine)
{
	int cmdlen = sbCmdLine.length();
	if (cmdlen <= 0) return this->itemNum();

	//	コマンドラインバッファへのポインタの設定
	LPSTR pBuf;
	if (cmdlen >= MAX_CMDLINE) {
		try {
			pBuf = new TCHAR[cmdlen + 4];
		} catch (...) {
			return -1;
		}
	} else {
		pBuf = m_workBuffer;	//	デフォルトのバッファ
	}

	LPCSTR pstrCmdLine = sbCmdLine;

	::CopyMemory(pBuf,pstrCmdLine,cmdlen + 1);

	LPSTR pTop = pBuf, pEnd = pBuf + cmdlen;
	TCHAR cSep = '\0';
	BOOL isStr = FALSE;
	int num = 1;
	for ( ; pTop < pEnd; pTop++) {
		switch (*pTop) {
		case ',':
			if (!isStr) {
				*pTop = cSep = '\0';
				num++;
			}
			break;
		case '"':
		case '\'':
			if (!isStr) {
				cSep = *pTop;
				isStr = TRUE;
			} else if (*pTop == cSep) {
				isStr = FALSE;
			}
			break;
		}
	}

#define IS_BLANK(ch) ((ch) == ' ' || (ch) == '\t')
#define IS_QUOTE(ch) ((ch) == '"' || (ch) == '\'')

	try {

	int len, head, size;
	for (pTop = pBuf; pTop < pEnd; pTop += len + 1) {
		len = lstrlen(pTop);
		LPSTR pt = pTop, pe = pTop + len - 1;
		if (len > 0) {
			while (IS_BLANK(*pe)) *pe-- = '\0';
			while (pt < pe && IS_BLANK(*pt)) pt++;
		} else pe++;
		head = pt - pBuf;
		size = pe - pt;
		if (IS_QUOTE(*pt)) { pt++;  *pe-- = '\0'; }
		this->addItem(new CmdArgv(pt,head,size));
	}

	} catch (...) {
		// nothing to do.
	}

	if (cmdlen >= MAX_CMDLINE) delete [] pBuf;

	return this->itemNum();
}

