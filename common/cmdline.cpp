//	$Id: cmdline.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	cmdline.cpp
 *	������̃p�[�X�����X�g��
 */

#include <exception>
#include "cmdline.h"

#define	IS_SEPARATOR(ch)	((ch) == ',' || (ch) == ' ' || (ch) == '\t')
#define	SEP_DEFAULT		0
#define	SEP_DBLQUOTE	1
#define	SEP_SGLQUOTE	2

CmdLineParser::CmdLineParser(const StringBuffer& sbCmdLine)
	: m_rawcmdline(sbCmdLine)
{
	// nothing to do more.
}

int
CmdLineParser::delArgv(int i)
{
	if (i < 0) i = itemNum() - 1;
	CmdArgv* pArgv = getItemByIndex(i);
	if (pArgv == NULL) return -1;
	int head = pArgv->m_head;
	int	tail = head + pArgv->m_size;
	CmdArgv* pNext = getItemByIndex(i+1);
	if (pNext != NULL)tail = pNext->m_head;
	StringBuffer last(m_rawcmdline.extract(tail,-1));
	m_rawcmdline.setlength(head);
	m_rawcmdline.append(last);
	int ret = delItemByIndex(i);
	initSequentialGet();
	while ((pArgv = getNextItem()) != 0)
		if (pArgv->m_head >= tail) pArgv->m_head -= tail - head;
	return ret;
}

RealCmdLineParser::RealCmdLineParser(const StringBuffer& sbCmdLine)
	: CmdLineParser(sbCmdLine)
{
	this->mergeArgv(sbCmdLine);
}

int
RealCmdLineParser::mergeArgv(const StringBuffer& sbCmdLine)
{
	int cmdlen = sbCmdLine.length();
	if (cmdlen <= 0) return this->itemNum();

	//	�R�}���h���C���o�b�t�@�ւ̃|�C���^�̐ݒ�
	LPSTR pBuf;
	if (cmdlen >= MAX_CMDLINE) {
		try {
			pBuf = new TCHAR[cmdlen + 4];
		} catch (exception&) {
			return -1;
		}
	} else {
		pBuf = m_workBuffer;	//	�f�t�H���g�̃o�b�t�@
	}

	LPCSTR pstrCmdLine = sbCmdLine;

	//	�R�}���h���C�����p�[�X���ăo�b�t�@�Ɋi�[
	//	�i"str1\0str2\0....strN\0\0"�j
	int septype, head, len;
	LPCSTR pTop = pBuf;

	try {

	for ( ; *pstrCmdLine != '\0'; pstrCmdLine++) {
		//	�擪�̘A�������Z�p���[�^�͔�΂�
		while (*pstrCmdLine != '\0' && IS_SEPARATOR(*pstrCmdLine))
			pstrCmdLine++;
		if (*pstrCmdLine == '\0') break;	//	�R�}���h���C���̏I���
		//	���p���ň͂܂�Ă��邩 and ���̎�ނ����߂�
		if (*pstrCmdLine == '"') {
			pstrCmdLine++;  septype = SEP_DBLQUOTE;	//	�Q�d���p��
		} else if (*pstrCmdLine == '\'') {
			pstrCmdLine++;  septype = SEP_SGLQUOTE;	//	�P�d���p��
		} else {
			septype = SEP_DEFAULT;	//	�󔒂܂��̓^�u
		}
		pTop = pstrCmdLine;
		head = pTop - (LPCSTR)sbCmdLine;
		//	�I�[���p���܂��͋�/�^�u�܂Ń|�C���^���ړ�
		while (*pstrCmdLine != '\0') {
			if ((septype == SEP_DEFAULT && IS_SEPARATOR(*pstrCmdLine))	||
				(septype == SEP_DBLQUOTE && *pstrCmdLine == '"')	||
				(septype == SEP_SGLQUOTE && *pstrCmdLine == '\'')) break;
			pstrCmdLine++;
		}
		len = pstrCmdLine - pTop;
		::CopyMemory(pBuf, pTop, len);
		*(pBuf + len) = '\0';	//	�I�[����
		// head, tail �͈��p�����܂�
		if (septype != SEP_DEFAULT) { head--;  len += 2; }
		this->addItem(new CmdArgv(pBuf, head, len));
		if (*pstrCmdLine == '\0') break;
	}

	} catch (exception&) {
		// nothing to do.
	}

	if (cmdlen >= MAX_CMDLINE) delete [] pBuf;

	return this->itemNum();
}

//	(���������)���p������������
StringBuffer
RemoveQuote(const StringBuffer& str)
{
	if (str.find('"') >= 0 || str.find('\'') >= 0) {
		//	���p���̏����c�ʓ|(^^;�Ȃ�Ŕėp�p�[�T�ɏ�����C��
		return CmdLineParser(str).getArgvStr(0);
	}
	return str;
}
