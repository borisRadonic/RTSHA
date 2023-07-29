#include "MemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "structures.h"
#include "internal.h"
#include "errors.h"

namespace internal
{
	void* MemoryPage::allocate_block_at_current_pos(size_t size)
	{
		if (!this->fitOnPage(size))
		{
			return nullptr;
		}
		MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)this->getPosition()));
		block.setSize(size);
		if (this->hasLastBlock())
		{
			MemoryBlock last_block(this->getLastBlock());
			block.setPrev(last_block);
		}
		this->setLastBlock(block);
		this->incPosition(size);
		this->decreaseFree(size);
		return block.getAllocAddress();
	}
}