// $Id: auto_ptr.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	auto_ptr.h
 *	スマートポインタクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_AUTO_PTR
#define	DENGAKUSERIES_CLASSES_AUTO_PTR

#include <windows.h>

// 配列でないオブジェクトへのポインタを扱うスマートポインタ
template<class T>
class Auto_Ptr {
public:
	Auto_Ptr(T* ptr)	//	生のポインタを受け取った場合
		:	m_ptr(ptr),
			m_bOwner(TRUE)	//	ポインタが指すオブジェクトを削除する責任がある
	{}
	Auto_Ptr(Auto_Ptr<T>& aptr)
		:	m_ptr(aptr.m_ptr),
			m_bOwner(aptr.m_bOwner)	//	オーナーフラグの継承
	{
		aptr.m_bOwner = FALSE;	//	代入元の Auto_Ptr オブジェクトは
								//	オーナーではなくなる
	}
	~Auto_Ptr()
	{
		//	自分がオーナーであればオブジェクトを削除
		if (m_bOwner) delete m_ptr;
	}

	Auto_Ptr<T>& operator=(Auto_Ptr<T>& aptr)
	{
		//	自分がオーナーであればオブジェクトを削除
		if (m_bOwner) delete m_ptr;
		m_ptr = aptr.m_ptr;
		m_bOwner = aptr.m_bOwner;	//	オーナーフラグの継承
		aptr.m_bOwner = FALSE;	//	aptr はオーナーではなくなる
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
	T* m_ptr;		//	生のポインタ
	BOOL m_bOwner;	//	オーナーフラグ
};

#endif
