//	$Id: array.h,v 1.4 2003-07-06 16:27:46 sugiura Exp $
/*
 *	array.h
 *	(primitive 型限定)配列クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_ARRAY
#define	DENGAKUSERIES_CLASSES_ARRAY

#include <windows.h>

// 最小の割り当ての単位（２の累乗）
#define MIN_ALLOC_UNIT  8

inline int
clip(int size)
{
	if (size < 0) size = 0;
	return (size + MIN_ALLOC_UNIT - 1) & ~(MIN_ALLOC_UNIT - 1);
}

template<class T>
class Array {
public:
	Array(int size)
		: m_size(clip(size))
	{
		m_ptr = new T[m_size];
	}
	~Array()
	{
		delete [] m_ptr;
	}
	Array(const Array<T>& rhs)
		: m_size(rhs.m_size)
	{
		m_ptr = new T[m_size];
		::CopyMemory(m_ptr, rhs.m_ptr, sizeof(T) * m_size);
	}
	Array<T>& operator=(const Array<T>& rhs)
	{
		if (this != &rhs) {
			delete [] m_ptr;
			m_ptr = new T[rhs.m_size];
			m_size = rhs.m_size;
			::CopyMemory(m_ptr, rhs.m_ptr, sizeof(T) * m_size);
		}
		return *this;
	}

	operator T*() const { return m_ptr; }
	T& operator[] (int i) { return m_ptr[i]; }
	const T& operator[] (int i) const { return m_ptr[i]; }

	int size() const { return m_size; }
	void resize(int newsize)
	{
		if (newsize > m_size) {
			newsize = clip(newsize);
			T* newptr = new T[newsize];
			::CopyMemory(newptr, m_ptr, sizeof(T) * m_size);
			delete [] m_ptr;
			m_ptr  = newptr;
			m_size = newsize;
		}
	}
	void zero(int head = 0, int size = -1)
	{
		if (size < 0) size = m_size - head;
		::ZeroMemory(m_ptr + head, sizeof(T) * size);
	}
	void copy(int head, const T* src, int size)
	{
		::CopyMemory(m_ptr + head, src, sizeof(T) * size);
	}

private:
	int m_size;
	T*  m_ptr;
};

#endif
