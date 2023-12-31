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
			Node* next;
			Node* prev;


			/**
			* @brief Constructs a new Node.
			* @param data The memory block address.
			* @param head Pointer to the next node.
			* @param prev Pointer to the previous node.
			*/
			explicit Node(const size_t& data, Node* head, Node* prev = nullptr)  noexcept : data(data), next(head), prev(prev)
			{
			};
		};
		
	public:

		/**
		* @brief Constructs a FreeLinkedList with the given memory page.
		* @param page The rtsha_page that this FreeLinkedList should manage.
		*/
		explicit FreeLinkedList(rtsha_page* page)  noexcept : head(nullptr), _page(page), count(0U)
		{
		}

		/// @brief Destructor for the FreeLinkedList.
		~FreeLinkedList() noexcept
		{
		}

		/**
		* @brief Adds a new node with the given memory block address to the head of the list.
		* @param data The memory block address to be added.
		*/
		rtsha_attr_inline void push(const size_t& data) noexcept
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
		rtsha_attr_inline bool is_empty() const noexcept
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
		rtsha_attr_inline size_t pop() noexcept
		{
			size_t ret(0U);
			if( !is_empty() )
			{
				Node* temp = head;
				ret = temp->data;
				head = head->next;

				if( count > 0U )
					count--;

				if (count == 0U)
				{
					head = nullptr;
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
		rtsha_attr_inline bool delete_address(const size_t& address, void* block) noexcept
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
				temp->next = nullptr;
				temp->prev = nullptr;


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
		Node* head;					///< @brief Pointer to the head of the list.
		rtsha_page* _page;			///< The memory page being managed by the free list.
		size_t count = 0U;			///< @brief Number of nodes in the list.
	};
}
