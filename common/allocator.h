//	$Id: allocator.h,v 1.1 2001-11-22 13:42:41 sugiura Exp $
/*
 *	allocator.h
 *	slab allocator ƒNƒ‰ƒX
 */

#ifndef	DENGAKUSERIES_CLASSES_ALLOCATOR
#define	DENGAKUSERIES_CLASSES_ALLOCATOR

#include <windows.h>

#define MIN_SIZE_SHIFT 5
#define MIN_BLOCK_SIZE (1 << MIN_SIZE_SHIFT) /* 2^5 */

#define PAGE_SIZE_SHIFT 11 /* 64KB = 2^16 */
#define PAGE_SIZE (MIN_BLOCK_SIZE << PAGE_SIZE_SHIFT)

#define MEMPAGE_MAGIC 0xa5a5a5a5

#define FIRST_BLOCK  0x00
#define SECOND_BLOCK 0x01

#define MEMBLOCK_USED 0x10

typedef struct _MemBlock {
	DWORD m_magic; // magic number = 0xa5a5a5a5
	DWORD m_shift;    // shift size of block
	DWORD m_flags;   // used, etc.
	struct _MemBlock m_next; // next block
	BYTE m_buf[0];
} MemBlock;

typedef struct _MBContainer {
	struct _MBContainer m_next;
	MemBlock* m_mblock;
} MBContainer;

typedef struct _MemPage {
	struct _MemPage* m_next;
	MBContainer* m_free_blocks[MAX_BLOCK_TYPE];
} MemPage;

class Allocator {
public:
	Allocator();
	~Allocator();

	void* alloc(DWORD size);
	void  free(void* ptr);

	virtual void* morecore(DWORD size) = 0; // really allocate memory
	virtual void  freecore(void*) = 0;

protected:

private:
	MemPage* m_pages; // pointers to pages

	MemPage* createNewPage();
	
	MemBlock* register_latterhalf(MemPage*, MemBlock*, DWORD);
	BOOL register_block(MemPage*, MemBlock*);
};

class NoMoreMemoryException {};

#endif
