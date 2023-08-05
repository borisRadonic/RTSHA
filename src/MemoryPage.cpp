#include "MemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "structures.h"
#include "internal.h"
#include "errors.h"

namespace rtsha
{
	void* MemoryPage::allocate_block_at_current_pos(size_t size)
	{
		if (this->fitOnPage(size))
		{
			MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)this->getPosition()));
			block.prepare(); /*memory can contain old bits*/
			block.setSize(size);	
			if (this->hasLastBlock())
			{
				MemoryBlock last_block(this->getLastBlock());
				block.setPrev(last_block);
				last_block.clearIsLast();
			}
			else
			{
				block.setAsFirst();
			}
			block.setLast();
			this->setLastBlock(block);
			this->incPosition(size);
			//cout << "new allocated " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
			return block.getAllocAddress();
		}
		return nullptr;
	}

	bool MemoryPage::checkBlock(size_t address)
	{
		size_t address2 = address - sizeof(rtsha_block); /*skip size and pointer to prev*/
		MemoryBlock block((rtsha_block*)(void*)address2);
		return block.isValid();
	}
}