// $Id: bitarray.h,v 1.2 2002-01-16 15:57:23 sugiura Exp $
/*
 *	bitarray.h
 *	ビット配列クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_BITARRAY
#define	DENGAKUSERIES_CLASSES_BITARRAY

#include <windows.h>

#define BITARRAY_PERBITS      (sizeof(DWORD) * 8)
#define BITARRAY_DEFAULT_SIZE (BITARRAY_PERBITS * 2)
#define BITARRAY_INDEX(ind)   ((ind) >> 5)
#define BITARRAY_BIT(ind)     (0x1 << ((ind) & 0x1F))
#define BITARRAY_SIZE(size)   ((size) / BITARRAY_PERBITS + (((size)&0x1F) != 0))

class BitArray {
public:
	BitArray(DWORD init_size = BITARRAY_DEFAULT_SIZE)
		: m_size(init_size), m_alloc_size(0), m_bits(NULL)
	{
		this->enlargeSize(init_size);
	}
	BitArray(const BitArray& rhs)
		: m_bits(NULL)
	{
		this->enlargeSize(rhs.m_size);
		::CopyMemory(m_bits, rhs.m_bits, sizeof(DWORD) * m_alloc_size);
	}
	~BitArray()
	{
		delete [] m_bits;
	}
	BitArray& operator=(const BitArray& rhs)
	{
		if (this != &rhs) {
			delete [] m_bits;
			m_bits = NULL;
			this->enlargeSize(rhs.m_size);
			::CopyMemory(m_bits, rhs.m_bits, sizeof(DWORD) * m_alloc_size);
		}
		return *this;
	}

#if 0
	int operator[] (DWORD i) const
	{
		if (i >= m_size) return 0;
		return (m_bits[BITARRAY_INDEX(i)] & BITARRAY_BIT(i)) != 0;
	}

	class BitArrayProxy {
	public:
		BitArrayProxy(BitArray* ba_ptr, DWORD index)
			: m_ba_ptr(ba_ptr), m_index(index)
		{
			// nothing to do.
		}

		BitArrayProxy& operator=(int bit)
		{
			m_ba_ptr->setBit(index, bit);
			return *this;
		}

		operator int() const { return m_ba_ptr->getBit(index); }

	private:
		BitArray* m_ba_ptr;
		DWORD     m_index;
	};

	BitArrayProxy operator[] (DWORD i) { return BitArrayProxy(this, i); }

	friend class BitArrayProxy;
#endif

	DWORD getSize() const { return m_size; }

	void setBit(int index, int bit)
	{
		if (index > m_size) this->enlargeSize(index);
		if (bit)
			m_bits[BITARRAY_INDEX(index)] |= BITARRAY_BIT(index);
		else
			m_bits[BITARRAY_INDEX(index)] &= ~BITARRAY_BIT(index);
	}

	int getBit(int i) const
	{
		if (i >= m_size) return 0;
		return (m_bits[BITARRAY_INDEX(i)] & BITARRAY_BIT(i)) != 0;
	}

	void clear()
	{
		if (m_bits != NULL)
			::ZeroMemory(m_bits, m_alloc_size * sizeof(DWORD));
	}

private:
	DWORD  m_size; // size in bit
	DWORD  m_alloc_size; // size in dword
	DWORD* m_bits;

	void enlargeSize(DWORD size)
	{
		DWORD new_alloc_size = BITARRAY_SIZE(size);
		if (m_bits == NULL) {
			m_bits = new DWORD[new_alloc_size];
			::ZeroMemory(m_bits, new_alloc_size * sizeof(DWORD));
		} else {
			DWORD* newbits = new DWORD[new_alloc_size];
			::CopyMemory(newbits, m_bits, sizeof(DWORD) * m_alloc_size);
			::ZeroMemory(newbits + sizeof(DWORD) * m_alloc_size,
						 sizeof(DWORD) * (new_alloc_size - m_alloc_size));
			delete [] m_bits;
			m_bits = newbits;
		}
		m_alloc_size = new_alloc_size;
		m_size = size;
	}
};

#endif
