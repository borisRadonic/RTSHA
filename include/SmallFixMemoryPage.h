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

#pragma once
#include <stdint.h>
#include "MemoryPage.h"

namespace rtsha
{
	using namespace std;


	/**
	* @class SmallFixMemoryPage
	* @brief This class provides various memory handling functions that manipulate MemoryBlock's on memory page with fixed blocked size.
	* This algorithm is an approach to memory management that is often used in specific situations where objects of a certain size are frequently
	* allocated and deallocated. By using of uses 'Fixed chunk size' algorithm greatly simplies the memory allocation process and reduce fragmentation.
	*
	* The memory is divided into pages of chunks(blocks) of a fixed size (32, 64, 128, 256 and 512 bytes).
	* When an allocation request comes in, it can simply be given one of these blocks.
	* This means that the allocator doesn't have to search through the heap to find a block of the right size, which can improve performance.
	* The free blocks memory is used as 'free list' storage.
	*
	* Deallocations are also straightforward, as the block is added back to the list of available chunks.
	* There's no need to merge adjacent free blocks, as there is with some other allocation strategies, which can also improve performance.
	*
	* However, fixed chunk size allocation is not a good fit for all scenarios. It works best when the majority of allocations are of the same size,
	* or a small number of different sizes. If allocations requests are of widely varying sizes, then this approach can lead to a lot of wasted memory,
	* as small allocations take up an entire chunk, and large allocations require multiple chunks.
	*
	*Small Fix Memory Page is also used internaly by "Power Two Memory Page" and "Big Memory Page" algorithms.
	*
	*
	*/
	class SmallFixMemoryPage : MemoryPage
	{
	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		SmallFixMemoryPage() = delete;

		/**
		* @brief Constructor that initializes the SmallFixMemoryPage with a given page.
		* @param page The rtsha_page structure to initialize the SmallFixMemoryPage with.
		*/
		explicit SmallFixMemoryPage(rtsha_page* page) noexcept : MemoryPage(page)
		{
		}

		/// @brief Virtual destructor for the SmallFixMemoryPage.
		virtual ~SmallFixMemoryPage() noexcept
		{
		}

		/*! \fn allocate_block(size_t size)
		* \brief Allocates a memory block of fixed size.
		*
		* \param size Size of the memory block, in bytes.
		*
		* \return On success, a pointer to the memory block allocated by the function.
		*/
		virtual void* allocate_block(const size_t& size) noexcept final;

		/*! \fn free_block(MemoryBlock& block)
		* \brief This function deallocates memory block.
		*
		* A block of previously allocated memory.
		*
		* \param block Previously allocated memory block.
		*/
		virtual void free_block(MemoryBlock& block) noexcept final;
	};
}
