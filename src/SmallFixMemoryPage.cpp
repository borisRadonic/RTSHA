#include "SmallFixMemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "internal.h"
#include "errors.h"

namespace rtsha
{
	using namespace internal;

	void* SmallFixMemoryPage::allocate_block(size_t size)
	{
		void* ret = nullptr;

		if ((0U == size) || (nullptr == _page))
		{
			return nullptr;
		}
		this->lock();
		/*try to use next free block*/
		FreeList* ptrList = reinterpret_cast<FreeList*>(this->getFreeList());
		size_t address = ptrList->pop();
		if (address > 0U)
		{
			this->decFreeBlocks();
			/*set block as allocated*/
			MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)address));
			block.setAllocated();
			//this->decreaseFree(size);
			ret = block.getAllocAddress();
			this->unlock();
			return ret;
		}
		ret = allocate_block_at_current_pos(size);
		this->unlock();
		return ret;
	}

	void SmallFixMemoryPage::free_block(MemoryBlock& block)
	{
		this->lock();

		/*set as free*/
		block.setFree();

		if (this->isLastPageBlock(block))
		{
			block.setLast();
		}

		/*check if it is the first page block*/
		if (!block.hasPrev())
		{
			/*it must be the first: update prev of the next*/
			rtsha_block* next_block = block.getNextBlock();
			MemoryBlock next(next_block);
			next.setAsFirst();
		}

		FreeList* ptrList = reinterpret_cast<FreeList*>(this->getFreeList());
		this->setFreeBlockAllocatorsAddress(block.getFreeBlockAddress());
		ptrList->push(reinterpret_cast<size_t>(reinterpret_cast<void*>(block.getBlock())));
		this->incFreeBlocks();

		this->unlock();
	}
}