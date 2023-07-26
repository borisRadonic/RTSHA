#include "MemoryBlock.h"


namespace internal
{
	rtsha_block* MemoryBlock::splitt(size_t new_size)
	{
		size_t osize = this->getSize();
		rtsha_block* pNewNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + new_size));
		pNewNextRight->prev = this->getBlock();
		pNewNextRight->size = osize - new_size;
		if (!this->isLast())
		{
			rtsha_block* pOldNextRight = reinterpret_cast<rtsha_block*>((void*)((size_t)_block + osize));
			pOldNextRight->prev = pNewNextRight;
		}
		this->setSize(new_size);		
		return pNewNextRight;
	}
}
