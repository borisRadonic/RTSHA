#pragma once
#include <stdint.h>
#include "MemoryBlock.h"
#include "InternListAllocator.h"
#include "FreeLinkedList.h"

namespace internal
{
	using namespace std;
	
	using flist = FreeLinkedList;

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
		rtsha_attr_inline void push(const size_t& address)
		{
			ptrLlist->push(address);
		}

		/**
		*@brief Pops and retrieves a memory address from the free list.
		*
		* @return The memory address retrieved from the free list.
		*/
		rtsha_attr_inline size_t pop()
		{			
			return ptrLlist->pop();
		}

		rtsha_attr_inline bool delete_address(const size_t& address, void* block)
		{
			return  (ptrLlist->delete_address(address, block));
		}

	private:
				
		rtsha_page* _page;									///< The memory page being managed by the free list.
		

		flist* ptrLlist;									///< Pointer to the list structure used internally.

		/// @brief Reserved storage for an object that will be created using 'placement new' on the stack.
		PREALLOC_MEMORY<flist> _storage_list = 0U;
	};
}

