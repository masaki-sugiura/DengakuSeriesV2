//	$Id: block_alloc.h,v 1.1 2002-01-16 15:57:23 sugiura Exp $
/*
 *	block_alloc.h
 *	あとでまとめて delete してもいいオブジェクトのための
 *	アロケーションクラス
 */

#ifndef DENGAKUSERIES_CLASSES_BLOCK_ALLOC
#define DENGAKUSERIES_CLASSES_BLOCK_ALLOC

#include "stack.h"
#include "auto_ptr.h"

template<class T>
class BlockAllocator {
public:
	BlockAllocator(UINT blocks)
		: m_blocks(blocks), m_curIndex(0), m_curBlock(NULL),
		  m_stBlocks(32)
	{}

	~BlockAllocator()
	{
		BYTE* ptr;
		while ((ptr = m_stBlocks.pop()) != NULL) {
			for (UINT i = 0; i < m_blocks; i++) {
				// call dtor manually
				((T*)ptr + i)->~T();
			}
			delete [] ptr;
		}
	}

	//	このメソッドで返した領域に対して placement new を呼ぶ
	T* allocate()
	{
		if (m_curBlock == NULL || m_curIndex >= m_blocks) {
			m_curBlock = (T*)(new BYTE[sizeof(T) * m_blocks]);
			m_stBlocks.push((BYTE*)m_curBlock);
			m_curIndex = 0;
		}
		return m_curBlock + m_curIndex++;
	}

private:
	UINT m_blocks;
	UINT m_curIndex;
	T* m_curBlock;
	Stack<BYTE> m_stBlocks;

	BlockAllocator(const BlockAllocator<T>&);
	BlockAllocator<T>& operator=(const BlockAllocator<T>&);
};

#endif
