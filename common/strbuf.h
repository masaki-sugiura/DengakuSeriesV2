//	$Id: strbuf.h,v 1.5 2003-07-06 16:27:46 sugiura Exp $
/*
 *	strbuf.h
 *	������N���X
 */

#ifndef	DENGAKUSERIES_CLASSES_STRBUF
#define	DENGAKUSERIES_CLASSES_STRBUF

#include "array.h"
#include "refcount.h"

#define	BUF_LEN	2

//	������N���X
class StringBuffer {
public:
	StringBuffer(int len = BUF_LEN);
	StringBuffer(LPCSTR str, int len = -1, int exlen = BUF_LEN);
	StringBuffer(const StringBuffer& that)
		: m_sbuf(that.m_sbuf)
	{
		// nothing to do.
	}
	virtual	~StringBuffer();

	//	required in pathname.h
	operator LPCSTR() const { return m_sbuf->m_buf; }
	//	operator	LPSTR();
	LPSTR getBufPtr();

	StringBuffer& operator=(const StringBuffer& rhs)
	{
		if (this != &rhs) m_sbuf = rhs.m_sbuf;
		return *this;
	}
	StringBuffer& operator=(LPCSTR rhs)
	{
		this->StringBuffer::reset(rhs);
		return *this;
	}
	StringBuffer& operator+=(const StringBuffer& rhs)
	{
		this->StringBuffer::append(rhs); // avoid virtual function call
		return *this;
	}

	void setlength(int);	//	�������؂�߂�

	int size() const	//	�o�b�t�@�T�C�Y��Ԃ�
	{
		if (!m_sbuf->isShareable()) m_sbuf->recalc();
		return m_sbuf->m_buf.size() - 1; // NULL �����̈���܂߂Ȃ�
	}

	void resize(int newsize)	//	�o�b�t�@�̃T�C�Y�𑝂₷(���炳�Ȃ�)
	{
		if (!m_sbuf->isShareable()) m_sbuf->recalc();
		this->dup();
		m_sbuf->resize(newsize);
	}

	virtual	void reset(LPCSTR str = NULL, int head = 0, int len = -1);

	void reset(LPCWSTR str, int head = 0, int len = -1);

	int toUnicode(LPWSTR) const;

	//	�w��ꏊ�̕����̕ύX
	void setcharAt(int at = -1, TCHAR ch = '\0');

	//	����(��)�̒ǉ�
	StringBuffer& append(LPCSTR str, int head = 0, int len = -1);
	StringBuffer& append(const StringBuffer& str, int head = 0, int len = -1)
	{ return this->append((LPCSTR)str,head,len); }
	StringBuffer& append(TCHAR);
	StringBuffer& append(int);
	StringBuffer& append(long);
	StringBuffer& append(ULONG);

	//	�w�蕶����̒u������
	StringBuffer& replaceStr(LPCSTR ostr, LPCSTR dstr, int num = -1);

	//	(�Q�o�C�g�������ӎ����Ȃ�)������̔��]
	StringBuffer& reverse();

	//	(�Q�o�C�g�������ӎ�����)������̔��]
	StringBuffer& reverse2();

	//	�Q�o�C�g�������ӎ����Ȃ�������̕ϊ�
	StringBuffer& toLower();
	StringBuffer& toUpper();

	//	�Q�o�C�g�������ӎ����镶����̕ϊ�
	StringBuffer& toLower2();
	StringBuffer& toUpper2();

	// �S�p<->���p�ϊ�
	StringBuffer& toHankaku();
	StringBuffer& toZenkakuHira();
	StringBuffer& toZenkakuKata();

	//	������̔�r
	int compareTo(LPCSTR str, BOOL cs = TRUE, int len = -1) const;

	//	raw-level access to the character
	TCHAR operator[](int at) const
	{
		if (!m_sbuf->isShareable()) m_sbuf->recalc();
		return m_sbuf->m_buf[at];
	}

	//	���ʂ̏��擾���\�b�h
	TCHAR charAt(int at = -1) const;	//	�w��ꏊ�̕�����Ԃ�
	int count(LPCSTR) const;	//	�w�蕶����̏o������Ԃ�
	int find(TCHAR) const;	//	�w�蕶���̍ŏ��̏o���ʒu��Ԃ�
	int rfind(TCHAR) const;	//	�w�蕶���̍Ō�̏o���ʒu��Ԃ�

	//	�Q�o�C�g������F�����Ȃ����擾���\�b�h
	int length() const;			//	������̃o�C�g����Ԃ�
	int find(LPCSTR) const;	//	�w�蕶����̍ŏ��̏o���ʒu��Ԃ�
	int rfind(LPCSTR) const;	//	�w�蕶����̍Ō�̏o���ʒu��Ԃ�
	//	�w��o�C�g���ȍ~�̎w��o�C�g�����̕�����𔲂��o��
	StringBuffer extract(int head, int len = -1) const;

	//	�Q�o�C�g������F��������擾���\�b�h
	int length2() const;		//	������̕�������Ԃ�
	int find2(LPCSTR) const;	//	�w�蕶����̍ŏ��̏o���ʒu��Ԃ�
	int rfind2(LPCSTR) const;	//	�w�蕶����̍Ō�̏o���ʒu��Ԃ�
	//	�w�蕶�����ȍ~�̎w�蕶�������̕�����𔲂��o��
	StringBuffer extract2(int head, int len = -1) const;

protected:
	//	StringBuffer �N���X���狤�L�����o�b�t�@�̎��̃N���X
	struct StringBuffer_rep : public RCObject {
#ifdef _DEBUG
		int m_id;
#endif
//		int m_bufsize;	//	�o�b�t�@�̃T�C�Y
		int m_len;		//	���݊i�[���̕�����̒���
		Array<TCHAR> m_buf;	//	�o�b�t�@�̎���

		StringBuffer_rep(int inisize);
		StringBuffer_rep(LPCSTR str, int len = -1, int exsize = BUF_LEN);
		StringBuffer_rep(const StringBuffer_rep&);
		~StringBuffer_rep();

		void init(LPCSTR,int,int);
		void recalc();
		void resize(int);

		friend class RCPtr<StringBuffer_rep>;
	};

	RCPtr<StringBuffer_rep>	m_sbuf;	//	�o�b�t�@�N���X�ւ̎Q�Ǝ����J�E���g
									//	�X�}�[�g�|�C���^

	void dup();	//	�o�b�t�@�N���X�̕������Q�ƃJ�E���g�̒���
};

extern const StringBuffer nullStr;

#endif

