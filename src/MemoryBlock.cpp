#include "MemoryBlock.h"
#include <cstdio>
#include <iostream>

namespace rtsha
{
	void MemoryBlock::splitt(const size_t& new_size, bool last)
	{		
		size_t osize = this->getSize();
		size_t* pNewBlock = reinterpret_cast<size_t*>((size_t)_block + new_size);
		*pNewBlock = 0U; /*clear size*/
		rtsha_block* pNewNextRight = reinterpret_cast<rtsha_block*>(pNewBlock);
		MemoryBlock nextRight(pNewNextRight);

		if (!this->isLast())
		{
			nextRight.clearIsLast();
		}
		nextRight.setPrev(*this);
		nextRight.setSize(osize-new_size);
		nextRight.setFree();

		if (!last)
		{
			rtsha_block* pOldNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + osize));
			pOldNextRight->prev = pNewNextRight;
		}
		else
		{
			/*it should never happen -  the last 64B internal block can not be free*/
			assert(false);
			nextRight.setLast();
			clearIsLast();
		}
		this->setSize(new_size);
	}

	void MemoryBlock::splitt_22()
	{
		bool last = this->isLast();
		size_t osize = this->getSize();
		size_t nsize = osize >> 1U;

		size_t* pNewBlock = reinterpret_cast<size_t*>((size_t)_block + nsize);
		*pNewBlock = 0U; /*clear size*/

		rtsha_block* pNewNextRight = new (reinterpret_cast<void*>(pNewBlock)) rtsha_block();
		MemoryBlock nextRight(pNewNextRight);
		if (!last)
		{
			nextRight.clearIsLast();
		}
		nextRight.setPrev(*this);
		nextRight.setSize(nsize);
		nextRight.setFree();

		if (!last)
		{
			rtsha_block* pOldNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)pNewNextRight + nsize));
			pOldNextRight->prev = pNewNextRight;
		}
		else
		{
			nextRight.setLast();
			clearIsLast();
		}
		this->setSize(nsize);
		this->_block = pNewNextRight;
	}

	void MemoryBlock::merge_left()
	{
		bool last = this->isLast();
		if (this->hasPrev() && this->isFree() )
		{
			MemoryBlock prev(this->getPrev());
			if (prev.isFree())
			{
				size_t tsize = this->getSize();
				size_t psize = prev.getSize();
				prev.clearIsLast();
				prev.setSize(tsize + psize);

				if (!last)
				{
					rtsha_block* pRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + tsize));
					
					MemoryBlock right(pRight);
					if( right.isValid() && right.hasPrev() && (right.getPrev() == this->getBlock()) )
					{
						right.setPrev(prev);						
					}
				}
				else
				{
					/*it should never happen -  the last block*/
					assert(false);
					prev.setLast();
				}
				/*destroy old header*/
				this->_block->prev = 0U;
				this->_block->size = 0U;
				this->_block = prev.getBlock();
			}
		}
	}

	void MemoryBlock::merge_right()
	{
		if (!this->isLast())
		{
			MemoryBlock next(this->getNextBlock());
			bool isLast = next.isLast();
			if (!isLast && !next.isLast())
			{
				MemoryBlock nextNext(next.getNextBlock());
				rtsha_block* next_next = next.getNextBlock();
				if ( (next_next != nullptr) && (next_next->prev != nullptr) && ((size_t) next_next->prev == (size_t)next.getBlock()) )
				{
					if (nextNext.isValid())					
					{
						nextNext.setPrev(*this);
					}
				}			
			}
			size_t tsize = this->getSize();
			size_t nsize = next.getSize();
			this->setSize(tsize + nsize);
			if (isLast)
			{
				this->setLast();
			}
		}		
	}
}