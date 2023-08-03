#pragma once
#pragma once
#include <stdint.h>
#include "internal.h"

namespace internal
{
	using namespace std;

	struct rtsha_block
	{
		rtsha_block()
			:size(0U)
			,prev(NULL)
		{
		}

		size_t					   size; /*size is alligned,last 2 bits are not used
											0 bit contains 1 when free
											1 bit contains 1 when last block*/
		rtsha_block* prev;
	};

	class MemoryBlock
	{
	public:

		MemoryBlock() = delete;

		MemoryBlock(rtsha_block* block) : _block(block)
		{
		}

		~MemoryBlock()
		{
		}

		MemoryBlock& operator = (const MemoryBlock& rhs)
		{
			this->_block = rhs._block;
			return *this;
		}


		/*old block is resized (on the left side is old block)*/
		void splitt(size_t new_size, bool last);

		/*old block is on the right side (last block in the chain)*/
		void splitt_22();

		void merge_left();

		void merge_right();

		inline void setAllocated()
		{
			_block->size = (_block->size >> 1U) << 1U;
		}

		inline void setFree()
		{
			_block->size = (_block->size | 1U);
		}

		inline void setLast()
		{
			_block->size = (_block->size | 2U);
		}

		inline void clearIsLast()
		{
			_block->size &= ~(1UL << 1U);			
		}

		inline rtsha_block* getBlock() const
		{
			return _block;
		}

		inline void* getAllocAddress() const
		{
			return reinterpret_cast<void*>((size_t)_block + 2U * sizeof(size_t));
		}

		inline size_t getSize() const
		{
			return (_block->size >> 2U) << 2U;
		}

		inline bool isValid() const
		{
			if (_block != nullptr)
			{
				size_t size = getSize();
				if ((_block != _block->prev) && (size > sizeof(size_t)))
				{
					size_t* ptrSize2 = (size_t*)((size_t)_block + size - sizeof(size_t));
					return (*ptrSize2 == size);
				}
			}
			return false;
		}

		inline void setSize( size_t size )
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
				size_t* ptrSize2 = (size_t*) ((size_t)_block + size - sizeof(size_t));
				*ptrSize2 = size;
			}
			else
			{
				_block->size = 0U;
			}
		}

		inline size_t getFreeBlockAddress() const
		{
			return ((size_t)_block + 2U * sizeof(size_t));
		}

		inline void setPrev(const MemoryBlock& prev)
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

		inline void setAsFirst()
		{
			_block->prev = NULL;			
		}

		inline bool isFree()
		{
			return is_bit(_block->size, 0U);
		}

		inline bool isLast()
		{
			return is_bit(_block->size, 1U);
		}

		inline bool hasPrev()
		{
			return (_block->prev != NULL);
		}

		inline rtsha_block* getNextBlock() const
		{
			return reinterpret_cast<rtsha_block*>((size_t)_block + this->getSize());
		}

		inline rtsha_block* getPrev() const
		{
			return _block->prev;
		}

		inline void destroy()
		{
			_block->prev = NULL;
			_block->size = 0;
		}
		
	private:
		rtsha_block* _block;
	};
}
