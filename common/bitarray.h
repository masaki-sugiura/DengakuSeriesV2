// $Id: bitarray.h,v 1.1 2001-11-26 14:43:16 sugiura Exp $
/*
 *	bitarray.h
 *	ビット配列クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_AUTO_PTR
#define	DENGAKUSERIES_CLASSES_AUTO_PTR

#include <windows.h>

#define BITARRAY_PERBITS      (sizeof(DWORD) * 8)
#define BITARRAY_DEFAULT_SIZE (BITARRAY_PERBITS * 2)
#define BITARRAY_INDEX(ind)   ((ind) >> 5)
#define BITARRAY_BIT(ind)     (0x1 << ((ind) & 0x1F))
#define BITARRAY_SIZE(size)   ((size) / BITARRAY_PERBITS + (((size)&0x1F) != 0))

class BitArray {
public:
	BitArray(DWORD init_size = BITARRAY_DEFAULT_SIZE)
		: m_size(BITARRAY_SIZE(init_size)), m_bits(NULL)
	{
		this->enlargeSize(m_size);
	}

	~BitArray()
	{
		delete [] m_bits;
	}

#if 0
	int operator[] (DWORD i) const
	{
		if (BITARRAY_INDEX(i) >= m_size) return 0;
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

	void setBit(int index, int bit)
	{
		DWORD size = BITARRAY_SIZE(index);
		if (size > m_size) {
			if (!bit) return; // 0 returns in default
			this->enlargeSize(size);
		}
		if (bit)
			m_bits[BITARRAY_INDEX(index)] |= BITARRAY_BIT(index);
		else
			m_bits[BITARRAY_INDEX(index)] &= ~BITARRAY_BIT(index);
	}

	int getBit(int i) const
	{
		if (BITARRAY_INDEX(i) >= m_size) return 0;
		return (m_bits[BITARRAY_INDEX(i)] & BITARRAY_BIT(i)) != 0;
	}


private:
	DWORD  m_size;
	DWORD* m_bits;

	void enlargeSize(DWORD size)
	{
		if (m_bits == NULL) {
			m_bits = new DWORD[size];
			::ZeroMemory(m_bits, sizeof(DWORD) * size);
		} else {
			DWORD* newbits = new DWORD[size];
			::CopyMemory(newbits, m_bits, sizeof(DWORD) * m_size);
			::ZeroMemory(newbits + m_size, sizeof(DWORD) * (size - m_size));
			delete [] m_bits;
			m_bits = newbits;
		}
	}
};

#endif
