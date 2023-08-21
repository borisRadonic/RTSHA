#include "FreeListArray.h"
#include <bitset>

namespace internal
{
	FreeListArray::FreeListArray(rtsha_page* page, size_t min_block_size, size_t page_size)
		:_page(page)
	{	
		assert(min_block_size >= MIN_BLOCK_SIZE);
		assert(page_size <= MAX_BLOCK_SIZE);
		assert(page_size > min_block_size);

		min_bin = std::bit_width(min_block_size);
		max_bin = std::bit_width(page_size);
		
		assert((max_bin - min_bin) < MAX_BINS);

		for (uint32_t i = 0U; i < MAX_BINS; i++)
		{
			arrPtrLists[i] = NULL;
		}
		for (uint32_t i = min_bin; i <= max_bin; i++)
		{									
			void* ptrmem = _storage_list_array.get_next_ptr();
			arrPtrLists[i - min_bin] = new( ptrmem) FreeLinkedList(page);
		}
	}
}
