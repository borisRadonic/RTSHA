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

		void del(const uint64_t key, size_t block);

		size_t find(const uint64_t key);
				
	private:
		rtsha_page* _page;
		mmap_allocator* _mallocator;
		mmap* _ptrMap;
		fmap_get_local_mem* _fnc;

	private:
	
		/*the objects will be instantiated in space reserved here*/
		uint8_t _storage_allocator[sizeof(mmap_allocator)];
		uint8_t _storage_map[sizeof(mmap)];
		uint8_t _storage_fnc[sizeof(fmap_get_local_mem)];

		
	};
}
