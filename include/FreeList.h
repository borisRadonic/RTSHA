#pragma once
#include <stdint.h>
#include "MemoryBlock.h"
#include "InternListAllocator.h"
#include "forward_list"

namespace internal
{
	using namespace std;
	
	using flist = std::forward_list<size_t, InternListAllocator<size_t>>;

	/**
	* @class FreeList
	* @brief A memory-efficient free list implementation aligned to the size of size_t.
	*
	* This class is designed to manage and recycle the list of free blocks in an efficient manner.
	* Internally, it utilizes a specialized list structure and a custom allocator to manage
	* blocks of memory. Allocator uses the space in unused space of alredy free blocks.
	*/
	class alignas(sizeof(size_t)) FreeList
	{
	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		FreeList() = delete;

		/**
		 * @brief Constructs a FreeList with the given memory page.
		* @param page The rtsha_page that this FreeList should manage.
		*/
		explicit FreeList(rtsha_page* page);
	
		/// @brief Destructor for the FreeList.
		~FreeList()
		{
		}
		
		/**
		* @brief Pushes a memory address onto the free list.
		*
		* @param address The memory address to be added to the free list.
		*/
		void push(const size_t address);

		/**
		*@brief Pops and retrieves a memory address from the free list.
		*
		* @return The memory address retrieved from the free list.
		*/
		size_t pop();

	private:
		rtsha_page* _page;									///< The memory page being managed by the free list.
		InternListAllocator<std::size_t>* _lallocator;		///< Internal list allocator for storin 'list' memory blocks in 'free blocks' space
		flist* ptrLlist;									///< Pointer to the list structure used internally.

	private:
		/**
		* @brief Reserved for storage on the stack.
		*
		* During the first allocation call post-construction, only sizeof(size_t) is used.
		* For subsequent calls, block free space will be used as storage using InternListAllocator.
	 */
		PREALLOC_MEMORY<size_t> _internal_list_storage = 0U;
		
		/// @brief Reserved storage for an object that will be created using 'placement new' on the stack.
		PREALLOC_MEMORY <InternListAllocator<size_t>> _storage_allocator = 0U;
		PREALLOC_MEMORY<flist> _storage_list = 0U;
	};
}

