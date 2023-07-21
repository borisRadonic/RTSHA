#include "allocator.h"
#include "errors.h"

extern uintptr_t	_heap_start;
extern size_t		_heap_top;
extern RTSHA_Error	_last_heap_error;


static inline size_t rtsha_select_size(rtsha_page* page, size_t size)
{
	if( (page->flags & RTSHA_PAGE_TYPE_24) == RTSHA_PAGE_TYPE_24 )
	{
		return RTSHA_PAGE_TYPE_24;
	}
	else if ((page->flags & RTSHA_PAGE_TYPE_32) == RTSHA_PAGE_TYPE_32)
	{
		return RTSHA_PAGE_TYPE_32;
	}
	else if ((page->flags & RTSHA_PAGE_TYPE_64) == RTSHA_PAGE_TYPE_64)
	{
		return RTSHA_PAGE_TYPE_64;
	}
	else if ((page->flags & RTSHA_PAGE_TYPE_128) == RTSHA_PAGE_TYPE_128)
	{
		return RTSHA_PAGE_TYPE_128;
	}
	else if ((page->flags & RTSHA_PAGE_TYPE_256) == RTSHA_PAGE_TYPE_256)
	{
		return RTSHA_PAGE_TYPE_256;
	}
	else if ((page->flags & RTSHA_PAGE_TYPE_512) == RTSHA_PAGE_TYPE_512)
	{
		return RTSHA_PAGE_TYPE_512;
	}	
	return size;
}

static inline rtsha_page* rtsha_select_page(size_t size)
{
	rtsha_heap_t* ptrHeap;
	rtsha_page* page;
	RTSHA_PageType page_type = 0U;
	uint32_t num;
	
	ptrHeap = (rtsha_heap_t*)_heap_start;
	num = 0U;

	page = NULL;
	if (size > 0U)
	{
		if (size <= RTSHA_PAGE_TYPE_32)
		{
			page_type = RTSHA_PAGE_TYPE_32;
		}		
		else if ((size > RTSHA_PAGE_TYPE_32) && (size <= RTSHA_PAGE_TYPE_64))
		{
			page_type = RTSHA_PAGE_TYPE_64;
		}
		else if ((size > RTSHA_PAGE_TYPE_64) && (size <= RTSHA_PAGE_TYPE_128))
		{
			page_type = RTSHA_PAGE_TYPE_128;
		}
		else if ((size > RTSHA_PAGE_TYPE_128) && (size <= RTSHA_PAGE_TYPE_256))
		{
			page_type = RTSHA_PAGE_TYPE_256;
		}
		else if ((size > RTSHA_PAGE_TYPE_256) && (size <= RTSHA_PAGE_TYPE_512))
		{
			page_type = RTSHA_PAGE_TYPE_512;
		}
		else
		{
			page_type = RTSHA_PAGE_TYPE_BIG;
		}
	}
		
	if( (page_type != 0U) && (ptrHeap != NULL) )
	{
		if ((ptrHeap->number_pages == 0U) || (ptrHeap->pages == NULL))
		{
			_last_heap_error = RTSHA_NoPages;
			return NULL;
		}
		page = ptrHeap->pages;
		while( (num < ptrHeap->number_pages) && (page != NULL) )
		{
			if( (page->flags & page_type) == page_type )
			{
				if (page->free >= ( size + sizeof(rtsha_block_struct) ) )
				{
					return page;
				}
			}
			num++;
			page = page->next;
		}
	}
	return NULL;
}

static inline rtsha_page* rtsha_find_page(uintptr_t ptr)
{
	rtsha_heap_t* ptrHeap;
	rtsha_page* page;
	ptrHeap = (rtsha_heap_t*)_heap_start;
	page = NULL;
	if (ptrHeap != NULL )
	{		
		if ((ptrHeap->number_pages == 0U) || (ptrHeap->pages == NULL))
		{
			_last_heap_error = RTSHA_NoPages;
			return NULL;
		}
		page = ptrHeap->pages;		
		while ( (page != NULL) && ( ((uintptr_t) page) < ptr ) )
		{
			if ((ptr > ((uintptr_t)page) )&& (ptr < ((uintptr_t)page->next)) )
			{
				return page;
			}
			page = page->next;
		}
	}
	_last_heap_error = RTSHA_NoPage;
	return NULL;
}

