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
#include "MemoryBlock.h"
#include "FreeLinkedList.h"
#include "MemoryPage.h"

#ifndef __arm__ //ARM architecture
#include <bit>
#endif

namespace internal
{
	#define MIN_BLOCK_SIZE  32U

	/**
	 * @class FreeListArray
	 * @brief Manages memory allocation of free blocks using a free list.
	 *
	 * This class is used for efficient memory management by maintaining a list of free memory blocks.
	 * The memory blocks are organized in an array based on their sizes, allowing for fast allocation and deallocation operations.
	 */
	class alignas(sizeof(size_t)) FreeListArray
	{

	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		FreeListArray() = delete;

		/**
		* @brief Constructs a FreeListArray with the given memory page.
		* @param page The rtsha_page that this FreeListArray should manage.
		* @param min_block_size Minimum block size this free list array can manage.
		* @param page_size Size of the page to be managed.
		*/
		explicit FreeListArray(rtsha_page* page, size_t min_block_size, size_t page_size) noexcept;

		/// @brief Destructor for the FreeListArray.
		~FreeListArray() noexcept
		{
		}

		/**
		* @brief Pushes a memory address onto the appropriate free list.
		*
		* @param address The memory address to be added to the free list.
		*/
		rtsha_attr_inline void push(const size_t data, size_t size) noexcept
		{
			if ((data > _page->start_position) && (data < _page->end_position))
			{


#ifdef __arm__ //ARM architecture
				int32_t index = rsha_bit_width(size) - min_bin;
#else
				int32_t index = std::bit_width(size) - min_bin;
#endif
				
				assert(index >= 0);

				if (index >= 0)
				{
					arrPtrLists[index]->push(data);
				}
			}
		}

		/**
		*@brief Pops and retrieves a memory address from the first appropriate free list.
		*
		* @return The memory address retrieved from the free list.
		*/
		rtsha_attr_inline size_t pop(size_t size) noexcept
		{
			size_t ret(0U);
			#ifdef __arm__ //ARM architecture
			size_t log2_size = rsha_bit_width(size);
			#else
			size_t log2_size = std::bit_width(size);
			#endif
			assert(log2_size >= min_bin);
			if (log2_size >= min_bin)
			{
				for (size_t n = log2_size; n <= max_bin; n++)
				{
					int32_t index = n - min_bin;
					assert(index >= 0);
					if (index >= 0)
					{
						if (false == arrPtrLists[index]->is_empty())
						{
							ret = arrPtrLists[index]->pop();
							if (ret > 0U)
							{
								if ((ret > _page->start_position) && (ret < _page->end_position))
								{
									return ret;
								}
							}
						}
					}
				}
			}
			return 0U;
		}

		/**
		 * @brief Attempts to delete a memory address from the free lists.
		 *
		 * This method searches for a given memory address within the range of managed memory pages.
		 * If found, the address is removed from the free list.
		 *
		 * @param address The memory address to be deleted.
		 * @param block Pointer to the memory block associated with the address.
		 * @param size Size of the memory block.
		 * @return Returns true if the address was found and removed, otherwise false.
		 */
		rtsha_attr_inline bool delete_address(const size_t& address, void* block, const size_t& size) noexcept
		{
			if ((address > _page->start_position) && (address < _page->end_position))
			{
				#ifdef __arm__ //ARM architecture
				size_t log2_size = rsha_bit_width(size);
				#else
				size_t log2_size = std::bit_width(size);
				#endif

				if (log2_size >= min_bin)
				{					
					for (size_t n = log2_size; n <= max_bin; n++)
					{
						int32_t index = n - min_bin;
						assert(index >= 0);
						if (index >= 0)
						{
							if (arrPtrLists[index]->delete_address(address, block))
							{
								return true;
							}
						}
					}
				}
			}
			return false;
		}

	private:

		size_t min_bin;						///< @brief The minimum bin size (in log2 scale).

		size_t max_bin;						///< @brief The maximum bin size (in log2 scale).

		rtsha_page* _page;					///< The memory page being managed by the free list.

		FreeLinkedList* arrPtrLists[MAX_BINS]; ///< @brief Array of pointers to free lists based on block sizes.

		/// @brief Preallocated memory for storing the array of free lists.
		PREALLOC_MEMORY<FreeLinkedList, MAX_BINS> _storage_list_array = 0U;
	};
}
