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
				return NULL;
			}
		}
		return nullptr;
	}

	void PowerTwoMemoryPage::free_block(MemoryBlock& block)
	{
		this->increaseFree(block.getSize());

		/*set as free*/
		block.setFree();

		if (isLastPageBlock(block))
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
		else
		{
			if (this->getFreeBlocks() > 0U)
			{
				//try_merge_left(block, left);
			}
		}
		if (this->getFreeBlocks() > 0U)
		{
			/*check if not last block*/
			if (!block.isLast() && !this->isLastPageBlock(block))
			{
				//try_merge_right(block, right);
			}
		}
		if (getFreeBlocks() > 0U)
		{
			//try_merge_left(block);
		}
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
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
			MemoryBlock next(block.getNextBlock());


			

			ptrMap->insert((const uint64_t)next.getSize(), (size_t)next.getBlock());
			this->incFreeBlocks();
		}
	}
}