static inline void* rtsha_allocate_block_at_current_pos(rtsha_page* page, size_t size)
{
	rtsha_block* ptrBlock;
	bool fit;
	size_t retAddress;
	void* ret = NULL;
	fit = false;

	ptrBlock = NULL;

	if ( (page->position + size) >= (page->size + page->start_position))
	{
		return NULL;
	}

	ptrBlock = (rtsha_block*)((void*)page->position);
	ptrBlock->size = size;
	ptrBlock->prev = page->last_block;

	
	page->last_block = ptrBlock;
	ptrBlock->size = ptrBlock->size;
	
	(void*)(page->position + size);

	page->position += size;
	page->free = page->free - size;

	ret = (void*)ptrBlock;

	retAddress = (size_t)ret + sizeof(rtsha_block_struct);

	return (void*)retAddress;
}

static inline void* rtsha_try_alloc_page_block(rtsha_page* page, size_t size)
{
	rtsha_free_list_node* pNode;
	rtsha_block* ptrBlock;
	size_t address;
	void* ret;
	ptrBlock = NULL;
	ret = NULL;
	address = list_pop(page->hfree);

	if (address > 0U)
	{		
		ptrBlock = (rtsha_block*)(void*)address;
					
		if (page->free_blocks >= 1U)
		{
			page->free_blocks -= 1U;
		}

		/*set block as allocated*/
		ptrBlock->size = (ptrBlock->size >> 1U) << 1U;
		ret = (void*)((size_t)ptrBlock + 2* sizeof( size_t));

		page->free = page->free - size;
		return ret;
	}
	return rtsha_allocate_block_at_current_pos(page, size);
}

static inline void* rtsha_try_alloc_big_page_block(rtsha_page* page, size_t size)
{
	rtsha_block* pNode, * pNodeTemp;
	size_t address;
	size_t diff, orig_size;
	void* ret;

	if (page->size == page->free)
	{
		multimap_drop_all(page->hfree);
		page->free_blocks = 0U;
		page->position = page->start_position;
	}
	if (page->free_blocks > 0U)
	{
		address = multimap_find(page->hfree, (const uint64_t)size);
		if (address != 0U)
		{
			pNode = (rtsha_block*)(void*)(address);
			orig_size = get_block_size( pNode->size );
			ret = NULL;
			if ( (pNode != NULL) && (get_block_size(pNode->size) >= size) )
			{
				diff = get_block_size(pNode->size) - size;
				diff = rtsha_align(diff);
				if (diff >= MIN_BLOCK_SIZE_FOR_SPLIT)
				{
					/*splitt block*/
					pNode->size = get_block_size(pNode->size) - diff;

					pNodeTemp = (rtsha_block*)(void*) ((size_t) (pNode)) + get_block_size(pNode->size);
					pNodeTemp->prev = pNode;
					pNodeTemp->size = diff;
					/*insert new block to the map of free blocks*/

					page->lastFreeBlockAddress = (size_t)pNodeTemp + sizeof(rtsha_block_struct);
					multimap_insert(page->hfree, (const uint64_t) (size_t) pNodeTemp->size, (size_t) (void*) pNodeTemp);
				}
				/*delete used block from the map of free blocks*/
				
				multimap_delete(page->hfree, (const uint64_t)orig_size, address);
				page->free_blocks -= 1U;

				/*set block as allocated*/
				pNode->size = (pNode->size >> 1U) << 1U;
				ret = (void*)((size_t)pNode + 2*sizeof(size_t));

				page->free = page->free - size;
			}
			return ret;
		}
	}
	return rtsha_allocate_block_at_current_pos(page, size);
}


