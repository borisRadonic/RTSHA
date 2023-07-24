#include "Heap.h"
#include "FreeList.h"
#include "FreeMap.h"

namespace internal
{
	Heap::Heap()
	{
		for (size_t i = 0; i < _pages.size(); i++)
		{
			_pages[i] = nullptr;
		}
	}

	Heap::~Heap()
	{

	}

	bool Heap::init(void* start, size_t size)
	{
		uintptr_t a_start = rtsha_align((uintptr_t)start);
		size_t a_size = rtsha_align(size);

		if (a_size == 0U)
		{
			_last_heap_error = RTSHA_ErrorInit;
			return false;
		}

		_heap_start = a_start;
		_heap_top = (size_t)_heap_start + a_size;
		_heap_current_position = (size_t)_heap_start;
		_heap_size = a_size;
		_number_pages = 0U;
		_heap_init = true;

		/*zero memory*/
		size_t* ptrMem;
		for (ptrMem = (size_t*)(void*)_heap_current_position; ptrMem < (size_t*)(void*)_heap_top; ptrMem++)
		{
			*ptrMem = 0U;
		}
		_last_heap_error = RTSHA_OK;
		return true;
	}

	bool Heap::add_page(rtsha_page_size_type size_type, size_t size)
	{
		size_t a_size = rtsha_align(size);
		if (_heap_top < (_heap_current_position + (a_size - sizeof(rtsha_page))))
		{
			_last_heap_error = RTSHA_ErrorInitOutOfHeap;
			return false;
		}
		rtsha_page* page = (rtsha_page*)(void*)_heap_current_position;
		
		/*set this page as last page*/
		page->flags = (uint16_t) size_type;
		page->free = a_size - sizeof(rtsha_page);
		page->free_blocks = 0U;
		page->size = page->free;
		page->last_block = NULL;

		/*set page blocks current possition*/
		page->position = _heap_current_position + sizeof(rtsha_page);

		page->start_position = page->position;

		if (rtsha_page_size_type::PageTypeBig == size_type)
		{
			if (a_size < (size_t) rtsha_page_size_type::PageType512)
			{
				return false;
			}
			page->ptr_list_map = reinterpret_cast<size_t> (reinterpret_cast<void*>(createFreeMap(page)));
		}
		else
		{
			page->ptr_list_map = reinterpret_cast<size_t> (reinterpret_cast<void*>(createFreeList(page)));
		}
		page->free_blocks = 0U;
		_heap_current_position += (sizeof(rtsha_page) + page->size);
		page->next = (rtsha_page*)(void*)_heap_current_position;
		_pages[_number_pages] = page;
		_number_pages++;
		



		return true;
	}

	size_t Heap::get_free_space() const
	{
		if (_heap_current_position >= _heap_top)
		{
			return 0U;
		}
		return (_heap_top - _heap_current_position);
	}

	rtsha_page_size_type Heap::get_ideal_page(size_t size) const
	{
		if (size > (size_t)rtsha_page_size_type::PageType512)
		{
			return rtsha_page_size_type::PageTypeBig;
		}
		else if (size <= (size_t) rtsha_page_size_type::PageType16)
		{
			return rtsha_page_size_type::PageType16;
		}
		else if ((size > (size_t)rtsha_page_size_type::PageType16) && (size <= (size_t)rtsha_page_size_type::PageType24))
		{
			return rtsha_page_size_type::PageType24;
		}
		else if ((size > (size_t)rtsha_page_size_type::PageType24) && (size <= (size_t)rtsha_page_size_type::PageType32))
		{
			return rtsha_page_size_type::PageType32;
		}
		else if ((size > (size_t)rtsha_page_size_type::PageType32) && (size <= (size_t)rtsha_page_size_type::PageType64))
		{
			return rtsha_page_size_type::PageType64;
		}
		else if ((size > (size_t)rtsha_page_size_type::PageType64) && (size <= (size_t)rtsha_page_size_type::PageType128))
		{
			return rtsha_page_size_type::PageType128;
		}
		else if ((size > (size_t)rtsha_page_size_type::PageType128) && (size <= (size_t)rtsha_page_size_type::PageType256))
		{
			return rtsha_page_size_type::PageType256;
		}
		else if ((size > (size_t)rtsha_page_size_type::PageType256) && (size <= (size_t)rtsha_page_size_type::PageType512))
		{
			return rtsha_page_size_type::PageType512;
		}
		else
		{
			return rtsha_page_size_type::PageTypeNotDefined;
		}
	}

