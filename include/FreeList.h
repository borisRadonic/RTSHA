#pragma once
#include <stdint.h>
#include "MemoryBlock.h"
#include "InternListAllocator.h"
#include "forward_list"

namespace internal
{
	using namespace std;
	
	using flist = std::forward_list<size_t, InternListAllocator<size_t>>;

	class alignas(sizeof(size_t)) FreeList
	{
	public:

		FreeList() = delete;

		FreeList(rtsha_page* page);
	
		~FreeList()
		{
		}
				
		void push(const size_t address);

		size_t pop();

	private:
		rtsha_page* _page;
		InternListAllocator<std::size_t>* _lallocator;
		flist* ptrLlist;

	private:
		/*reserved for storage on stack*/
		/*only sizeof(size_t) is used by first allocate call after construction
		* 1 x size_of(size_t) should be fine for the first call
		* for all other calls block free space will be used as storage using InternListAllocator
		*/
		size_t _internal_list_storage;
		uint8_t _storage_allocator[sizeof(InternListAllocator<size_t>)];
		uint8_t _storage_list[sizeof(flist)];
	};

	FreeList* createFreeList(rtsha_page* page);
}

