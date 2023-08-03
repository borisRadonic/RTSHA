#pragma once
#include <stdint.h>
#include "MemoryBlock.h"
#include "InternMapAllocator.h"
#include "map"

namespace internal
{
	using namespace std;

	using mmap_allocator = InternMapAllocator<std::pair<const uint64_t, size_t>>;
	
	using mmap = std::multimap<const uint64_t, size_t, std::less<const uint64_t>, internal::InternMapAllocator<std::pair<const uint64_t, size_t>>>;
			
	class alignas(sizeof(size_t)) FreeMap
	{
	public:

		FreeMap() = delete;

		FreeMap(rtsha_page* page);

		~FreeMap()
		{
		}

		void insert(const uint64_t key, size_t block);

		bool del(const uint64_t key, size_t block);

		size_t find(const uint64_t key);

		bool exists(const uint64_t key, size_t block);

		size_t size() const;
				
	private:
		rtsha_page*			_page;
		mmap_allocator*		_mallocator;
		mmap*				_ptrMap;
		

	private:

		/*reserved storage for object that will be created with 'placement new' on stack*/
		PREALLOC_MEMORY <mmap_allocator>		_storage_allocator = 0U;
		PREALLOC_MEMORY<mmap>					_storage_map = 0U;
	};
}
