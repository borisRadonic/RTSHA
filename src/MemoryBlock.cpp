#include "MemoryBlock.h"


namespace internal
{
	rtsha_block* MemoryBlock::splitt(size_t new_size)
	{
		bool last = this->isLast();
		size_t osize = this->getSize();
		rtsha_block* pNewNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + new_size));
		MemoryBlock nextRight(pNewNextRight);
		nextRight.setPrev(*this);
		nextRight.setFree();
				
		if (!last)
		{
			rtsha_block* pOldNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + osize));
			pOldNextRight->prev = pNewNextRight;
		}
		this->setSize(osize-new_size);
		return pNewNextRight;
	}

	void MemoryBlock::splitt_22()
	{
		bool last = this->isLast();
		size_t osize = this->getSize();
		size_t nsize = osize >> 1U;
		rtsha_block* pNewNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + nsize));
		MemoryBlock nextRight(pNewNextRight);
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
			/*normaly it should not be the case (last block is the smallest in Power Two Page)*/
			nextRight.setLast();
			clearIsLast();
		}
		this->setSize(nsize);
		this->_block = pNewNextRight;
	}

	void MemoryBlock::merge_left()
	{
		bool last = this->isLast();
		if (this->hasPrev())
		{
			MemoryBlock prev(this->getPrev());
			if (prev.isFree())
			{
				size_t tsize = this->getSize();
				size_t psize = prev.getSize();
				prev.setSize(tsize + psize);

				if (!last)
				{
					rtsha_block* pRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + tsize));
					MemoryBlock right(pRight);
					if (right.isValid())
					{
						right.setPrev(prev.getBlock());
					}
				}
				else
				{
					prev.setLast();
				}
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


			rtsha_block* next_next = next.getNextBlock();
			if (!isLast && (next_next != nullptr))
			{
				MemoryBlock nextNext(next_next);
				nextNext.setPrev(*this);
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
