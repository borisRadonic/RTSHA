#include "Heap.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "BigMemoryPage.h"
#include "SmallFixMemoryPage.h"
#include "PowerTwoMemoryPage.h"
#include "HeapCallbacks.h"

namespace rtsha
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
		address_t a_start = rtsha_align( reinterpret_cast<address_t>(start) );
		size_t a_size = rtsha_align(size);

		if (a_size == 0U)
		{
			_last_heap_error = RTSHA_ErrorInit;
			return false;
		}
		_heap_start = a_start;
		_heap_top = _heap_start + a_size;

		_heap_current_position = _heap_start;
		_heap_size = a_size;
		_number_pages = 0U;
		_heap_init = true;

		/*zero memory*/
		for (address_t* ptrMem = reinterpret_cast<address_t*>(_heap_current_position); ptrMem < reinterpret_cast<address_t*>(_heap_top); ptrMem++)
		{
			*ptrMem = 0U;
		}
		_last_heap_error = RTSHA_OK;
		return true;
	}

	bool Heap::add_page(HeapCallbacksStruct* callbacks, rtsha_page_size_type size_type, size_t size, size_t max_objects, size_t min_block_size, size_t max_block_size)
	{
		size_t a_size = rtsha_align(size);
		if (_heap_top < (_heap_current_position + (a_size - sizeof(rtsha_page))))
		{
			_last_heap_error = RTSHA_ErrorInitOutOfHeap;
			return false;
		}
				
		rtsha_page* page = reinterpret_cast<rtsha_page*>(_heap_current_position);

		page->callbacks = callbacks;

		/*set this page as last page*/
		page->flags = static_cast<uint32_t>( size_type );

		page->end_position = _heap_current_position + a_size;

		page->free_blocks = 0U;
		
		page->last_block = NULL;

		/*set page blocks current possition*/
		page->position = _heap_current_position + sizeof(rtsha_page);

		page->start_position = page->position;

		if (rtsha_page_size_type::PageTypeBig == size_type)
		{
			if (page->end_position <= (page->position + 64U))
			{
				return false;
			}			
			init_big_block_page(page, a_size, max_objects);
		}
		else if (rtsha_page_size_type::PageTypePowerTwo == size_type)
		{
			if (min_block_size == 0U || max_block_size == 0U)
			{
				return false;
			}
			init_power_two_page(page, a_size, max_objects, min_block_size, max_block_size);
		}
		else
		{
			init_small_fix_page(page, a_size);
		}
		_pages[_number_pages] = page;
		_number_pages++;
		return true;
	}

	void Heap::init_small_fix_page(rtsha_page* page, size_t a_size)
	{
		page->start_map_data = 0U;
		page->map_page = nullptr;
		page->ptr_list_map = reinterpret_cast<size_t> (static_cast<void*>(createFreeList(page)));
		page->free_blocks = 0U;
		_heap_current_position += a_size;
		page->next = reinterpret_cast<rtsha_page*>(_heap_current_position);
	}

	void Heap::init_power_two_page(rtsha_page* page, size_t a_size, size_t max_objects, size_t min_block_size, size_t max_block_size)
	{
		/*we need additional space for out map data... we can not store data in free blocks... to dangerous...not safe...*/
				
		page->min_block_size = std::max(32U, ExpandToPowerOf2(min_block_size));
		page->max_block_size = std::max(64U, ExpandToPowerOf2(max_block_size));

		if (max_objects == 0U)
		{
			//todo: platform 32 and 64 bit
			page->max_blocks = ((a_size - sizeof(rtsha_page)) - 2U * INTERNAL_MAP_STORAGE_SIZE - sizeof(rtsha_page)) / (INTERNAL_MAP_STORAGE_SIZE + 512U + sizeof(rtsha_block));
		}
		else
		{
			page->max_blocks = max_objects;
		}

		//todo: platform 32 and 64 bit
		page->start_map_data = (page->end_position - page->max_blocks * (INTERNAL_MAP_STORAGE_SIZE * 2U)); /*fixed blocks 64 bytes on 32 bit platform*/


		page->map_page = reinterpret_cast<rtsha_page*>(page->start_map_data);

		page->map_page->flags = static_cast<uint32_t>(rtsha_page_size_type::PageType64);
		page->map_page->free_blocks = 0U;

		page->map_page->end_position = page->end_position;

		page->end_position = page->start_map_data;

		page->map_page->last_block = NULL;
		page->map_page->start_map_data = 0U;
		page->map_page->map_page = nullptr;
		page->map_page->position = page->start_map_data + sizeof(rtsha_page);
		page->map_page->start_position = page->start_map_data;

		page->map_page->ptr_list_map = reinterpret_cast<size_t> (reinterpret_cast<void*>(createFreeList(page->map_page)));
		page->ptr_list_map = reinterpret_cast<size_t> (reinterpret_cast<void*>(createFreeMap(page)));
		
		PowerTwoMemoryPage mem_page(page);
		mem_page.createInitialFreeBlocks();
				
		_heap_current_position += a_size;
		page->next = reinterpret_cast<rtsha_page*>(_heap_current_position);
	}

	void Heap::init_big_block_page(rtsha_page* page, size_t a_size, size_t max_objects)
	{
		/*we need additional space for out map data... we can not store data in free blocks... to dangerous...not safe...*/
		if (max_objects == 0U)
		{
			page->max_blocks = ((a_size - sizeof(rtsha_page)) - 2U * INTERNAL_MAP_STORAGE_SIZE - sizeof(rtsha_page)) / (INTERNAL_MAP_STORAGE_SIZE + 512U + sizeof(rtsha_block));
		}
		else
		{
			page->max_blocks = max_objects;
		}
		page->start_map_data = (page->end_position - page->max_blocks * (INTERNAL_MAP_STORAGE_SIZE * 2U)); /*fixed blocks 64 bytes on 32 bit platform*/

		page->map_page = reinterpret_cast<rtsha_page*>(page->start_map_data);

		page->map_page->flags = static_cast<uint32_t>(rtsha_page_size_type::PageType64);
		page->map_page->free_blocks = 0U;

		page->map_page->end_position = page->end_position;

		page->end_position = page->start_map_data;

		page->map_page->last_block = NULL;
		page->map_page->start_map_data = 0U;
		page->map_page->map_page = nullptr;
		page->map_page->position = page->start_map_data + sizeof(rtsha_page);

		page->map_page->ptr_list_map = reinterpret_cast<size_t> (reinterpret_cast<void*>(createFreeList(page->map_page)));
		page->ptr_list_map = reinterpret_cast<size_t> (reinterpret_cast<void*>(createFreeMap(page)));

		_heap_current_position += a_size;

		page->next = reinterpret_cast<rtsha_page*>(_heap_current_position);
				
		BigMemoryPage mem_page(page);
		mem_page.createInitialFreeBlocks();		
	}

	size_t Heap::get_free_space() const
	{
		if (_heap_current_position >= _heap_top)
		{
			return 0U;
		}
		return (static_cast<size_t>(_heap_top - _heap_current_position));
	}

	rtsha_page_size_type Heap::get_ideal_page(size_t size) const
	{
		if (size > (size_t)rtsha_page_size_type::PageType512)
		{
			return rtsha_page_size_type::PageTypeBig;
		}
		
		else if (size <= (size_t) rtsha_page_size_type::PageType32)
		{
			return rtsha_page_size_type::PageType32;
		}		
		else if ((size > (size_t)rtsha_page_size_type::PageType32) && (size <= (size_t)rtsha_page_size_type::PageType32))
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

	rtsha_page* Heap::select_page(rtsha_page_size_type ideal_page, size_t size, bool no_big) const
	{
		if (rtsha_page_size_type::PageTypeNotDefined != ideal_page)
		{
			for (const auto& page : _pages)
			{
				if ((page != nullptr) && (page->flags == static_cast<uint32_t>( ideal_page) ))
				{
					return page;
				}
			}
		}
		/*no ideal page*/
		if (false == no_big)
		{
			/*check if big*/
			if (size > (size_t)rtsha_page_size_type::PageType512)
			{
				/*use big page*/
				for (const auto& page : _pages)
				{
					if ((page != nullptr) && (page->flags == static_cast<uint32_t>( rtsha_page_size_type::PageTypeBig)))
					{
						return page;
					}
				}
			}
		}	

		/*try to use first that fits*/
		for (const auto& page : _pages)
		{
			if ( (page != nullptr) )
			{
				if ( (page->flags != static_cast<uint32_t>( rtsha_page_size_type::PageTypePowerTwo)) &&
					 (page->flags != static_cast<uint32_t>( rtsha_page_size_type::PageTypeBig)) &&
					 (size <= static_cast<size_t>(page->flags)) )
				{
					return page;
				}
				else if (page->flags == static_cast<uint32_t>(rtsha_page_size_type::PageTypePowerTwo))
				{
					return page;
				}
			}
		}
		return NULL;
	}

	rtsha_page* Heap::get_big_memorypage() const
	{
		for (const auto& page : _pages)
		{
			if ((page != nullptr) && (page->flags == static_cast<uint32_t>(rtsha_page_size_type::PageTypeBig)))
			{
				return page;
			}
		}
		return nullptr;
	}

	rtsha_page* Heap::get_block_page(address_t block_address)
	{
		for (const auto& page : _pages)
		{
			if ((page != nullptr))
			{
				if ((block_address >= page->start_position) && (block_address < page->end_position))
				{
					return page;
				}
			}
		}
		return nullptr;
	}

	void* Heap::malloc(size_t size)
	{
		void* ret = NULL;
		if (size > 0U)
		{
			size_t a_size(size);
			/*we have header and data*/
			a_size += sizeof(rtsha_block);
			/*and size2 as control block*/
			a_size += sizeof(size_t);

			if (get_big_memorypage() != nullptr)
			{
				rtsha_page_size_type ideal_page = get_ideal_page(a_size);
				if (ideal_page != rtsha_page_size_type::PageTypeNotDefined)
				{
					rtsha_page* page = select_page(ideal_page, a_size);
					if (NULL == page)
					{
						_last_heap_error = RTSHA_NoFreePage;
						return ret;
					}										

					if (page->flags != static_cast<uint32_t>(rtsha_page_size_type::PageTypeBig))
					{
						a_size = page->flags;
						SmallFixMemoryPage memory_page(page);
						ret = memory_page.allocate_block(a_size);
					}
					else
					{
						a_size = rtsha_align(a_size);
						BigMemoryPage memory_page(page);
						ret = memory_page.allocate_block(a_size);
					}
				}
			}
			else
			{
				a_size = ExpandToPowerOf2(a_size);
				rtsha_page* page = select_page(rtsha_page_size_type::PageTypeNotDefined, a_size, true);
				if (NULL == page)
				{
					_last_heap_error = RTSHA_NoFreePage;
					return ret;
				}
				
				if (page->flags == static_cast<uint32_t>(rtsha_page_size_type::PageTypePowerTwo))
				{
					PowerTwoMemoryPage memory_page(page);
					ret = memory_page.allocate_block(a_size);
				}
				else
				{
					a_size = page->flags;
					SmallFixMemoryPage memory_page(page);
					ret = memory_page.allocate_block(a_size);
				}				
			}
		}
		return ret;
	}

	void Heap::free(void* ptr)
	{
		if (ptr != nullptr)
		{
			size_t address = (size_t)ptr;
			address -= sizeof(rtsha_block); /*skip size and pointer to prev*/
			MemoryBlock block( reinterpret_cast<rtsha_block*>(address) );
			
			if (block.isValid())
			{
				rtsha_page* page = get_block_page(static_cast<address_t>(address));
				
				if ( (page != nullptr) && (!block.isFree()) )
				{
					if (page->flags == static_cast<uint32_t>(rtsha_page_size_type::PageTypeBig))
					{
						BigMemoryPage memory_page(page);
						memory_page.free_block(block);
					}
					else if (page->flags == static_cast<uint32_t>(rtsha_page_size_type::PageTypePowerTwo))
					{
						PowerTwoMemoryPage memory_page(page);
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
		address -= sizeof(rtsha_block); /*skip size and pointer to prev*/

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

	void* Heap::memcpy(void* _Dst, void const* _Src, size_t _Size)
	{
		if ((_Src != nullptr) && (_Dst != nullptr) && (_Size > 0U))
		{
			size_t dst = reinterpret_cast<size_t>(_Dst);
			size_t src = reinterpret_cast<size_t>(_Dst);

			rtsha_page* dstPage = get_block_page(static_cast<address_t>(dst));
			rtsha_page* srcPage = get_block_page(static_cast<address_t>(dst));

			if (dstPage != nullptr)
			{
				/*check destination block*/
				address_t dstAddress = static_cast<address_t>(dst - sizeof(rtsha_block)); /*skip size and pointer to prev*/
				MemoryBlock dstBlock(reinterpret_cast<rtsha_block*>(dstAddress));
				if (!dstBlock.isValid() || (dstBlock.getSize() < _Size) || dstBlock.isFree())
				{
					return nullptr;
				}
			}

			if (srcPage != nullptr)
			{
				/*check source block*/
				address_t srcAddress = static_cast<address_t>(src - sizeof(rtsha_block)); /*skip size and pointer to prev*/
				MemoryBlock srcBlock(reinterpret_cast<rtsha_block*>(srcAddress));
				if (!srcBlock.isValid() || (srcBlock.getSize() < _Size) || srcBlock.isFree() )
				{
					return nullptr;
				}
			}
			return ::memcpy(_Dst, _Src, _Size);
		}
		return nullptr;
	}

	void* Heap::memset(void* _Dst, int _Val, size_t _Size)
	{
		if ((_Dst != nullptr) && (_Size > 0U))
		{
			size_t dst = reinterpret_cast<size_t>(_Dst);
			rtsha_page* dstPage = get_block_page(static_cast<address_t>(dst));

			if (dstPage != nullptr)
			{
				/*check destination block*/
				address_t dstAddress = static_cast<address_t>(dst - sizeof(rtsha_block)); /*skip size and pointer to prev*/
				MemoryBlock dstBlock(reinterpret_cast<rtsha_block*>(dstAddress));
				if (!dstBlock.isValid() || (dstBlock.getSize() < _Size) || dstBlock.isFree())
				{
					return nullptr;
				}
			}
			return ::memset(_Dst, _Val, _Size);
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