#include "PowerTwoMemoryPage.h"
#include "FreeMap.h"
#include "internal.h"
#include "errors.h"

namespace rtsha
{
	using namespace internal;

	void* PowerTwoMemoryPage::allocate_block(size_t size)
	{
		void* ret = NULL;

		if ((0U == size) || (nullptr == _page))
		{
			return nullptr;
		}

		this->lock();

		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
		if ((this->getFreeBlocks() > 0U) || (ptrMap->size() > 0U))
		{
			size_t address = ptrMap->find(static_cast<const uint64_t>(size));
			if (address != 0U)
			{
				MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)address));
				size_t orig_size = block.getSize();

				if (!block.isValid())
				{
					assert(false);
					this->reportError(RTSHA_InvalidBlock);
					this->unlock();
					return ret;
				}

				if (block.isValid() && (orig_size >= size))
				{					
					/*delete used block from the map of free blocks*/
					const uint64_t k = static_cast<const uint64_t>(orig_size);
					if (ptrMap->del(k, reinterpret_cast<size_t>(block.getBlock())))
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
					ret = block.getAllocAddress();
					
					if (!block.isValid())
					{
						assert(false);
						this->reportError(RTSHA_InvalidBlock);
						this->unlock();
						return NULL;
					}
				}
			}
		}

		this->unlock();
		
		return ret;
	}

	void PowerTwoMemoryPage::free_block(MemoryBlock& block)
	{
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());

		this->lock();

		/*set as free*/
		block.setFree();

		if (this->isLastPageBlock(block))
		{
			ptrMap->insert(static_cast<const uint64_t>(block.getSize()), reinterpret_cast<size_t>(block.getBlock()));
			this->incFreeBlocks();
			this->unlock();
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
		if (block.isValid())
		{
			ptrMap->insert(static_cast<const uint64_t>(block.getSize()), reinterpret_cast<size_t>(block.getBlock()));
			this->incFreeBlocks();
		}
		else
		{
			assert(false);
			this->reportError(RTSHA_InvalidBlock);
		}		
		
		this->unlock();
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
				ptrMap->insert(static_cast<const uint64_t>(prev.getSize()), reinterpret_cast<size_t>(prev.getBlock()));
				this->incFreeBlocks();
			}
		}
	}

	void PowerTwoMemoryPage::mergeLeft(MemoryBlock& block)
	{
		MemoryBlock prev(block.getPrev());
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
		if (ptrMap->del(static_cast<const uint64_t>(prev.getSize()), reinterpret_cast<size_t>(prev.getBlock())) )
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
		if (ptrMap->del(static_cast<const uint64_t>(next.getSize()), reinterpret_cast<size_t>(next.getBlock())))
		{
			/*decrease the number of free blocks*/
			this->decFreeBlocks();
		}
		block.merge_right();
	}

	void PowerTwoMemoryPage::createInitialFreeBlocks()
	{
		/*create initial free blocks*/
		size_t data_size = this->getEndPosition() - this->getStartPosition();
		size_t last_lbit = sizeof(SIZE_MAX) * 8U - 1U;
		size_t val = 1U << last_lbit;
		size_t rest = data_size;
		rtsha_block* prev = nullptr;
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());

		bool first(false);
		bool condition = (rest > this->getMinBlockSize()) && (this->getPosition() < this->getEndPosition());
		while (condition)
		{
			if (rest < val)
			{
				val = val >> 1U; /*divide by two*/
			}
			else
			{
				MemoryBlock block(reinterpret_cast<rtsha_block*>(this->getPosition()));
				block.setSize(val);
				MemoryBlock prev_block(prev);
				block.setPrev(prev_block);

				if (!first)
				{
					block.setAsFirst();
					first = true;
				}
				this->incPosition(val);
				rest = rest - val;
				prev = block.getBlock();
				block.setFree();
				ptrMap->insert((const uint64_t)block.getSize(), (size_t)block.getBlock());
				this->setLastBlock(block);
				this->incFreeBlocks();

				condition = (rest > this->getMinBlockSize() && (this->getPosition() < this->getEndPosition()) );
				if (!condition)
				{
					block.setLast();
					break;
				}
			}
		}
	}
}