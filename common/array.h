//	$Id: array.h,v 1.5 2005-07-04 15:58:21 sugiura Exp $
/*
 *	array.h
 *	(primitive �^����)�z��N���X
 */

#ifndef	DENGAKUSERIES_CLASSES_ARRAY
#define	DENGAKUSERIES_CLASSES_ARRAY

#include <windows.h>

#if 0
// �ŏ��̊��蓖�Ă̒P�ʁi�Q�̗ݏ�j
#define MIN_ALLOC_UNIT  8

inline int
clip(int size)
{
	if (size < 0) size = 0;
	return (size + MIN_ALLOC_UNIT - 1) & ~(MIN_ALLOC_UNIT - 1);
}
#endif

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
		validateRegionParams(head, size);
		::ZeroMemory(m_ptr + head, sizeof(T) * size);
	}
	void copy(int head, const T* src, int size)
	{
		validateRegionParams(head, size);
		::CopyMemory(m_ptr + head, src, sizeof(T) * size);
	}

// �ŏ��̊��蓖�Ă̒P�ʁi�Q�̗ݏ�j
#define MIN_ALLOC_UNIT  8
//	static const int MIN_ALLOC_UNIT = 8;

	static int clip(int size)
	{
		if (size < 0) size = 0;
		return (size + MIN_ALLOC_UNIT - 1) & ~(MIN_ALLOC_UNIT - 1);
	}

private:
	int m_size;
	T*  m_ptr;

	void validateRegionParams(int& head, int& size)
	{
		if (head >= m_size) {
			size = 0;
		} else if (head < 0) {
			head = 0;
		}
		if (size < 0 || head + size > m_size) {
			size = m_size - head;
		}
	}
};

#endif
