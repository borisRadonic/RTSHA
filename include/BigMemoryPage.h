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
	* @class BigMemoryPage
	* @brief This class provides various memory handling functions that manipulate MemoryBlock's on 'big memory page'
	*
	*Similar to the 'Power Two Memory Page', this algorithm employs the 'Best Fit' algorithm, in conjunction with a 'Red-Black' balanced tree,
	*which offers worst-case guarantees for insertion, deletion, and search times. 
	*It promptly merges or coalesces memory blocks larger than 'MIN_BLOCK_SIZE_FOR_SPLIT' bytes after they are released.
	*/
	class BigMemoryPage : public MemoryPage
	{
	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		BigMemoryPage() = delete;

		/**
		* @brief Constructor that initializes the BigMemoryPage with a given page.
		* @param page The rtsha_page structure to initialize the BigMemoryPage with.
		*/
		explicit BigMemoryPage(rtsha_page* page) : MemoryPage(page)
		{
		}

		/// @brief Virtual destructor for the BigMemoryPage.
		virtual ~BigMemoryPage()
		{
		}

		/*! \fn allocate_block(size_t size)
		* \brief Allocates a block of memory of the specified size.
		*
		* \param size The size of the memory block to allocate.
		*
		* \return On success, a pointer to the memory block allocated by the function.
		*/
		virtual void* allocate_block(const size_t& size) noexcept final;

		/**
		* @brief Frees the specified memory block.
		* @param block The memory block to be freed.
		*/
		virtual void free_block(MemoryBlock& block) noexcept final;

		/**
		* @brief Creates the initial first two free blocks within the page.
		*/
		void createInitialFreeBlocks() noexcept;

	protected:
		
		/**
		* @brief Splits a memory block based on the specified size.
		* @param block The block to be split.
		* @param size The desired size of the block after splitting.
		*/
		void splitBlock(MemoryBlock& block, size_t size)  noexcept;

		/**
		* @brief Merges the specified block with its left neighbor.
		* @param block The block to be merged with its left neighbor.
		*/
		void mergeLeft(MemoryBlock& block) noexcept;

		/**
		* @brief Merges the specified block with its right neighbor.
		* @param block The block to be merged with its right neighbor.
		*/
		void mergeRight(MemoryBlock& block)  noexcept;
	};
}