	rtsha_page* Heap::select_page(rtsha_page_size_type ideal_page, size_t size) const
	{
		for (const auto& page : _pages)
		{
			if( (page != nullptr) && (page->flags == (uint16_t) ideal_page))
			{
				if (page->free >= (size + sizeof(rtsha_block)))
				{
					return page;
				}
			}
		}
		/*no ideal page*/
		/*check if big*/
		if (size > (size_t)rtsha_page_size_type::PageType512)
		{
			/*use big page*/
			for (const auto& page : _pages)
			{
				if ( (page != nullptr) && (page->flags == (uint16_t)rtsha_page_size_type::PageTypeBig))
				{
					if (page->free >= (size + sizeof(rtsha_block)))
					{
						return page;
					}
				}
			}
		}
		
		/*try to use first that fits*/
		for (const auto& page : _pages)
		{
			if ( (page != nullptr) && (size < (size_t)page->flags) )
			{
				if (size < (size_t) page->flags)
				{
					if (page->free >= (size + sizeof(rtsha_block)))
					{
						return page;
					}
				}
			}
		}
		return NULL;
	}


	void* Heap::malloc(size_t size)
	{
		if (size > 0U)
		{
			size_t a_size = rtsha_align(size);
			/*we have header and data*/
			a_size += sizeof(rtsha_block);

			rtsha_page_size_type ideal_page = get_ideal_page(a_size);
			if (ideal_page != rtsha_page_size_type::PageTypeNotDefined)
			{
				rtsha_page* page = select_page(ideal_page, a_size);
				if (NULL == page)
				{
					_last_heap_error = RTSHA_NoFreePage;
					return NULL;
				}

				if (page->flags != (uint16_t)rtsha_page_size_type::PageTypeBig)
				{
					a_size = (size_t) page->flags;
				}
				MemoryPage memory_page(page);
				return memory_page.allocate_page_block(a_size);
			}
		}
		return NULL;
	}


	void Heap::free(void* ptr)
	{
		size_t address = (size_t)ptr;
		address -= (2U * sizeof(size_t)); /*skip size and pointer to prev*/
				
		MemoryBlock block((rtsha_block*)(void*)address);
		
		if (block.isValid())
		{
			size_t size = block.getSize();
			rtsha_page_size_type ideal_page = get_ideal_page(size);
			if (ideal_page != rtsha_page_size_type::PageTypeNotDefined)
			{
				rtsha_page* page = select_page(ideal_page, size);
				MemoryPage memory_page(page);
				if (page != NULL)
				{
					if (!block.isFree())
					{
						memory_page.free_block(block);
					}
				}
			}
		}
	}

	void* Heap::calloc(size_t nitems, size_t size)
	{
		return malloc(nitems * size);
	}

	void* Heap::realloc(void* ptr, size_t size)
	{
		void* new_memory;
		size_t* ptr_old;
		size_t* ptr_new;
		rtsha_block* block;
		size_t a_size;
		size_t count = 0U;

		a_size = rtsha_align(size);

		ptr_old = (size_t*)ptr;

		if (size == 0U)
		{
			free(ptr);
			return NULL;
		}

		/*todo!!!!!!!!!!!!!!!!!!!!!!*/

		block = (rtsha_block*)ptr;
		block--;

		if (block->size == a_size)
		{
			return ptr;
		}

		/*todo!!!!!!!!!!!!!!!!!!!!!!*/

		/*allocate new memory*/
		new_memory = malloc(size);
		ptr_new = (size_t*)new_memory;

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

	FreeList* Heap::createFreeList(rtsha_page* page)
	{
		if (_last_list < MAX_PAGES)
		{
			/*create objects on stack in reserved memory using new in place*/
			uint8_t* ptrStack = _storage_free_lists;
			ptrStack += _last_list * sizeof(FreeList);

			FreeList* ptr = new (ptrStack) FreeList(page);
			_last_list++;
			return ptr;
		}
		return nullptr;
	}

	FreeMap* Heap::createFreeMap(rtsha_page* page)
	{
		if (_last_map < MAX_PAGES)
		{
			uint8_t* ptrStack = _storage_free_maps;
			ptrStack += _last_map * sizeof(FreeMap);

			/*create objects on stack in reserved memory using new in place*/
			FreeMap* ptr = new (ptrStack) FreeMap(page);
			_last_map++;
			return ptr;
		}
		return nullptr;
	}


}