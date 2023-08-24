/******************************************************************************
The MIT License(MIT)

Real Time Safety Heap Allocator (RTSHA)
https://github.com/borisRadonic/RTSHA

Copyright(c) 2023 Boris Radonic

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "FreeListArray.h"
#include <bitset>

namespace internal
{
	FreeListArray::FreeListArray(rtsha_page* page, size_t min_block_size, size_t page_size) noexcept
		:_page(page)
	{	
		assert(min_block_size >= MIN_BLOCK_SIZE);
		assert(page_size > min_block_size);
#ifdef __arm__ //ARM architecture
				min_bin = rsha_bit_width(min_block_size);
				max_bin = rsha_bit_width(page_size);
#else
				min_bin = std::bit_width(min_block_size);
				max_bin = std::bit_width(page_size);
#endif
		
		assert((max_bin - min_bin) < MAX_BINS);

		for (uint32_t i = 0U; i < MAX_BINS; i++)
		{
			arrPtrLists[i] = nullptr;
		}
		for (uint32_t i = min_bin; i <= max_bin; i++)
		{									
			void* ptrmem = _storage_list_array.get_next_ptr();
			arrPtrLists[i - min_bin] = new( ptrmem) FreeLinkedList(page);
		}
	}
}
