// $Id: bitarray.h,v 1.6 2005-07-04 15:58:21 sugiura Exp $
/*
 *	bitarray.h
 *	ビット配列クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_BITARRAY
#define	DENGAKUSERIES_CLASSES_BITARRAY

#include "array.h"

#define BITARRAY_PERBITS      (sizeof(DWORD) * 8)
#define BITARRAY_DEFAULT_SIZE (BITARRAY_PERBITS * 2)
#define BITARRAY_INDEX(ind)   (int)((ind) >> 5)
#define BITARRAY_BIT(ind)     (0x1 << ((ind) & 0x1F))
#define BITARRAY_SIZE(size)   ((size) / BITARRAY_PERBITS + (((size)&0x1F) != 0))

#define BITARRAY_MAX_SIZE  0x400

class BitArray {
public:
	BitArray(int init_size = BITARRAY_DEFAULT_SIZE)
		: m_size(Array<DWORD>::clip(init_size)),
		  m_bits(BITARRAY_SIZE(m_size))
	{
		m_bits.zero();
	}
	// copy ctor and op= can be default

#if 0
	int operator[] (int i) const
	{
		if (i < 0 || i >= m_size) return 0;
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

	BitArrayProxy operator[] (int i) { return BitArrayProxy(this, i); }

	friend class BitArrayProxy;
#endif

	DWORD getSize() const { return m_size; }

	void setBit(int index, int bit)
	{
		if (index < 0 || index >= BITARRAY_MAX_SIZE) return;
		if (index > m_size) {
			int asize = BITARRAY_SIZE(index);
			if (asize > m_bits.size()) {
				int oldsize = m_bits.size();
				m_bits.resize(asize);
				m_bits.zero(oldsize, -1);
			}
			m_size = index;
		}
		if (bit)
			m_bits[BITARRAY_INDEX(index)] |= BITARRAY_BIT(index);
		else
			m_bits[BITARRAY_INDEX(index)] &= ~BITARRAY_BIT(index);
	}

	int getBit(int i) const
	{
		if (i < 0 || i > m_size) return 0;
		return (m_bits[BITARRAY_INDEX(i)] & BITARRAY_BIT(i)) != 0;
	}

	void clear()
	{
		m_bits.zero();
	}

private:
	int  m_size; // size in bit
	Array<DWORD> m_bits;
};

#endif
