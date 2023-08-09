#pragma once
#include "internal.h"
#include <stdint.h>
#include "MemoryBlock.h"
#include "HeapCallbacks.h"


namespace rtsha
{
	using namespace std;
	using namespace internal;

	/**
	* @enum rtsha_page_size_type
	* @brief Enumerates the different supported page types and sizes .
	*
	* This enumeration defines the various page sizes and types within the
	* memory system. Each entry specifies a distinct page size or category.
	*/
	enum struct rtsha_page_size_type : uint16_t
	{
		PageTypeNotDefined = 0U,  ///< Not used page type.

		PageType16	= 16U,		///< Represents a fixed memory page. All blocks are of the same size, 16 bytes, including the block header data.
		PageType32	= 32U,		///< Represents a fixed memory page. All blocks are of the same size, 32 bytes, including the block header data.
		PageType64	= 64U,		///< Represents a fixed memory page. All blocks are of the same size, 64 bytes, including the block header data.
		PageType128 = 128U,		///< Represents a fixed memory page. All blocks are of the same size, 128 bytes, including the block header data.
		PageType256 = 256U,		///< Represents a fixed memory page. All blocks are of the same size, 256 bytes, including the block header data.
		PageType512 = 512U,		///< Represents a fixed memory page. All blocks are of the same size, 512 bytes, including the block header data.

		PageTypeBig			= 613U,	///< Represents a 'Big Memory Page'
		PageTypePowerTwo	= 713U	///< Represents a 'Power Two Memory Page'
	};
	
	/**
	* @struct rtsha_page
	* @brief Represents a page within the memory system.
	*
	* This structure provides details about a page's layout, size, position,
	* and associated blocks, as well as callback and linking mechanisms for
	* managing the page in larger memory structures.
	*/
	struct rtsha_page
	{
		/// @brief Default constructor for initialization.
		rtsha_page()		
		{
		}

		address_t					ptr_list_map			= 0U;	///< Pointer or address to the list/map associated with the page.
		
		uint32_t					flags					= 0U;	///< Flags associated with the page.

		address_t					start_position			= 0U;	///< Start address of page data.
		address_t					end_position			= 0U;	///< End address of the page.
		
		address_t					position				= 0U;	///< Current position or address within the page.
		size_t						free_blocks				= 0U;	///< Number of free blocks within the page.
		
		rtsha_block*				last_block				= NULL;	///< Pointer to the last block within the page.

		address_t					lastFreeBlockAddress	= 0U;	///< Address of the last free block within the page.

		address_t					start_map_data			= 0U;	///< Start address or position of map data for the page.
		
		rtsha_page*					map_page				= 0U;	///< Associated map page if any. Used with together with 'Big Memory Page' and 'Power Two Page'.

		size_t						max_blocks				= 0U;	///< Maximum number of blocks supported by the page.

		size_t						min_block_size			= 0U;	///< Minimum block size for the page (used with Power Two pages).
		size_t						max_block_size			= 0U;	///< Maximum block size for the page (used with PowerTwo pages).

		HeapCallbacksStruct*		callbacks				= NULL;	///< Callback functions associated with the page.

		rtsha_page*					next					= NULL; ///< Pointer to the next page similar structure.
	};

	/*! \class MemoryPage
	* \brief This is a base class representing a page in memory.
	* It provides various memory handling functions that manipulate MemoryBlock's. 
	*/
	class MemoryPage
	{
	public:
				
		/// @brief Default constructor is deleted.
		MemoryPage() = delete;

		/**
		 * @brief Constructor that initializes the MemoryPage with a given page.
		 * @param page The pointer to page in memory
		 */
		explicit MemoryPage(rtsha_page* page) noexcept
			: _page(page)
		{			
		}

		/// @brief Virtual destructor.
		virtual ~MemoryPage()
		{
		}

		/**
		 * @brief Check if a block exists at the given address and if the block is valid.
		 * @param address The address to check.
		 * @return True if the block exists, otherwise false.
		 */
		bool checkBlock(size_t address);

		/**
		 * @brief Pure virtual function to allocate a block of memory.
		 * @param size Size of the block to allocate.
		 * @return A pointer to the allocated block.
		 */
		virtual void* allocate_block(size_t size) = 0;
		
		/**
		 * @brief Pure virtual function to free a block of memory.
		 * @param block The block of memory to be freed.
		 */
		virtual void free_block(MemoryBlock& block) = 0;

		/**
		 * @brief Allocates a block of memory at the current position.
		 * @param size Size of the block to allocate.
		 * @return A pointer to the allocated block.
		 */
		void* allocate_block_at_current_pos(size_t size);

		/**
		* @brief Increments the count of free blocks.
		*
		* If the _page is not null, this function increments the free_blocks count
		* associated with the _page. Typically called when a block is freed.
		*/
		inline void incFreeBlocks()
		{
			if (_page != nullptr)
			{
				_page->free_blocks++;
			}
		}

	protected:

		/**
		* @brief Locks the page for thread-safe operations.
		*
		* This method is used in conjunction with multithreading support to ensure
		* that modifications to the page are synchronized.
		*/
		inline void lock()
		{
			#ifdef MULTITHREADING_SUPPORT
			if ((nullptr != this->_page->callbacks) && (nullptr != this->_page->callbacks->ptrLockFunction))
			{
				this->_page->callbacks->ptrLockFunction();
			}
			#endif
		}

