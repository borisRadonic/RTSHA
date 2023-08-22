#pragma once
#include <stdint.h>
#include "MemoryBlock.h"
#include "FreeLinkedList.h"
#include "MemoryPage.h"
#include <bit>

namespace internal
{
	#define MIN_BLOCK_SIZE  32U
	#define MAX_BLOCK_SIZE 0x4000000U
	
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
		explicit FreeListArray(rtsha_page* page, size_t min_block_size, size_t page_size);

		/// @brief Destructor for the FreeListArray.
		~FreeListArray()
		{
		}

		/**
		* @brief Pushes a memory address onto the appropriate free list.
		*
		* @param address The memory address to be added to the free list.
		*/
		rtsha_attr_inline void push(const size_t data, size_t size)
		{
			if ((data > _page->start_position) && (data < _page->end_position))
			{
				int32_t index = std::bit_width(size) - min_bin - 1;
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
		rtsha_attr_inline size_t pop(size_t size)
		{
			size_t ret(0U);
			size_t log2_size = std::bit_width(size);
			assert(log2_size > min_bin);
			if (log2_size > min_bin)
			{				
				for (size_t n = log2_size; n <= max_bin; n++)
				{
					int32_t index = n - min_bin - 1;
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
		rtsha_attr_inline bool delete_address(const size_t& address, void* block, const size_t& size)
		{
			if ((address > _page->start_position) && (address < _page->end_position))
			{
				size_t log2_size = std::bit_width(size);
				if (log2_size >= min_bin)
				{					
					for (size_t n = log2_size; n <= max_bin; n++)
					{
						int32_t index = n - min_bin - 1;
						assert(index >= 0);
						if (index >= 0)
						{
							if (arrPtrLists[n - min_bin - 1]->delete_address(address, block))
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