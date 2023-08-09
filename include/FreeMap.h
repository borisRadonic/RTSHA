#pragma once
#include <stdint.h>
#include "MemoryBlock.h"
#include "InternMapAllocator.h"
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
		explicit FreeMap(rtsha_page* page);

		/// @brief Destructor for the FreeMap.
		~FreeMap()
		{
		}

		/**
		* @brief Inserts a key-value pair into the multimap.
		*
		* @param key The key for the insertion. (The size of block is used as a key.)
		* @param block The associated value for the key. (Address of the block in memory.)
		*/
		void insert(const uint64_t key, size_t block);

		/**
		* @brief Deletes a key-value pair from the map based on the given key.
		*
		* @param key The key of the key-value pair to be deleted.
		* @param block The associated value for the key.
		* @return True if the deletion was successful, otherwise false.
		*/
		bool del(const uint64_t key, size_t block);

		/**
		* @brief Finds the value associated with a given key.
		*
		* @param key The key to be looked up.
		* @return The value associated with the key.
		*/
		size_t find(const uint64_t key);

		/**
		* @brief Checks if a given key-value pair exists in the map.
		*
		* @param key The key to be checked.
		* @param block The associated value for the key.
		* @return True if the key-value pair exists, otherwise false.
		*/
		bool exists(const uint64_t key, size_t block);

		/**
		* @brief Retrieves the number of key-value pairs in the map.
		*
		* @return The size of the map.
		*/
		size_t size() const;
				
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
