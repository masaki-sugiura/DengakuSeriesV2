//	$Id: optmap.cpp,v 1.1 2002-04-17 16:23:31 sugiura Exp $
/*
 *	optmap.cpp
 *	OptMap �N���X�̎���
 */

#include "optmap.h"
#include "rec_op.h"

// �f�t�H���g�̃I�v�V���������ƃt���O�l�̃e�[�u��
static DWORD defFlags[] = {
	FLAG_RECURSIVE,	//	�ċA�I�Ɍ���
	FLAG_RETURNNUM,	//	���������t�@�C���E�t�H���_�̐���Ԃ�
	FLAG_OVERRIDE_FORCED | FLAG_REMOVE_FORCED,	//	�����I�ɏ���
	FLAG_OVERRIDE_CONFIRM | FLAG_REMOVE_CONFIRM,	//	�m�F
	FLAG_OVERRIDE_NOTNEWER	//	�V�����t�@�C���E�t�H���_�̂�
};

// �f�t�H���g�̃I�v�V�����̂��߂̐ÓI�I�u�W�F�N�g
OptMap defOptMap("rnfiu", defFlags);

// �I�v�V�����̉�͂��s���֐�
BOOL
GetFlags(const StringBuffer& av, DWORD& fFlags, const OptMap& optMap)
{
	int len = av.length();
	for (int i = 1; i < len; i++) { // �ŏ��̕����̓I�v�V�����w��q
		DWORD fl = optMap.getFlag(av[i]);
		if (fl != OPT_INVALID) fFlags |= fl;
		else return FALSE;
	}
	return TRUE;
}