void* rtsha_allocate_page_block(rtsha_page* page, size_t size)
{
	size_t min_size;
		
	if ((0U == size) || (NULL == page))
	{
		return NULL;
	}
	min_size = sizeof(rtsha_block) + rtsha_select_size(page, size);
	
	if ((page->flags & RTSHA_PAGE_TYPE_BIG) != RTSHA_PAGE_TYPE_BIG)
	{
		return rtsha_try_alloc_page_block(page, min_size);
	}
	//todo: allocate big page block
	return rtsha_try_alloc_big_page_block(page, min_size);
	
}

/*return last left free*/
static rtsha_block* rtsha_shrink_left(rtsha_page* page, rtsha_block* block)
{
	rtsha_block* pBlock = block;
	rtsha_block* temp_block;
	size_t temp_size = 0U;
	rtsha_block* pLastFree = block;

	if (page->last_block != block)
	{
		temp_block = (rtsha_block*)((size_t)((void*)pBlock) + get_block_size(pBlock->size));
	}
	else
	{
		temp_block = NULL;
	}
	

	/*merge all free neighbor blocks left together*/
	while ((pBlock->prev != NULL) && (is_bit(pBlock->prev->size, 0)) && (pBlock->prev != block ) )
	{
		/*left is free -> shrink those two blocks*/
	
		
		temp_size = get_block_size(pBlock->prev->size);

		pBlock->prev->size = get_block_size(pBlock->prev->size);
				
		pBlock->prev->size += get_block_size(pBlock->size);

		/*rempve prev block from map of the free blocks*/
		multimap_delete( page->hfree, (const uint64_t)temp_size, (size_t)pBlock->prev);
		
		if (page->free_blocks >= 1U)
		{
			page->free_blocks -= 1U;
		}
		if (page->free_blocks == 0)
		{
			int a = 0;
			a++;
		}


		/*set as free*/
		if (pBlock->prev->size > 0U)
		{			
			if (pBlock == page->last_block)
			{
				page->last_block = pBlock->prev;
				/*set as last block*/
				set_block_as_last(pBlock->prev);
			}
			
			set_block_as_free(pBlock->prev);

			pLastFree = pBlock->prev;

			/*update previous of the block on the right side*/
			if ((temp_block != NULL) && (temp_block->prev != NULL))
			{
				temp_block->prev = pBlock->prev;
			}
		}
		pBlock = pBlock->prev;
	}
	return pLastFree;
}

static rtsha_block* rtsha_shrink_right(rtsha_page* page, rtsha_block* block)
{
	rtsha_block* pBlock = block;
	rtsha_block* temp_block = NULL;
	rtsha_block* temp_blockR = NULL;

	if (page->last_block != block)
	{
		temp_block = (rtsha_block*)((size_t)((void*)pBlock) + get_block_size(pBlock->size));
	}
	else
	{
		temp_block = NULL;
	}
	
	while ( (temp_block != NULL) && is_bit(temp_block->size, 0) && (temp_block->size > 0U) )
	{
		/*right is free -> merge two blocks*/
		pBlock->size = get_block_size(pBlock->size);
		temp_block->size = get_block_size(temp_block->size);
		pBlock->size = pBlock->size + temp_block->size;

		/*clear right block header data*/
		if (temp_block == page->last_block)
		{
			page->last_block = pBlock;
			/*set as last block*/
			set_block_as_last(pBlock);
		}
		else
		{
			/*set prev of the next right*/
			temp_blockR = (rtsha_block*)((size_t)((void*)temp_block) + get_block_size(temp_block->size));
			if ((temp_blockR != NULL) && (temp_blockR->size > 0U))
			{
				temp_blockR->prev = pBlock;
			}
		}
		
		if (page->free_blocks >= 1U)
		{
			multimap_delete(page->hfree, (const uint64_t)temp_block->size, (size_t)temp_block);
			page->free_blocks -= 1U;
		}
		temp_block->size = 0U;

		/*set as free*/
		if (pBlock->size > 0U)
		{
			set_block_as_free(pBlock);
		}
		if (page->last_block == pBlock)
		{
			return pBlock;
		}
		temp_block = (rtsha_block*)((size_t)((void*)pBlock) + get_block_size(pBlock->size));
	}
	return pBlock;
}

