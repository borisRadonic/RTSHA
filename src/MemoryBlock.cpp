#include "MemoryBlock.h"


namespace internal
{
	rtsha_block* MemoryBlock::splitt(size_t new_size)
	{
		bool last = this->isLast();
		size_t osize = this->getSize();
		rtsha_block* pNewNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + new_size));
		pNewNextRight->prev = this->getBlock();
		pNewNextRight->size = osize - new_size;
				
		if (!last)
		{
			rtsha_block* pOldNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + osize));
			pOldNextRight->prev = pNewNextRight;
		}
		
		this->setSize(new_size);		
		return pNewNextRight;
	}

	void MemoryBlock::splitt_22()
	{
		bool last = this->isLast();
		
		size_t osize = this->getSize();
		size_t nsize = osize >> 1U;
		rtsha_block* pNewNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + nsize));

		pNewNextRight->prev = this->getBlock();
		pNewNextRight->size = nsize;

		if (!last)
		{
			rtsha_block* pOldNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)pNewNextRight + nsize));
			pOldNextRight->prev = pNewNextRight;
		}

		this->setSize(nsize);
		this->_block = pNewNextRight;
	}
}
