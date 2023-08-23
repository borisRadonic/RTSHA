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
#include "InternMapAllocator.h"
#include "internal.h"
#include "map"

namespace internal
{
	using namespace std;
	using namespace rtsha;

	using mmap_allocator = InternMapAllocator<std::pair<const uint64_t, size_t>>;
	
	using mmap = std::multimap<const uint64_t, size_t, std::less<const uint64_t>, internal::InternMapAllocator<std::pair<const uint64_t, size_t>>>;
	
	/**
	* @class FreeMap
	* @brief A memory-efficient multimap implementation aligned to the size of size_t.
	*
	* The FreeMap class is designed to manage key-value pairs in memory.
	* It offers functionalities like insertion, deletion, and lookup of key-value pairs.
	* Internally, it employs a custom allocator and map structure to handle the memory.
	* Custom allocator uses a small part of memory page as 'SmallFixedMemoryPage'
	*/
	class alignas(sizeof(size_t)) FreeMap
	{
	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		FreeMap() = delete;

		/**
		* @brief Constructs a FreeMap with the provided memory page.
		*
		* @param page The rtsha_page that this FreeMap will manage.
		*/
		explicit FreeMap(rtsha_page* page) noexcept;

		/// @brief Destructor for the FreeMap.
		~FreeMap() noexcept
		{
		}

		/**
		* @brief Inserts a key-value pair into the multimap.
		*
		* @param key The key for the insertion. (The size of block is used as a key.)
		* @param block The associated value for the key. (Address of the block in memory.)
		*/
		rtsha_attr_inline void insert(const uint64_t key, size_t block) noexcept
		{
			if ((_ptrMap != nullptr))
			{
				_ptrMap->insert(std::pair<const uint64_t, size_t>(key, block));
			}
		}

		/**
		* @brief Deletes a key-value pair from the map based on the given key.
		*
		* @param key The key of the key-value pair to be deleted.
		* @param block The associated value for the key.
		* @return True if the deletion was successful, otherwise false.
		*/
		rtsha_attr_inline bool del(const uint64_t key, size_t block) noexcept
		{
			if ((_ptrMap != nullptr))
			{
				mmap::iterator it = _ptrMap->find(key);
				while(it != _ptrMap->end())
				{
					if ((it->first == key) && (it->second == block))
					{
						it = _ptrMap->erase(it);
						return true;
					}
					it++;
				}
			}
			return false;
		}

		/**
		* @brief Finds the value associated with a given key.
		*
		* @param key The key to be looked up.
		* @return The value associated with the key.
		*/
		rtsha_attr_inline size_t find(const uint64_t key) noexcept
		{
			if ((_ptrMap != nullptr))
			{
				mmap::iterator it = _ptrMap->lower_bound(key);
				if (it != _ptrMap->end())
				{
					return it->second;
				}
			}
			return 0U;
		}


		/**
		* @brief Checks if a given key-value pair exists in the map.
		*
		* @param key The key to be checked.
		* @param block The associated value for the key.
		* @return True if the key-value pair exists, otherwise false.
		*/
		rtsha_attr_inline bool exists(const uint64_t key, size_t block)  noexcept
		{
			if ((_ptrMap != nullptr))
			{
				mmap::iterator it = _ptrMap->find(key);
				if (it != _ptrMap->end())
				{
					if ((it->first == key) && (it->second == block))
					{
						return true;
					}
				}
			}
			return false;
		}

		/**
		* @brief Retrieves the number of key-value pairs in the map.
		*
		* @return The size of the map.
		*/
		rtsha_attr_inline size_t size() const noexcept
		{
			if ((_ptrMap != nullptr))
			{
				return _ptrMap->size();
			}
			return 0U;
		}


				
	private:
		rtsha_page*			_page;				///< The memory page being managed by the free map.
		mmap_allocator*		_mallocator;		///< Internal map allocator for managing memory key-value pairs.
		mmap*				_ptrMap;			///< Pointer to the map structure used internally.
		

	private:

		/// @brief Reserved storage for an object that will be created using 'placement new' on the stack.
		PREALLOC_MEMORY <mmap_allocator>		_storage_allocator = 0U;
		PREALLOC_MEMORY<mmap>					_storage_map = 0U;
	};
}
