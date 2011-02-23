//	$Id: seq_op.h,v 1.6 2002/04/17 16:23:31 sugiura Exp $
/*
 *	seq_op.h
 *	�����̃t�@�C���E�t�H���_�����ɑ΂��ď��Ԃɏ������s�����߂̏������N���X
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

// SeqOpResult �̗v�f�̐擪�����y�уt�@�C�����Ƃ̃Z�p���[�^
#define SEQ_OP_RESULT_SUCCESS  (TCHAR)'0'
#define SEQ_OP_RESULT_FAILURE  (TCHAR)'1'
#define SEQ_OP_RESULT_CANCEL   (TCHAR)'2'
#define SEQ_OP_RESULT_SEPCHAR  (TCHAR)'|'

// FLAG_RETURNNUM �t���O���w�肵�����́A�X�̌��ʂ�ێ����邽�߂̃��X�g
class SeqOpResult : public LinkList<StringBuffer>,
					public Enumerator<StringBuffer> {
public:
	SeqOpResult() : m_nSuccess(0), m_head(-1) {}

	// �v�f�������Ă��邩�ǂ��� -> Enumerator
	BOOL isValid() const { return this->itemNum() > 0; }

	// �v�f��A���I�Ɏ擾���邽�߂̃��\�b�h -> Enumerator
	BOOL findNext()
	{
		return ++m_head < this->itemNum();
	}
	StringBuffer getValue() const
	{
		if (!this->isValid()) return nullStr;
		return *this->getItemByIndex(m_head);
	}

	int m_nSuccess; // �������������̌����J�E���g����J�E���^

private:
	int m_head; // ���� getValue() �ŕԂ��v�f�̃C���f�b�N�X
};

// inline Add****() �̎���
void AddEnumResult(SeqOpResult*, const StringBuffer&, const StringBuffer&, TCHAR);

// ���������������ʂ�ǉ�����
inline void
AddSuccess(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	if (psor) psor->m_nSuccess++;
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_SUCCESS);
}

// ���s�����������ʂ�ǉ�����
inline void
AddFailure(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_FAILURE);
}

// �L�����Z�����ꂽ�������ʂ�ǉ�����
inline void
AddCancel(
	SeqOpResult* psor,
	const StringBuffer& path,
	const StringBuffer& path2 = nullStr)
{
	AddEnumResult(psor, path, path2, SEQ_OP_RESULT_CANCEL);
}


// �����̃t�@�C���E�t�H���_�����ɑ΂��ĘA���I�ɏ������s�����߂̏������N���X
class SequentialOp {
public:
	SequentialOp(int, CmdLineParser&, DirList&, SeqOpResult* psor);
	virtual ~SequentialOp();

	// ���������s����
	// �����̏����̂����ꂩ�����s������ FALSE, �����łȂ���� TRUE ��Ԃ�
	int doOp();

protected:
	int m_srcIndex; // m_params ���̍ŏ��̃\�[�X�t�@�C���̈ʒu
	CmdLineParser& m_params; // �����̃��X�g
	DirList& m_DirList; // �J�����g�t�H���_�̃��X�g
	SeqOpResult* m_pEnumResult; // �X�̌��ʂ�ۑ����郊�X�g
								// (FLAG_RETURNNUM �t���O�w�莞�̂ݑ��݂���)

	// ���C���h�J�[�h�����̓W�J���ʂ��`�F�b�N���郁�\�b�h
	// FALSE ��Ԃ��Ƃ��̃t�@�C���E�t�H���_�͏������Ȃ����AdoOp() �̖߂�l�ɂ�
	// �e�����Ȃ�
	virtual BOOL precheck_wc(WIN32_FIND_DATA&);

	// ���C���h�J�[�h�łȂ������̐��������`�F�b�N���郁�\�b�h
	// FALSE ��Ԃ��Ƃ��̃t�@�C���E�t�H���_�͏��������AdoOp() �̖߂�l�͕K��
	// FALSE �ɂȂ�
	virtual BOOL precheck_normal(PathName&);

	// ���ۂ̏������s��(�������z)���\�b�h
	// �߂�l�� RO_SUCCESS ���ARO_FAILED, RO_STOP �̃r�b�g�a�̂����ꂩ
	// ��Q������ NULL �łȂ��ꍇ�A�������ʂ� SeqOpResult �ɒǉ�����K�v������
	virtual DWORD do_op(PathName&, SeqOpResult*) = 0;

private:
	// �R�s�[�֎~
	SequentialOp(const SequentialOp&);
	SequentialOp& operator=(const SequentialOp&);
};

// �����̃\�[�X�����ƗB��̃f�X�e�B�l�[�V���������������������ԂɎ��s����
// ���߂̘g�g�݂�񋟂���֐��ƁA���̏��������ۂɍs���֐��̌^��`
typedef BOOL (*PFN_FILETOFILE)(const PathName&, const PathName&, DWORD, SeqOpResult*);
int FileToFileOperation(PFN_FILETOFILE, DWORD, DirList&, CmdLineParser&, SeqOpResult*&);

#endif
