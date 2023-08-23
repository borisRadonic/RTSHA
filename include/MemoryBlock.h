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

namespace rtsha
{
	using namespace std;

	/**
	* @struct rtsha_block
	* @brief Represents a block of memory within a memory page.
	*
	* This structure provides the necessary attributes to manage a memory block,
	* including its size and reference to a previous block.
	*/
	struct rtsha_block
	{
		/// @brief Default constructor for the block, initializing it to default values.
		rtsha_block()
			:size(0U)
			,prev(NULL)
		{
		}

		size_t					   size;	///< Size of the block. Aligned size with the last two bits 
											///< reserved for special flags. Bit 0 indicates free status,
											///< and bit 1 indicates if it's the last block.
		rtsha_block* prev;					///< Pointer to the previous block.
	};

	/**
	* @class MemoryBlock
	* @brief Provides an abstraction for handling blocks of memory.
	*
	* This class offers various utility functions for manipulating a block of memory,
	* including splitting, merging, and various getters and setters.
	*/
	class MemoryBlock
	{
	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		MemoryBlock() = delete;

		/**
		* @brief Constructor that initializes the MemoryBlock with a given block.
		* @param block The rtsha_block to initialize the MemoryBlock with.
		*/
		explicit MemoryBlock(rtsha_block* block) noexcept : _block(block)
		{
		}

		/// @brief Destructor for the MemoryBlock.
		~MemoryBlock() noexcept
		{
		}

		/**
		* @brief Overloaded assignment operator for the MemoryBlock class.
		*
		* This allows one MemoryBlock to be assigned to another, copying its underlying block reference.
		* @param rhs The right-hand side MemoryBlock instance to assign from.
		* @return A reference to the updated MemoryBlock.
		*/
		MemoryBlock& operator = (const MemoryBlock& rhs) noexcept
		{
			this->_block = rhs._block;
			return *this;
		}


		/**
		 * @brief Splits the current block into two blocks.
		 *
		 * After splitting, the original block is resized and located on the left side.
		 * @param new_size The size of the original block after the split.
		 * @param last Indicates if the new block should be the last in the chain.
		 */
		void splitt(const size_t& new_size, bool last) noexcept;

		/**
		*@brief Splits the current block into two blocks of the same size such that the old block is on the right side.
		*
		* This is used when the old block is the last block in a chain.
		*/
		void splitt_22() noexcept;

		/**
		* @brief Merges the current block with the one to its left.
		*/
		void merge_left() noexcept;

		/**
		* @brief Merges the current block with the one to its right.
		*/
		void merge_right() noexcept;


		/**
		* @brief Marks the current block as allocated.
		*/
		rtsha_attr_inline void setAllocated() noexcept
		{
			_block->size = (_block->size >> 1U) << 1U;
		}

		/**
		* @brief Marks the current block as free.
		*/
		rtsha_attr_inline void setFree() noexcept
		{
			_block->size = (_block->size | 1U);
		}

		/**
		* @brief Marks the current block as the last block in the chain.
		*/
		rtsha_attr_inline void setLast() noexcept
		{
			_block->size = (_block->size | 2U);
		}

		/**
		* @brief Clears the 'is last' status of the current block.
		*/
		rtsha_attr_inline void clearIsLast() noexcept
		{
			_block->size &= ~(1UL << 1U);			
		}

		/**
		* @brief Retrieves the underlying rtsha_block pointer.
		* @return A pointer to the associated rtsha_block.
		*/
		rtsha_attr_inline rtsha_block* getBlock() const noexcept
		{
			return _block;
		}

		/**
		*@brief Retrieves the memory address allocated for the block.
		* @return The starting address of the allocated memory (block data).
		*/
		rtsha_attr_inline void* getAllocAddress() const noexcept
		{
			return reinterpret_cast<void*>((size_t)_block + 2U * sizeof(size_t));
		}

		/**
		* @brief Gets the size of the current block.
		* @return The size of the block.
		*/
		rtsha_attr_inline size_t getSize() const noexcept
		{
			return (_block->size >> 2U) << 2U;
		}

		/**
		* @brief Checks if the current MemoryBlock instance is valid.
		* @return True if the block is valid, otherwise false.
		*/
		rtsha_attr_inline bool isValid() const noexcept
		{
			if (_block != nullptr)
			{
				size_t size = getSize();
				if ((_block != _block->prev) && (size > sizeof(size_t)))
				{					
					size_t* ptrSize2 = reinterpret_cast<size_t*>((size_t)_block + size - sizeof(size_t));
					return (*ptrSize2 == size);
				}
			}
			return false;
		}

		/**
		* @brief Sets the size of the current block.
		* @param size The new size to set for the block.
		*/
		rtsha_attr_inline void setSize( size_t size ) noexcept
		{
			if (size > sizeof(size_t))
			{
				bool free = isFree();
				bool last = isLast();
				_block->size = size;
				if (free)
				{
					setFree();
				}
				if (last)
				{
					setLast();
				}
				size_t* ptrSize2 = reinterpret_cast<size_t*>((size_t)_block + size - sizeof(size_t));
				*ptrSize2 = size;
			}
			else
			{
				_block->size = 0U;
			}
		}

		/**
		* @brief Retrieves the address of the free block.
		* @return The address of the free block.
		*/
		rtsha_attr_inline size_t getFreeBlockAddress() const noexcept
		{
			return ((size_t)_block + 2U * sizeof(size_t));
		}

		/**
		* @brief Sets the previous block for the current block.
		* @param prev The previous MemoryBlock to set.
		*/
		rtsha_attr_inline void setPrev(const MemoryBlock& prev) noexcept
		{
			if (prev.isValid())
			{
				_block->prev = prev.getBlock();
			}
			else
			{
				_block->prev = NULL;
			}
		}

		/**
		* @brief Sets the current block as the first block in a chain.
		*/
		rtsha_attr_inline void setAsFirst() noexcept
		{
			_block->prev = NULL;			
		}

		/**
		* @brief Checks if the current block is free.
		*
		* The method examines the 0th bit of the size attribute to determine the block's status.
		* @return True if the block is free, otherwise false.
		*/
		rtsha_attr_inline bool isFree() noexcept
		{
			return is_bit(_block->size, 0U);
		}

		/**
		*@brief Checks if the current block is the last block in the chain.
		*
		* The method examines the 1st bit of the size attribute to determine the block's position.
		* @return True if the block is the last block, otherwise false.
		*/
		rtsha_attr_inline bool isLast() noexcept
		{
			return is_bit(_block->size, 1U);
		}

		/**
		* @brief Checks if the current block has a predecessor.
		*
		* @return True if the block has a previous block, otherwise false.
		*/
		rtsha_attr_inline bool hasPrev() noexcept
		{
			return (_block->prev != NULL);
		}

		/**
		* @brief Retrieves the next block in memory relative to the current block.
		*
		* @return A pointer to the next rtsha_block in the chain.
		*/
		rtsha_attr_inline rtsha_block* getNextBlock() const noexcept
		{
			return reinterpret_cast<rtsha_block*>((size_t)_block + this->getSize());
		}

		/**
		* @brief Retrieves the previous block relative to the current block.
		*
		* @return A pointer to the previous rtsha_block.
		*/
		rtsha_attr_inline rtsha_block* getPrev() const noexcept
		{
			return _block->prev;
		}

		/**
		* @brief Prepares the current block for use by resetting its attributes in memory.
		*/
		rtsha_attr_inline void prepare() noexcept
		{
			_block->prev = NULL;
			_block->size = 0;
		}
		
	private:
		rtsha_block* _block; ///< The underlying memory block being managed.
	};
}
