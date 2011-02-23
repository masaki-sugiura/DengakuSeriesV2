//	$Id: optmap.h,v 1.1 2002/04/17 16:23:31 sugiura Exp $
/*
 *	optmap.h
 *	�I�v�V���������ƑΉ�����t���O�l���Ǘ�����N���X
 */

#ifndef DENGAKUSERIES_CLASSES_OPTMAP
#define DENGAKUSERIES_CLASSES_OPTMAP

#include "strbuf.h"
#include "array.h"

#define OPT_INVALID  ((DWORD)-1)

// �I�v�V���������ƑΉ�����t���O�l���Ǘ�����N���X
class OptMap {
public:
	// opts  : �I�v�V���������̕���
	// flags : opts �̕����ɑΉ�����t���O�l�̔z��ւ̃|�C���^
	//         �v�f���͍Œ� opts.length() �K�v
	OptMap(const StringBuffer& opts, DWORD* flags)
		: m_opts(opts),
		  m_flags(opts.length())
	{
		m_flags.copy(0, flags, m_opts.length());
	}

	// ���� ch �ɑΉ�����t���O�l��Ԃ�
	DWORD getFlag(TCHAR ch) const
	{
		int idx = m_opts.find(ch);
		return (idx >= 0) ? m_flags[idx] : OPT_INVALID;
	}

private:
	const StringBuffer m_opts;
	Array<DWORD> m_flags;
};

// �f�t�H���g�I�v�V������`
extern OptMap defOptMap;

// omap �Ɋ�Â��ăI�v�V������������͂���֐�
// ����P�����̍ŏ��̕����͖������ɖ��������(�I�v�V�����w��q�Ƃ݂Ȃ�)
BOOL GetFlags(const StringBuffer&, DWORD&, const OptMap& omap = defOptMap);

#endif
