#include "allocator.h"
#include "errors.h"

extern uintptr_t	_heap_start;
extern size_t		_heap_top;
extern RTSHA_Error	_last_heap_error;

static inline size_t rtsha_select_size(rtsha_page* page, size_t size)
{
	if( (page->flags & RTSHA_PAGE_TYPE_16) == RTSHA_PAGE_TYPE_16 )
	{
		return RTSHA_PAGE_TYPE_16;
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
		if (size <= RTSHA_PAGE_TYPE_16)
		{
			page_type = RTSHA_PAGE_TYPE_16;
		}
		else if ((size > RTSHA_PAGE_TYPE_16) && (size <= RTSHA_PAGE_TYPE_32))
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

static void rtsha_free_list_add(rtsha_page* page, rtsha_block* block)
{
	size_t* ptrfree = (size_t*)(_heap_top - ((page->free_blocks + 1U) * sizeof(size_t)));
	*ptrfree = (size_t)block;
	page->free_blocks += 1U;
}

static void rtsha_remove_free_from_list(rtsha_page* page, rtsha_block* block)
{
	rtsha_block* free_block;
	size_t* ptrfree = NULL;
	size_t high;
	size_t low;
	size_t*	ptrH;
	size_t* ptrL;
	size_t count = 0U;
	free_block = NULL;
	high = 0U;
	low = 0U;
	ptrH = NULL;
	ptrL = NULL;
	if (page->free_blocks > 0U)
	{
		ptrfree = (size_t*)(_heap_top - (page->free_blocks * sizeof(size_t)));
		low = (size_t) ptrfree;
		free_block = (rtsha_block*)(*ptrfree);
		/*firs find free block on the list of free blocks*/
		while ((free_block != NULL) && (page->free_blocks > 0U))
		{
			if ((size_t)free_block >= _heap_top)
			{
				break;
			}
			if (block == free_block)
			{
				/*shift all on free list up*/
				page->free_blocks = page->free_blocks - 1U;
				high = (size_t) ptrfree;
				count = (high - low ) >> 2U;
				low = high - sizeof(size_t);

				ptrH = (size_t*) high;
				ptrL = (size_t*) low;
				while (count > 0)
				{
					*ptrH = *ptrL;
					ptrH--;
					ptrL--;
					count--;
				}
				break;
			}

			ptrfree++;
			free_block = (rtsha_block*)(*ptrfree);
		}
	}
}

static inline void* rtsha_allocate_block_at_current_pos(rtsha_page* page, size_t size )
{
	rtsha_block* ptrBlock;
	bool fit;
	void* ret = NULL;
	fit = false;
	
	ptrBlock = NULL;

	ptrBlock = (rtsha_block*)((void*)page->position);
	ptrBlock->size = size;
	ptrBlock->prev = page->last_block;
	
	page->last_block = ptrBlock;
	ptrBlock->size = ptrBlock->size;
	
	(void*)(page->position + size);

	page->position += size;
	page->free = page->free - size - sizeof(size_t);

	ptrBlock++;
	ret = (void*)ptrBlock;
	return ret;
}

static inline void* rtsha_alloc_big_page_block(rtsha_page* page, size_t size)
{
	rtsha_block* ptrBlock;
	rtsha_block* ptrBlockMinDiff;
	size_t compare;
	size_t diff;
	size_t min_diff;
	size_t* ptrfree;
	size_t count;
	bool fit;
	void* ret = NULL;
	ptrBlockMinDiff = NULL;
	count = 0U;

	/*check if any free on the list*/
	ptrfree = (size_t*)(_heap_top - (page->free_blocks * sizeof(size_t)));
	ptrBlock = (rtsha_block*)(*ptrfree);

	diff = 0U;
	fit = false;
	compare = 0U;
	
	if (sizeof(size_t) == 8U)
	{
		min_diff = UINT64_MAX;
	}
	else
	{
		min_diff = UINT32_MAX;
	}
		
	/*try to find perfect-fit method (use only free blocks)*/
	while( (NULL != ptrBlock) && (count < page->free_blocks > 0U) && (ptrBlock->size > 0U) )
	{
		/*only free blocks*/
		if (is_bit(ptrBlock->size, 0))
		{
			if (is_bit(ptrBlock->size, 1))
			{
				compare = size + 3U;
			}
			else
			{
				compare = size + 1U;
			}

			if (ptrBlock->size == compare) /*size with flags*/
			{
				fit = true;
				break;
			}
			else
			{
				/*calculate the difference*/
				if (ptrBlock->size > size)
				{
					diff = ptrBlock->size - size;
					if (diff < size)
					{
						min_diff = diff;
						ptrBlockMinDiff = ptrBlock;
					}
				}
			}
		}
		count++;
		ptrfree++;
		ptrBlock = (rtsha_block*)(*ptrfree);
	}
	if (false == fit)
	{
		ret = rtsha_allocate_block_at_current_pos(page, size);
		if (NULL != ret)
		{
			return ret;
		}

		/*no free space! Use best-fit method based on min-diff*/
		if (NULL == ptrBlockMinDiff)
		{
			return NULL;
		}
		ptrBlock = ptrBlockMinDiff;
	}
	/*set as allocated block*/
	if (ptrBlock != NULL)
	{
		ptrBlock->size = (ptrBlock->size >> 1U) << 1U;

		/*remove from free list*/
		rtsha_remove_free_from_list(page, ptrBlock);

		ptrBlock++;
		ret = (void*)ptrBlock;
		return ret;
	}
	return NULL;
}

static inline void* rtsha_allocate_page_block(rtsha_page* page, size_t size)
{
	rtsha_block* last_free_block;
	size_t min_size;
	size_t* ptrfree;
	void* ret;
	ret = NULL;
	ptrfree = NULL;
	min_size = 0U;
	
	if ((0U == size) || (NULL == page))
	{
		return NULL;
	}
	min_size = sizeof(rtsha_block) + rtsha_select_size(page, size);

	/*check if any free on the list*/
	ptrfree = (size_t*)(_heap_top - (page->free_blocks * sizeof(size_t)));
	last_free_block = (rtsha_block*)(*ptrfree);


	if ((last_free_block != NULL) && (page->free_blocks > 0U))
	{
		/*check if big blocks page*/
		if ((page->flags & RTSHA_PAGE_TYPE_BIG) == RTSHA_PAGE_TYPE_BIG)
		{
			return rtsha_alloc_big_page_block(page, min_size);
		}
		else
		{
			/*use last free block of the same size*/
			last_free_block->size = min_size;
;
			ptrfree++;
			last_free_block = (rtsha_block*)(*ptrfree);


			ret = (void*)last_free_block;
			return ret;
		}
	}
	/*allocate at the current position*/
	return rtsha_allocate_block_at_current_pos(page, min_size);
}

/*return last left free*/
static rtsha_block* rtsha_shrink_left(rtsha_page* page, rtsha_block* block)
{
	rtsha_block* pBlock = block;
	rtsha_block* temp_block;
	rtsha_block* pLastFree = block;

	temp_block = (rtsha_block*)((size_t)((void*)pBlock) + get_block_size(pBlock->size));

	/*merge all free neighbor blocks left together*/
	while ((pBlock->prev != NULL) && (is_bit(pBlock->prev->size, 0)))
	{
		/*left is free -> shrink those two blocks*/
		pBlock->prev->size = get_block_size(pBlock->prev->size);
		pBlock->prev->size += get_block_size(pBlock->size);

		rtsha_remove_free_from_list(page, pBlock);

		pBlock->size = 0U;
		/*set as free*/
		if (pBlock->prev->size > 0U)
		{			
			if (pBlock == page->last_block)
			{
				page->last_block = pBlock->prev;
				/*set as last block*/
				pBlock->prev->size = pBlock->size | 2U;
			}
			pBlock->prev->size = pBlock->prev->size | 1U;
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

	temp_block = (rtsha_block*)((size_t)((void*)pBlock) + get_block_size(pBlock->size));
	
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
			pBlock->size = pBlock->size | 2U;
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
		temp_block->size = 0U;
		if (page->free_blocks >= 1U)
		{
			rtsha_remove_free_from_list(page, temp_block);
		}

		/*set as free*/
		if (pBlock->size > 0U)
		{
			pBlock->size = pBlock->size | 1U;
			
		}
		if (page->last_block == pBlock)
		{
			return pBlock;
		}
		temp_block = (rtsha_block*)((size_t)((void*)pBlock) + get_block_size(pBlock->size));
	}
	return pBlock;
}

static inline void rtsha_free_page_block(rtsha_page* page, void* block)
{
	rtsha_block* block_to_free = (rtsha_block*) block;
			
	page->free = page->free + get_block_size(block_to_free->size);
		
	size_t* ptrfree = (size_t*) (_heap_top - (page->free_blocks * sizeof(size_t)));
	rtsha_block* last_free_block = (rtsha_block*)(*ptrfree);

	if (page->free_blocks == 0U)
	{
		/*set as free*/
		block_to_free->size = block_to_free->size | 1U;		
		rtsha_free_list_add(page, block_to_free);
		return;
	}

	if ( (page->free_blocks > 0U) && (last_free_block != block_to_free) )
	{			
		/*set as free*/
		block_to_free->size = block_to_free->size | 1U;

		if (page->last_block == block_to_free)
		{
			block_to_free->size = block_to_free->size | 2U;
		}
			
		/*add to free list*/
		rtsha_free_list_add(page, block_to_free);

		/*shrink only for big variable size */
		if ((page->flags & RTSHA_PAGE_TYPE_BIG) == RTSHA_PAGE_TYPE_BIG)
		{
			block_to_free = rtsha_shrink_left(page, block_to_free);

			/*check if not last block*/
			if( !is_bit(block_to_free->size, 1) && (page->last_block != block_to_free) )
			{
				block_to_free = rtsha_shrink_right(page, block_to_free);				
			}

			block_to_free = rtsha_shrink_left(page, block_to_free);								
		}
	}
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
	size_t size;
	rtsha_block* block;
	rtsha_page* page;
	
	size = 0U;
	block = (rtsha_block*)ptr;
	block--;

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

void* rtsha_calloc(size_t nmemb, size_t size)
{
	return 0;
}

void* rtsha_realloc(void* ptr, size_t size)
{
	return 0;
}