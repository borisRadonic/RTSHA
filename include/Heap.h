#pragma once
#include <stdint.h>
#include <array>
#include "MemoryPage.h"
#include "structures.h"
#include "errors.h"
#include "FreeList.h"
#include "FreeMap.h"

namespace rtsha
{
	using namespace std;
	using namespace internal;

	class Heap
	{
	public:

		Heap();

		~Heap();

		bool init(void* start, size_t size);

		bool add_page(HeapCallbacksStruct* callbacks, rtsha_page_size_type size_type, size_t size, size_t max_objects = 0U, size_t min_block_size = 0U, size_t max_block_size = 0U);
		
		size_t get_free_space() const;
		
		rtsha_page* get_big_memorypage() const;

		rtsha_page* get_block_page(address_t block_address);
				
		void* malloc(size_t size);

		void free(void* ptr);

		void* calloc(size_t nitems, size_t size);
		
		void* realloc(void* ptr, size_t size);

		rtsha_page_size_type get_ideal_page(size_t size) const;

		rtsha_page* select_page(rtsha_page_size_type ideal_page, size_t size, bool no_big = false) const;

		FreeList* createFreeList(rtsha_page* page);
		
		FreeMap* createFreeMap(rtsha_page* page);

	protected:
		size_t		_number_pages = 0U;
		address_t	_heap_start = NULL;
		size_t		_heap_size = 0U;
		address_t	_heap_current_position = NULL;
		address_t	_heap_top = NULL;
		bool		_heap_init = false;
		RTSHA_Error _last_heap_error = RTSHA_OK;
				
		std::array<rtsha_page*, MAX_PAGES>	_pages; 
		
	
	private:
		void init_small_fix_page(rtsha_page* page, size_t a_size);
		void init_power_two_page(rtsha_page* page, size_t a_size, size_t max_objects, size_t min_block_size, size_t max_block_size);
		void init_big_block_page(rtsha_page* page, size_t a_size, size_t max_objects);

	private:
		/*bytes on stack to store map and list objects created with new inplace*/
		PREALLOC_MEMORY<FreeList,	(MAX_SMALL_PAGES + MAX_BIG_PAGES)>	_storage_free_lists = 0U;
		PREALLOC_MEMORY<FreeMap,	MAX_BIG_PAGES>		_storage_free_maps = 0U;
	};
}

