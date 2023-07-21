#include "structures.h"
#include "internal.h"
#include "errors.h"


uintptr_t	_heap_start = 0U;
size_t		_heap_size = 0U;
size_t		_heap_current_position = 0U;
size_t		_heap_top = 0U;

bool		_heap_init = false;

RTSHA_Error _last_heap_error = RTSHA_OK;

rtsha_heap_t* _heap;


rtsha_heap_t* rtsha_heap_init(void* start, size_t size)
{
	rtsha_heap_t* ptrHeap;
	size_t a_size;
	
	uintptr_t a_start = rtsha_align( (uintptr_t) start );
	
	a_size = rtsha_align(size);

	if (a_size == 0U)
	{
		_last_heap_error = RTSHA_ErrorInit;
		return NULL;
	}

	ptrHeap = (rtsha_heap_t*)a_start;
	_heap_start = a_start;
		
	_heap_top = (size_t) _heap_start + a_size;

	_heap_current_position = (size_t)_heap_start;
		
	_heap_size	= a_size;
	
	ptrHeap->number_pages = 0U;
	ptrHeap->pages = NULL;

	_heap_init = true;

	_heap_current_position += sizeof(rtsha_heap_t);

	size_t* ptrMem;
	for ( ptrMem = (size_t*)(void*)_heap_current_position; ptrMem < (size_t*)(void*)_heap_top; ptrMem++ )
	{
		*ptrMem = 0U;
	}
	_last_heap_error = RTSHA_OK;

	_heap = ptrHeap;

	return ptrHeap;
}

rtsha_page* rtsha_add_page(rtsha_heap_t* heap, RTSHA_PageType page_type, size_t size)
{
	size_t a_size = rtsha_align(size);

	size_t int_page_size = 0U;
				
	if (_heap_top < (_heap_current_position + (a_size - sizeof(rtsha_page))) )
	{
		_last_heap_error = RTSHA_ErrorInitOutOfHeap;
		return NULL;
	}
	
	rtsha_page* page = (rtsha_page*) (void*) _heap_current_position;
	page->reserved = 0xFF;

	/*set this page as last page*/
	page->flags = page_type;
	
	page->free = a_size - sizeof(rtsha_page);

	page->free_blocks = 0U;

	page->size = page->free;

	page->last_block = NULL;

	/*set page blocks current possition*/
	page->position = _heap_current_position + sizeof(rtsha_page);

	page->start_position = page->position;

	if (RTSHA_PAGE_TYPE_BIG == page_type)
	{
		if (a_size < RTSHA_PAGE_TYPE_BIG)
		{
			return NULL;
		}					
		page->max_blocks = ((a_size - sizeof(rtsha_page)) / ((sizeof(rtsha_block) + RTSHA_LIST_ITEM_SIZE + page_type)));
		/*create multimap*/
		page->hfree = (uint8_t) multimap_create(page);

	}
	else
	{
		page->max_blocks = ( (a_size - sizeof(rtsha_page)) / ((sizeof(rtsha_block) + RTSHA_LIST_ITEM_SIZE + page_type)));
		/*start for STD vector*/
		//page->reserved_start = page->position + page->max_blocks * (sizeof(rtsha_block) + page_type);
		page->hfree = list_create(page);
		if (page->hfree == RTSHA_FreeListInvalidHandle)
		{
			_last_heap_error = RTSHA_FreeListAllocError;
			return NULL;
		}
		
	}
		
	page->free_blocks = 0U;
	
	_heap_current_position += (sizeof(rtsha_page) + page->size);
	
	page->next= (rtsha_page*)(void*)_heap_current_position;

	if (heap->number_pages == 0U )
	{
		heap->pages = page;
	}

	heap->number_pages = heap->number_pages + 1U;

	return page;
}

size_t rtsha_get_free_space()
{
	if (_heap_current_position >= _heap_top)
	{
		return 0U;
	}	
	return (_heap_top - _heap_current_position);
}