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
	

	class alignas(sizeof(size_t)) FreeListArray
	{

	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		FreeListArray() = delete;

		/**
		 * @brief Constructs a FreeListArray with the given memory page.
		* @param page The rtsha_page that this FreeListArray should manage.
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
				size_t log2_size = std::bit_width(size);
				assert(log2_size > min_bin);
				if (log2_size > min_bin)
				{
					arrPtrLists[log2_size - min_bin - 1]->push(data);
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
			if (log2_size >= min_bin)
			{
				for (size_t n = log2_size; n <= max_bin; n++)
				{
					if (false == arrPtrLists[n - min_bin - 1]->is_empty())
					{
						ret = arrPtrLists[n - min_bin - 1]->pop();
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
			return 0U;
		}

		rtsha_attr_inline bool delete_address(size_t address, void* block, size_t size)
		{
			if ((address > _page->start_position) && (address < _page->end_position))
			{
				size_t log2_size = std::bit_width(size);
				if (log2_size >= min_bin)
				{
					for (size_t n = log2_size; n <= max_bin; n++)
					{
						if (arrPtrLists[n - min_bin - 1]->delete_address(address, block))
						{
							return true;
						}
					}
				}
			}
			return false;
		}

	private:

		size_t min_bin;

		size_t max_bin;

		rtsha_page* _page;					///< The memory page being managed by the free list.

		FreeLinkedList* arrPtrLists[MAX_BINS];

		PREALLOC_MEMORY<FreeLinkedList, MAX_BINS> _storage_list_array = 0U;
	};
}