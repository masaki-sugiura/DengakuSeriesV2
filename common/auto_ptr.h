// $Id: auto_ptr.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	auto_ptr.h
 *	�X�}�[�g�|�C���^�N���X
 */

#ifndef	DENGAKUSERIES_CLASSES_AUTO_PTR
#define	DENGAKUSERIES_CLASSES_AUTO_PTR

#include <windows.h>

// �z��łȂ��I�u�W�F�N�g�ւ̃|�C���^�������X�}�[�g�|�C���^
template<class T>
class Auto_Ptr {
public:
	Auto_Ptr(T* ptr)	//	���̃|�C���^���󂯎�����ꍇ
		:	m_ptr(ptr),
			m_bOwner(TRUE)	//	�|�C���^���w���I�u�W�F�N�g���폜����ӔC������
	{}
	Auto_Ptr(Auto_Ptr<T>& aptr)
		:	m_ptr(aptr.m_ptr),
			m_bOwner(aptr.m_bOwner)	//	�I�[�i�[�t���O�̌p��
	{
		aptr.m_bOwner = FALSE;	//	������� Auto_Ptr �I�u�W�F�N�g��
								//	�I�[�i�[�ł͂Ȃ��Ȃ�
	}
	~Auto_Ptr()
	{
		//	�������I�[�i�[�ł���΃I�u�W�F�N�g���폜
		if (m_bOwner) delete m_ptr;
	}

	Auto_Ptr<T>& operator=(Auto_Ptr<T>& aptr)
	{
		//	�������I�[�i�[�ł���΃I�u�W�F�N�g���폜
		if (m_bOwner) delete m_ptr;
		m_ptr = aptr.m_ptr;
		m_bOwner = aptr.m_bOwner;	//	�I�[�i�[�t���O�̌p��
		aptr.m_bOwner = FALSE;	//	aptr �̓I�[�i�[�ł͂Ȃ��Ȃ�
		return *this;
	}
	Auto_Ptr<T>& operator=(T* ptr)
	{
		if (m_bOwner) delete m_ptr;
		m_ptr = ptr;
		m_bOwner = TRUE;
		return *this;
	}

	T& operator*() { return *m_ptr; }
	T* operator->() { return m_ptr; }
	T* ptr() { return m_ptr; }

private:
	T* m_ptr;		//	���̃|�C���^
	BOOL m_bOwner;	//	�I�[�i�[�t���O
};

#endif