		/**
		* @brief Unlocks the page after thread-safe operations.
		*
		* This method complements the `lock` method by releasing the lock on the page.
		*/
		inline void unlock()
		{
			#ifdef MULTITHREADING_SUPPORT
			if ((nullptr != this->_page->callbacks) && (nullptr != this->_page->callbacks->ptrUnLockFunction))
			{
				this->_page->callbacks->ptrUnLockFunction();
			}
			#endif
		}

		/**
		* @brief Reports an error using the specified error callback.
		*
		* @param error The error code to report.
		*/
		inline void reportError(uint32_t error)
		{
			if ((nullptr != this->_page->callbacks) && (nullptr != this->_page->callbacks->ptrErrorFunction))
			{
				this->_page->callbacks->ptrErrorFunction(error);
			}
		}

		/**
		*@brief Sets the address of the last free block temporary.
		* The addres will be used by InternListAllocator as storage for the elements of the 'std::forward_list' 
		*
		* @param address The address to set.
		*/
		inline void setFreeBlockAllocatorsAddress(size_t address)
		{
			_page->lastFreeBlockAddress = address;
		}

		/**
		* @brief Retrieves the type of the page.
		*
		* @return The type of the page.
		*/
		inline rtsha_page_size_type getPageType() const
		{
			return (rtsha_page_size_type) _page->flags;
		}

		/**
		* @brief Gets the free list pointer of the page.
		*
		* @return A pointer to the free list.
		*/
		inline void* getFreeList() const
		{
			return reinterpret_cast<void*>(_page->ptr_list_map);
		}

		/**
		* @brief Gets the free map pointer of the page.
		*
		* @return A pointer to the free map.
		*/
		inline void* getFreeMap() const
		{
			return reinterpret_cast<void*>(_page->ptr_list_map);
		}

		/**
		* @brief Retrieves the number of free blocks in the page.
		*
		* @return The number of free blocks.
		*/
		inline size_t getFreeBlocks() const
		{
			if (_page != nullptr)
			{
				return _page->free_blocks;
			}
			return 0U;
		}

		/**
		* @brief Retrieves the number of free blocks in the page.
		*
		* @return The number of free blocks.
		*/
		inline size_t getMinBlockSize() const
		{
			if (_page != nullptr)
			{
				return _page->min_block_size;
			}
			return 0U;			
		}

		/**
		* @brief Gets the current page position.
		*
		* @return The current position.
		*/
		inline address_t getPosition() const
		{
			if (_page != nullptr)
			{
				return _page->position;
			}
			return 0U;
		}

		/**
		* @brief Sets the current page position.
		*
		* @param pos The position to set.
		*/
		inline void setPosition(address_t pos)
		{
			if (_page != nullptr)
			{
				_page->position = pos;
			}
		}

		/**
		 * @brief Increments the page position by the given value.
		 *
		 * @param val The value to increment the position by.
		 */
		inline void incPosition(size_t val)
		{
			if (_page != nullptr)
			{
				_page->position += val;
			}
		}

		/**
		* @brief Decreases the oage position by the given value.
		*
		* @param val The value to decrement the position by.
		*/
		inline void decPosition(size_t val)
		{
			if (_page != nullptr)
			{
				if (_page->position >= val)
				{
					_page->position -= val;
				}				
			}
		}
		
		/**
		* @brief Decreases the number of free blocks in the page.
		*/
		inline void decFreeBlocks()
		{
			if ( (_page != nullptr) && (_page->free_blocks > 0U) )
			{
				_page->free_blocks--;
			}
		}
		
		/**
		* @brief Gets the end position of the page.
		*
		* @return The end position.
		*/
		inline address_t getEndPosition() const
		{
			return _page->end_position;
		}

		/**
		* @brief Gets the start position of the page.
		*
		* @return The start position.
		*/
		inline address_t getStartPosition() const
		{
			return _page->start_position;
		}

		/**
		* @brief Checks if a block of the specified size fits on the page.
		*
		* @param size The size of the block to check.
		* @return True if the block fits, false otherwise.
		*/
		inline bool fitOnPage(size_t size) const
		{
			if ((_page->position + size) < (_page->end_position))
			{
				if (_page->start_map_data == 0U)
				{
					return true;
				}
				else
				{
					if ((_page->position + size) < _page->start_map_data)
					{
						return true;
					}
				}
			}
			return false;
		}

		/**
		* @brief Checks if the page 'last block' has ben set
		*
		* @return True if the page has a last block, false otherwise.
		*/
		inline bool hasLastBlock() const
		{
			return (_page->last_block != nullptr);
		}

		/**
		* @brief Determines if the provided block is the last block of the page.
		*
		* @param block The block to check.
		* @return True if it's the last block, false otherwise.
		*/
		inline bool isLastPageBlock(MemoryBlock& block) const
		{
			if (this->getPosition() == ((size_t)block.getBlock() + block.getSize()))
			{
				return (block.getBlock() == _page->last_block);
			}
			return false;
		}

		/**
		* @brief Gets the last block of the page.
		*
		* @return A pointer to the last block.
		*/
		inline rtsha_block* getLastBlock() const
		{
			return _page->last_block;
		}

		/**
		* @brief Sets the last block of the page.
		*
		* @param block The block to set as the last block.
		*/
		inline void setLastBlock(const MemoryBlock& block)
		{
			_page->last_block = block.getBlock();
		}

		/**
		* @brief Pointer to the page structure in memory.
		*/
		rtsha_page* _page;

	};
}

