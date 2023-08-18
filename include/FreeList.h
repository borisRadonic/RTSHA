#pragma once
#include <stdint.h>
#include "MemoryBlock.h"
#include "InternListAllocator.h"
#include "forward_list"

namespace internal
{
#if not defined USE_STL_LIST
	template<typename T>
	class alignas(sizeof(size_t)) FreeLinkedList
	{
	private:
		struct Node
		{
			T data;
			Node* next;
			explicit Node(size_t address, const T& d, Node* head) : data(d), next(head)
			{
				Node* this_object = reinterpret_cast<Node*>(address);
				*this_object = *this;
			}
		};
		Node* head;

	public:

		explicit FreeLinkedList(rtsha_page* page) : head(NULL), _page(page)
		{
		}

		~FreeLinkedList()
		{
		}

		rtsha_attr_inline void push(const T& address)
		{
			Node newNode(_page->lastFreeBlockAddress, address, head);
			head = reinterpret_cast<Node*>(_page->lastFreeBlockAddress);
		}

		rtsha_attr_inline bool is_empty() const
		{
			return (head == nullptr);
		}

		rtsha_attr_inline T pop()
		{
			T ret(0U);
			if (!is_empty() && head != NULL )
			{
				Node* temp = head;			
				ret = temp->data;
				head = head->next;
			}			
			return ret;
		}
		
	private:
		rtsha_page* _page;									///< The memory page being managed by the free list.
	};
#endif

	using namespace std;
	
#ifdef USE_STL_LIST
	using flist = std::forward_list<size_t, InternListAllocator<size_t>>;
#else
	using flist = FreeLinkedList<size_t>;
#endif

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
		rtsha_attr_inline void push(const size_t address)
		{
#ifdef USE_STL_LIST
			ptrLlist->emplace_front(address);
#else
			ptrLlist->push(address);
#endif
		}

		/**
		*@brief Pops and retrieves a memory address from the free list.
		*
		* @return The memory address retrieved from the free list.
		*/
		rtsha_attr_inline size_t pop()
		{
#ifdef USE_STL_LIST
			flist::iterator ptr = ptrLlist->begin();
			if (ptr != ptrLlist->end())
			{
				size_t address = ptrLlist->front();
				ptrLlist->pop_front();
				return address;
			}
			return 0U;
#else
			return ptrLlist->pop();
#endif
		}

	private:
		rtsha_page* _page;									///< The memory page being managed by the free list.
		
#ifdef USE_STL_LIST
		
		InternListAllocator<std::size_t>* _lallocator;		///< Internal list allocator for storin 'list' memory blocks in 'free blocks' space
#endif
		flist* ptrLlist;									///< Pointer to the list structure used internally.

#ifdef USE_STL_LIST
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
#endif
		/// @brief Reserved storage for an object that will be created using 'placement new' on the stack.
		PREALLOC_MEMORY<flist> _storage_list = 0U;
	};
}

