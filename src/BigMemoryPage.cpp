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
					size_t diff = orig_size - size;
					//diff = rtsha_align(diff);

					if (diff >= (MIN_BLOCK_SIZE_FOR_SPLIT << 12))
					{
						/*delete used block from the map of free blocks*/
						const uint64_t k = (const uint64_t)orig_size;
						if (ptrMap->del(k, (size_t)block.getBlock()))
						{
							/*decrease the number of free blocks*/
							this->decFreeBlocks();
						}

						/*splitt block*/
						MemoryBlock tempNode(block.splitt(size));
						/*insert new block to the map of free blocks*/
						ptrMap->insert((const uint64_t)tempNode.getSize(), (size_t)tempNode.getBlock());
						this->incFreeBlocks();
					}
					else
					{
						/*delete used block from the map of free blocks*/
						const uint64_t k = (const uint64_t)orig_size;
						if (ptrMap->del(k, (size_t)block.getBlock()))
						{
							/*decrease the number of free blocks*/
							this->decFreeBlocks();
						}
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
		bool left(false);
		bool right(false);
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
				try_merge_left(block, left);
			}
		}
		if (this->getFreeBlocks() > 0U)
		{
			/*check if not last block*/
			if (!block.isLast() && !this->isLastPageBlock(block))
			{
				try_merge_right(block, right);
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

	void BigMemoryPage::try_merge_left(MemoryBlock& block, bool& merged)
	{
		size_t old_size = 0U;
		merged = false;
		if ((this->getFreeBlocks() >= 1U) && block.hasPrev())
		{
			MemoryBlock prev(block.getPrev());
			if (prev.isValid() && prev.isFree())
			{
				/*left is free -> merge those two blocks*/
				old_size = prev.getSize();
				prev.setSize(prev.getSize() + block.getSize());

				merged = true;

				/*remove prev. block with old size from map of the free blocks*/
				FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
				if (ptrMap != nullptr)
				{
					if (ptrMap->del((const uint64_t)old_size, (size_t)prev.getBlock()))
					{
						this->decFreeBlocks();
					}
				}
				/*prev. block with new size will be inserted later*/
				if (this->isLastPageBlock(block))
				{
					setLastBlock(prev);
					prev.setLast();
				}

				/*set as free*/
				prev.setFree();

				if (!block.isLast())
				{
					/*update previous of the block on the right side*/
					MemoryBlock temp_block((rtsha_block*)((size_t)((void*)block.getBlock()) + block.getSize()));
					if (temp_block.isValid() && temp_block.hasPrev())
					{
						temp_block.setPrev(prev);
					}
				}
				block = MemoryBlock(prev.getBlock());
			}
		}
	}

	void BigMemoryPage::try_merge_right(MemoryBlock& block, bool& merged)
	{
		merged = false;
		if (!this->isLastPageBlock(block) && (this->getFreeBlocks() > 0U))
		{
			rtsha_block* pBlock = block.getBlock();
			MemoryBlock temp_block((rtsha_block*)((size_t)((size_t)(void*)pBlock) + block.getSize()));
			if (temp_block.isValid() && (temp_block.getSize() > 0U) && temp_block.isFree() && temp_block.hasPrev() && (temp_block.getPrev() == pBlock))
			{
				/*right is free -> merge two blocks*/
				block.setSize(block.getSize() + temp_block.getSize());

				merged = true;

				/*clear right block header data*/
				if (temp_block.isLast())
				{
					/*set as last block*/
					block.setLast();
					this->setLastBlock(block);
				}
				else
				{
					/*set prev of the next right*/
					MemoryBlock temp_blockR(temp_block.getNextBlock());
					if (temp_blockR.isValid() && temp_blockR.hasPrev() && (temp_blockR.getPrev() == temp_block.getBlock()))
					{
						temp_blockR.setPrev(temp_block);
					}
				}
				FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
				if (ptrMap != nullptr)
				{
					if (ptrMap->del((const uint64_t)temp_block.getSize(), (size_t)temp_block.getBlock()))
					{
						this->decFreeBlocks();
					}
				}
				temp_block.setSize(0U);

				/*set as free*/
				block.setFree();
			}
		}
	}
}