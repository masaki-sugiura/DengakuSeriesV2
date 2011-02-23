//	$Id: allocator.cpp,v 1.1 2001/11/22 13:42:41 sugiura Exp $
/*
 *	allocator.cpp
 *	Allocator クラスの実装
 */

/*
	slab allocator

	メモリプール：
		1) 2^n のサイズのメモリを確保。
		2) k_min <= k < n なる k について 2^k のサイズ毎のリンクリストを
		   用意する。
		3) 2^(n-1) x 2 のメモリブロックを n-1 番目のリンクリストに追加する。

	メモリアロケート：
		1) アロケート要求サイズより大きく最も近い 2^k の k を求める。
		   但し、k_min より小さければ k = k_min とする。
		2) k 番目のリンクリストに既に要素があればそれをリストから外す。
		2')k 番目のリンクリストに要素がなければ、k+1 ～ n-1 の内で最も小さい
		   空でないリスト(m 番目とする)から要素を外す。
		   その要素を 2^k x 2, 2^(k+1), ..., 2^(m-1) のブロックに分割し、
		   2^k の１つの要素以外をリンクリストにつなぐ。
		3) 外したブロックのメモリへのポインタを返す。

	メモリデアロケート：
		1) 渡されたポインタに対応するメモリブロックを探す。
		2) 1) で見つけたブロックを 2^k サイズのブロックとすると、このブロック
		   とペアになっているブロックが使用されているかどうかを調べる。
		   もし使用中であれば k 番目のリンクリストにそのままつなぐ。
		   もし未使用(＝リンクリストにつながれている)であれば、２つを併せて
		   2^(k+1) サイズのブロックとして扱い、2) の処理を繰り返す。

 */

#include "allocator.h"

Allocator::Allocator()
{
}

Allocator::~Allocator()
{
}

void*
Allocator::alloc(int size)
{
}

void
Allocator::free(void* ptr)
{
}

MemPage*
Allocator::createNewPage()
{
	MemPage* ret = this->morecore(sizeof(MemPage) + PAGE_SIZE);
	if (ret != NULL) {
		::ZeroMemory(ret, sizeof(MemPage));
		MemBlock* first = register_latterhalf(ret,
								(MemBlock*)((BYTE*)ret + sizeof(MemPage)),
								PAGE_SIZE_SHIFT - 1);
		// 最初は分割した最初のページも登録
		first->m_next = ret->m_free_blocks[PAGE_SIZE_SHIFT - 1];
		ret->m_free_blocks[PAGE_SIZE_SHIFT - 1] = first;
	}
	return ret;
}

MemBlock*
Allocator::register_latterhalf(MemPage* page, MemBlock* block, DWORD shift)
{
	DWORD size = MIN_BLOCK_SIZE << shift;
	MemBlock* second = (MemBlock*)((BYTE*)block + size);
	block->m_magic = second->m_magic = MEMPAGE_MAGIC;
	block->m_shift = second->m_shift = shift;
	block->m_flags  = FIRST_BLOCK;
	second->m_flags = SECOND_BLOCK;
	second->m_next = page->m_free_blocks[shift];
	page->m_free_blocks[shift] = second;
	return block;
}

BOOL
Allocator::register_block(MemPage* page, MemBlock* block)
{
	if (block->m_magic != MEMBLOCK_MAGIC) return FALSE;

	if (block->m_shift == PAGE_SHIFT_SIZE - 1) {
		block->m_next = page->m_free_blocks[PAGE_SHIFT_SIZE - 1];
		page->m_free_blocks[PAGE_SHIFT_SIZE - 1] = block;
		block->m_flags &= ~MEMBLOCK_USED;
		return TRUE;
	}

	MemBlock* pair;
	if (block->m_flags & FIRST_BLOCK) {
		pair = (MemBlock*)((BYTE*)block + block->m_size);
		if (pair->m_magic != MEMBLOCK_MAGIC) return FALSE;
		if ((pair->m_flags & MEMBLOCK_USED) == 0) {
			// remove pair from link list
			block->m_shift++;
			block->m_flags &= ~MEMBLOCK_USED;
			return TRUE;
		}
	} else {
		pair = (MemBlock*)((BYTE*)block - block->m_size);
		if (pair->m_magic != MEMBLOCK_MAGIC) return FALSE;
		if ((pair->m_flags & MEMBLOCK_USED) == 0) {
		}
	}

}

