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
		page->flags = (uint32_t) size_type;
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
			/*we need additional space for out map data... we can not store data in free blocks... to dangerous...not safe...*/

			page->max_blocks = (page->free - 2U * INTERNAL_MAP_STORAGE_SIZE - sizeof(rtsha_page)) / (INTERNAL_MAP_STORAGE_SIZE + 512U + sizeof(rtsha_block));
			page->start_map_data = (page->size + page->start_position - page->max_blocks * (INTERNAL_MAP_STORAGE_SIZE * 2U) ); /*fixed blocks 64 bytes on 32 bit platform*/
			page->map_page = reinterpret_cast<rtsha_page*>(page->start_map_data);

			page->map_page->flags = (uint32_t) rtsha_page_size_type::PageType64;						
			page->map_page->free_blocks = 0U;
			page->map_page->size = page->free - page->start_map_data;
			page->map_page->free = page->map_page->size;
			page->map_page->last_block = NULL;
			page->map_page->start_map_data = 0U;
			page->map_page->map_page = nullptr;
			page->map_page->position = page->start_map_data + sizeof(rtsha_page);

			page->map_page->ptr_list_map = reinterpret_cast<size_t> (reinterpret_cast<void*>(createFreeList(page->map_page)));
			page->ptr_list_map = reinterpret_cast<size_t> (reinterpret_cast<void*>(createFreeMap(page)));
		}
		else
		{
			page->start_map_data = 0U;
			page->map_page = nullptr;
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
				return page;			
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
					return page;
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
					return page;
				}
			}
		}
		return NULL;
	}

	void* Heap::malloc(size_t size)
	{
		if (size > 0U)
		{
			size_t a_size(size);
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
					SmallFixMemoryPage memory_page(page);
					return memory_page.allocate_block(a_size);
				}
				else
				{
					a_size = rtsha_align(a_size);
					BigMemoryPage memory_page(page);					
					return memory_page.allocate_block(a_size);
				}				
			}
		}
		return NULL;
	}

	void Heap::free(void* ptr)
	{
		if (ptr != nullptr)
		{
			size_t address = (size_t)ptr;
			address -= (2U * sizeof(size_t)); /*skip size and pointer to prev*/

			MemoryBlock block((rtsha_block*)(void*)address);

			size_t old_size = block.getSize();

			if (block.isValid())
			{
				size_t size = block.getSize();
				rtsha_page_size_type ideal_page = get_ideal_page(size);
				if (ideal_page != rtsha_page_size_type::PageTypeNotDefined)
				{
					rtsha_page* page = select_page(ideal_page, size);

					if ( (page != nullptr) && (!block.isFree()) )
					{
						if (page->flags == (uint16_t)rtsha_page_size_type::PageTypeBig)
						{
							BigMemoryPage memory_page(page);
							memory_page.free_block(block);
						}
						else
						{
							SmallFixMemoryPage memory_page(page);
							memory_page.free_block(block);
						}
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

		size_t a_size;
		size_t count = 0U;

		a_size = rtsha_align(size);

		ptr_old = (size_t*)ptr;

		if (size == 0U)
		{
			this->free(ptr);
			return NULL;
		}

		size_t address = (size_t)ptr;
		address -= (2U * sizeof(size_t)); /*skip size and pointer to prev*/

		MemoryBlock block((rtsha_block*)(void*)address);


		if (block.getSize() == a_size)
		{
			return ptr;
		}

		/*allocate new memory*/
		new_memory = this->malloc(size);
		ptr_new = (size_t*)new_memory;

		if (NULL != new_memory)
		{
			if (block.getSize() > a_size)
			{
				for (count = 0U; count < block.getSize(); count++)
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

		this->free(ptr);

		return new_memory;
	}

	FreeList* Heap::createFreeList(rtsha_page* page)
	{
		/*create objects on stack in reserved memory using new in place*/
		void* ptrList = _storage_free_lists.get_next_ptr();
		if(ptrList != nullptr)
		{
			return new (ptrList) FreeList(page);
		}
		return nullptr;
	}

	FreeMap* Heap::createFreeMap(rtsha_page* page)
	{
		void* ptrMap = _storage_free_maps.get_next_ptr();
		if (ptrMap != nullptr)
		{
			return new (ptrMap) FreeMap(page);
		}
		return nullptr;
	}
}