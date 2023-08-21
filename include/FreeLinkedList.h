#pragma once
#include <stdint.h>
#include "internal.h"
#include "MemoryPage.h"
#include <bitset>

namespace internal
{
	using namespace rtsha;
	
	class alignas(sizeof(size_t))  FreeLinkedList
	{
		struct alignas(sizeof(size_t)) Node
		{
			size_t data;
			Node* prev;
			Node* next;
			explicit Node(const size_t& data, Node* head, Node* prev = nullptr) : data(data), next(head), prev(prev)
			{
			};
		};
		
	public:

		explicit FreeLinkedList(rtsha_page* page) : head(NULL), _page(page), count(0U)
		{
		}

		~FreeLinkedList()
		{
		}

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
		

		rtsha_attr_inline bool delete_address(const size_t address, void* block)
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
		rtsha_page* _page;									///< The memory page being managed by the free list.
		Node* head;
		size_t count = 0U;
	};
}
