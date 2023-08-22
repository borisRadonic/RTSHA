#pragma once
#include <stdint.h>
#include "internal.h"
#include "MemoryPage.h"
#include <bitset>

namespace internal
{
	using namespace rtsha;
	
	/**
	* @class FreeLinkedList
	* @brief Implements a doubly linked list for managing free blocks of memory.
	*
	* This class provides basic operations like push, pop, and delete for managing free blocks
	* of memory in a specific memory page. The nodes of the list are aligned to the size of size_t
	* for performance and memory layout reasons.
	*/
	class alignas(sizeof(size_t))  FreeLinkedList
	{
		/**
		* @struct Node
		* @brief A node in the FreeLinkedList.
		*
		* This node contains a data representing the memory block address and pointers to the
		* previous and next nodes in the list.
		*/
		struct alignas(sizeof(size_t)) Node
		{
			size_t data;
			Node* prev;
			Node* next;

			/**
			* @brief Constructs a new Node.
			* @param data The memory block address.
			* @param head Pointer to the next node.
			* @param prev Pointer to the previous node.
			*/
			explicit Node(const size_t& data, Node* head, Node* prev = nullptr) : data(data), next(head), prev(prev)
			{
			};
		};
		
	public:

		/**
		* @brief Constructs a FreeLinkedList with the given memory page.
		* @param page The rtsha_page that this FreeLinkedList should manage.
		*/
		explicit FreeLinkedList(rtsha_page* page) : head(NULL), _page(page), count(0U)
		{
		}

		/// @brief Destructor for the FreeLinkedList.
		~FreeLinkedList()
		{
		}

		/**
		* @brief Adds a new node with the given memory block address to the head of the list.
		* @param data The memory block address to be added.
		*/
		rtsha_attr_inline void push(const size_t& data)
		{
			Node* newNode = new (reinterpret_cast<void*>(_page->lastFreeBlockAddress)) Node(data, head, nullptr);
			
			if (head)
			{
				head->prev = newNode;
			}
			head = newNode;

			count++;
		}

		/**
		* @brief Checks if the list is empty.
		* @return Returns true if the list is empty, otherwise false.
		*/
		rtsha_attr_inline bool is_empty() const
		{
			if (head == nullptr)
			{
				assert(count == 0);
				return true;
			}			
			if (count == 0)
			{
				return true;
			}
			return false;
		}

		/**
		* @brief Removes and retrieves a memory block address from the head of the list.
		* @return The memory block address retrieved from the list.
		*/
		rtsha_attr_inline size_t pop()
		{
			size_t ret(0U);
			if (!is_empty() && head != NULL)
			{
				Node* temp = head;
				ret = temp->data;
				head = head->next;

				if( count > 0U )
					count--;

				if (count == 0U)
				{
					head = NULL;
				}
			}
			else
			{
				count = 0U;
			}
			return ret;
		}
		
		/**
		* @brief Deletes a node with the specified memory block address from the list.
		* @param address Memory block address of the node to be deleted.
		* @param block Pointer to the memory block associated with the address.
		* @return Returns true if the node was found and removed, otherwise false.
		*/
		rtsha_attr_inline bool delete_address(const size_t& address, void* block)
		{
			if (head == nullptr)
			{
				// the list is empty
				count = 0U;
				return false;
			}
			Node* temp = reinterpret_cast<Node*>(address);

			if (head->data == temp->data)
			{
				//the node to delete is the head				
				if (head->next)
					head->next->prev = nullptr;
				head = head->next;

				temp->data = 0U;

				if (count)
					count--;
				return true;
			}
			size_t blockAddress = reinterpret_cast<size_t>(block);
			if (temp->data == blockAddress)
			{
				temp->data = 0U;
				if (temp->prev)
				{
					temp->prev->next = temp->next;
				}
				if (temp->next)
				{
					temp->next->prev = temp->prev;
				}
				if (count)
					count--;
				return true;
			}
			return false;
		}
		
	private:
		rtsha_page* _page;			///< The memory page being managed by the free list.
		Node* head;					///< @brief Pointer to the head of the list.
		size_t count = 0U;			///< @brief Number of nodes in the list.
	};
}