void rtsha_free_page_block(rtsha_page* page, void* block)
{
	rtsha_block* block_to_free = (rtsha_block*) block;
			
	page->free = page->free + get_block_size(block_to_free->size);

	/*set as free*/
	set_block_as_free(block_to_free);

	if (page->last_block == block_to_free)
	{
		set_block_as_last(block_to_free);
	}
	
	/*shrink only for big variable size */
	if ((page->flags & RTSHA_PAGE_TYPE_BIG) == RTSHA_PAGE_TYPE_BIG)
	{
		if (page->free_blocks > 0U)
		{
			block_to_free = rtsha_shrink_left(page, block_to_free);
		}
		if (page->free_blocks > 0U)
		{
			/*check if not last block*/
			if (!is_bit(block_to_free->size, 1U) && (page->last_block != block_to_free))
			{
				block_to_free = rtsha_shrink_right(page, block_to_free);
			}
		}
		if (page->free_blocks > 0U)
		{
			block_to_free = rtsha_shrink_left(page, block_to_free);
		}
		page->lastFreeBlockAddress = (size_t)block_to_free + sizeof(rtsha_block_struct);
		multimap_insert(page->hfree, (const uint64_t) get_block_size(block_to_free->size), (size_t)block_to_free);
		page->free_blocks++;
		return;
	}
	page->free_blocks += 1U;
	
	page->lastFreeBlockAddress = (size_t)block_to_free + sizeof(rtsha_block_struct);
	list_push(page->hfree, (size_t) block_to_free );
}

void* rtsha_malloc(size_t size)
{
	rtsha_page* page;
	size_t a_size = 0U;
	page = NULL;
	if (size > 0U)
	{
		a_size = rtsha_align(size);
		page = rtsha_select_page(a_size);
		if (NULL == page)
		{
			_last_heap_error = RTSHA_NoFreePage;
			return NULL;
		}
		return rtsha_allocate_page_block(page, a_size);
	}
	return NULL;
}

void rtsha_free(void* ptr)
{
	size_t address;
	rtsha_block* block;
	rtsha_page* page;
	
	address = (size_t)ptr;
	address -= (2 * sizeof(size_t)); /*skip size and pointer to prev*/


	block = (rtsha_block*) (void*)address;
	
	page = rtsha_find_page((uintptr_t)ptr);

	if (NULL != page)
	{
		/*check if not already free*/

		if (!is_bit(block->size, 0))
		{
			rtsha_free_page_block(page, (void*)block);
		}		
	}
}

void* rtsha_calloc(size_t nitems, size_t size)
{
	return rtsha_malloc(nitems * size);
}

void* rtsha_realloc(void* ptr, size_t size)
{
	void* new_memory;
	size_t* ptr_old;
	size_t* ptr_new;
	rtsha_block* block;
	size_t a_size;
	size_t count = 0U;

	a_size = rtsha_align(size);

	ptr_old = (size_t*) ptr;

	if (size == 0U)
	{
		rtsha_free(ptr);
		return NULL;
	}

	block = (rtsha_block*)ptr;
	block--;

	if (block->size == a_size)
	{
		return ptr;
	}

	/*allocate new memory*/
	new_memory = rtsha_malloc(size);
	ptr_new = (size_t*) new_memory;
	
	if (NULL != new_memory)
	{
		if (block->size > a_size)
		{
			for (count = 0U; count < block->size; count++)
			{
				*ptr_new = *ptr_old;
				ptr_old++;
				ptr_new++;
			}
		}
		else
		{
			for (count = 0U; count < a_size; count++)
			{
				*ptr_new = *ptr_old;
				ptr_old++;
				ptr_new++;
			}
		}
	}
	return new_memory;
}