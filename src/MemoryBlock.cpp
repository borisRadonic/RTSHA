#include "MemoryBlock.h"


namespace internal
{
	rtsha_block* MemoryBlock::splitt(size_t new_size)
	{
		size_t old_size = this->getSize();
		rtsha_block* pNewNode = reinterpret_cast<rtsha_block*>( (void*) ( (size_t)_block + old_size));
		this->setSize(new_size);
		pNewNode->prev = this->getBlock();
		pNewNode->size = old_size - new_size;
		return pNewNode;
	}
}
