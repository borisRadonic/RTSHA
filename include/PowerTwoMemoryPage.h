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
#include "internal.h"
#include "MemoryPage.h"

namespace rtsha
{
	using namespace std;

	/*! \class PowerTwoMemoryPage
	* \brief This class provides various memory handling functions that manipulate MemoryBlock's on 'Power two memory page'
	*
	This is a complex system, which only allows blocks of sizes that are powers of two. This makes merging free blocks back together easier
	and reduces fragmentation. A specialised binary search tree data structures (red-black tree) for fast storage and retrieval of ordered
	information are stored at the end of the page using fixed size Small Fix Memory Page.
	
	This is a fairly efficient method of allocating memory, particularly useful for systems where memory fragmentation is an important concern.
	The algorithm divides memory into partitions to try to minimize fragmentation and the 'Best Fit' algorithm searches the page to find the
	smallest block that is large enough to satisfy the allocation.

	Furthermore, this system is resistant to breakdowns due to its algorithmic approach to allocating and deallocating memory.
	The coalescing operation helps ensure that large contiguous blocks of memory can be reformed after they are freed, reducing 
	the likelihood of fragmentation over time.

	Coalescing relies on having free blocks of the same size available, which is not always the case, and so this system does not
	completely eliminate fragmentation but rather aims to minimize it.
	*/
	class PowerTwoMemoryPage : public MemoryPage
	{
	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		PowerTwoMemoryPage() = delete;

		/**
		* @brief Constructor that initializes the PowerTwoMemoryPage with a given page.
		* @param page The rtsha_page structure to initialize the PowerTwoMemoryPage with.
		*/
		PowerTwoMemoryPage(rtsha_page* page) noexcept : MemoryPage(page)
		{
		}

		/// @brief Virtual destructor for the PowerTwoMemoryPage.
		virtual ~PowerTwoMemoryPage() noexcept
		{
		}

		/*! \fn allocate_block(size_t size)
		* \brief Allocates a memory block of power 2 size.
		*
		* \param size The size of the memory block, in bytes.
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

		/*! \fn createInitialFreeBlocks()
		* \brief This function creates initial free blocks on empty memory page.
		*
		*/
		void createInitialFreeBlocks() noexcept;

	};
}
