#include "PowerTwoMemoryPage.h"
#include "FreeMap.h"
#include "structures.h"
#include "internal.h"
#include "errors.h"

namespace internal
{
	void* PowerTwoMemoryPage::allocate_block(size_t size)
	{
		if ((0U == size) || (nullptr == _page))
		{
			return nullptr;
		}

		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
		if ((this->getFreeBlocks() > 0U) || (ptrMap->size() > 0U))
		{
			size_t address = ptrMap->find((const uint64_t)size);
			if (address != 0U)
			{
				MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)address));
				size_t orig_size = block.getSize();
				if (block.isValid() && (orig_size >= size))
				{					
					/*delete used block from the map of free blocks*/
					const uint64_t k = (const uint64_t)orig_size;
					if (ptrMap->del(k, (size_t)block.getBlock()))
					{
						/*decrease the number of free blocks*/
						this->decFreeBlocks();
					}
					if (block.isValid() && (orig_size > size))
					{
						this->splitBlockPowerTwo(block, size);
					}

					/*set block as allocated*/
					block.setAllocated();
					return block.getAllocAddress();
				}
				return nullptr;
			}
		}
		return nullptr;
	}

	void PowerTwoMemoryPage::free_block(MemoryBlock& block)
	{
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());

		/*set as free*/
		block.setFree();

		if (this->isLastPageBlock(block))
		{
			ptrMap->insert((const uint64_t)block.getSize(), (size_t)block.getBlock());
			this->incFreeBlocks();
			return;
		}
		/*merging loop left*/		
		while (block.hasPrev() && (this->getFreeBlocks() > 0U))
		{		
			MemoryBlock prev(block.getPrev());					
			if (prev.isValid() && prev.isFree() && (prev.getSize() == block.getSize()) )
			{
				/*merge two blocks*/
				mergeLeft(block);
				break;
			}
			else
			{
				break;
			}			
		}
				
		/*merging loop right*/
		while (!block.isLast() && !this->isLastPageBlock(block) && (this->getFreeBlocks() > 0U))
		{			
			MemoryBlock next(block.getNextBlock());

			if (next.isValid() && next.isFree() && (next.getSize() == block.getSize()))
			{
				/*merge two blocks*/
				mergeRight(block);
				break;
			}
			else
			{
				break;
			}
		}

		ptrMap->insert((const uint64_t)block.getSize(), (size_t)block.getBlock());
		this->incFreeBlocks();
	}

	void PowerTwoMemoryPage::splitBlockPowerTwo(MemoryBlock& block, size_t end_size)
	{
		/*create initial free blocks*/
		size_t data_size = block.getSize();
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());

		while (data_size > end_size)
		{
			block.splitt_22();
			data_size = block.getSize();
			
			if (block.hasPrev())
			{
				MemoryBlock prev(block.getPrev());
				ptrMap->insert((const uint64_t) prev.getSize(), (size_t)prev.getBlock());
				this->incFreeBlocks();
			}
		}
	}

	void PowerTwoMemoryPage::mergeLeft(MemoryBlock& block)
	{
		MemoryBlock prev(block.getPrev());
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
		if (ptrMap->del( (const uint64_t)prev.getSize(), (size_t)prev.getBlock()))
		{
			/*decrease the number of free blocks*/
			this->decFreeBlocks();
		}
		block.merge_left();
	}

	void PowerTwoMemoryPage::mergeRight(MemoryBlock& block)
	{
		MemoryBlock next(block.getNextBlock());
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
		if (ptrMap->del((const uint64_t)next.getSize(), (size_t)next.getBlock()))
		{
			/*decrease the number of free blocks*/
			this->decFreeBlocks();
		}
		block.merge_right();
	}
}