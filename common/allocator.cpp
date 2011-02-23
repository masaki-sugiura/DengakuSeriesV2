//	$Id: allocator.cpp,v 1.1 2001/11/22 13:42:41 sugiura Exp $
/*
 *	allocator.cpp
 *	Allocator �N���X�̎���
 */

/*
	slab allocator

	�������v�[���F
		1) 2^n �̃T�C�Y�̃��������m�ہB
		2) k_min <= k < n �Ȃ� k �ɂ��� 2^k �̃T�C�Y���̃����N���X�g��
		   �p�ӂ���B
		3) 2^(n-1) x 2 �̃������u���b�N�� n-1 �Ԗڂ̃����N���X�g�ɒǉ�����B

	�������A���P�[�g�F
		1) �A���P�[�g�v���T�C�Y���傫���ł��߂� 2^k �� k �����߂�B
		   �A���Ak_min ��菬������� k = k_min �Ƃ���B
		2) k �Ԗڂ̃����N���X�g�Ɋ��ɗv�f������΂�������X�g����O���B
		2')k �Ԗڂ̃����N���X�g�ɗv�f���Ȃ���΁Ak+1 �` n-1 �̓��ōł�������
		   ��łȂ����X�g(m �ԖڂƂ���)����v�f���O���B
		   ���̗v�f�� 2^k x 2, 2^(k+1), ..., 2^(m-1) �̃u���b�N�ɕ������A
		   2^k �̂P�̗v�f�ȊO�������N���X�g�ɂȂ��B
		3) �O�����u���b�N�̃������ւ̃|�C���^��Ԃ��B

	�������f�A���P�[�g�F
		1) �n���ꂽ�|�C���^�ɑΉ����郁�����u���b�N��T���B
		2) 1) �Ō������u���b�N�� 2^k �T�C�Y�̃u���b�N�Ƃ���ƁA���̃u���b�N
		   �ƃy�A�ɂȂ��Ă���u���b�N���g�p����Ă��邩�ǂ����𒲂ׂ�B
		   �����g�p���ł���� k �Ԗڂ̃����N���X�g�ɂ��̂܂܂Ȃ��B
		   �������g�p(�������N���X�g�ɂȂ���Ă���)�ł���΁A�Q�𕹂���
		   2^(k+1) �T�C�Y�̃u���b�N�Ƃ��Ĉ����A2) �̏������J��Ԃ��B

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
		// �ŏ��͕��������ŏ��̃y�[�W���o�^
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

