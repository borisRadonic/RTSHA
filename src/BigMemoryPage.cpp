#include "BigMemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "structures.h"
#include "internal.h"
#include "errors.h"

namespace internal
{
	void* BigMemoryPage::allocate_block(size_t size)
	{
		if ((0U == size) || (nullptr == _page))
		{
			return nullptr;
		}

		bool deleted = false;
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
						deleted = true;
					}
					size_t diff = orig_size - size;
					if (diff >= (MIN_BLOCK_SIZE_FOR_SPLIT ))
					{						
						this->splitBlock(block, size);
					}
					/*set block as allocated*/
					block.setAllocated();
					return block.getAllocAddress();
				}
				return NULL;
			}
		}
		return MemoryPage::allocate_block_at_current_pos(size);
	}

	void BigMemoryPage::free_block(MemoryBlock& block)
	{
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
		this->increaseFree(block.getSize());
				
		/*set as free*/
		block.setFree();
				
		rtsha_block* orig_prev = block.getPrev();
		bool alreadym = false;
		
		/*merging loop left*/
		while (block.hasPrev() && (this->getFreeBlocks() > 0U))
		{
			MemoryBlock prev(block.getPrev());
						
			if (prev.isValid() && prev.isFree() && (prev.getSize() > 0U))
			{
				/*merge two blocks*/
				//mergeLeft(block);
				alreadym = true;
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
			if (fitOnPage(block.getSize()))
			{
				MemoryBlock next(block.getNextBlock());

				if (next.isValid() && next.isFree() && (next.getSize() > 0U) && next.hasPrev() && (next.getPrev() == block.getBlock()))
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
			else
			{
				break;
			}
		}
		
					
		ptrMap->insert((const uint64_t)block.getSize(), (size_t)block.getBlock());
		this->incFreeBlocks();

		if (ptrMap->size() != this->getFreeBlocks())
		{
			int a = 0;
			a++;
		}

	
	}

	void BigMemoryPage::splitBlock(MemoryBlock& block, size_t size)
	{
		/*create initial free blocks*/

		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
	
		block.splitt(size);
		if (block.isValid() && !block.isLast())
		{
			MemoryBlock next(block.getNextBlock());
			ptrMap->insert((const uint64_t)next.getSize(), (size_t)next.getBlock());
			this->incFreeBlocks();
		}
	}

	void BigMemoryPage::mergeLeft(MemoryBlock& block)
	{
		MemoryBlock prev(block.getPrev());
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
		if (ptrMap->del((const uint64_t)prev.getSize(), (size_t)prev.getBlock()))
		{
			/*decrease the number of free blocks*/
			this->decFreeBlocks();
		}
		block.merge_left();
				
		if (block.isLast())
		{
			this->setLastBlock(block);
		}
	}

	void BigMemoryPage::mergeRight(MemoryBlock& block)
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