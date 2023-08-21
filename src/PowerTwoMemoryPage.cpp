#include "PowerTwoMemoryPage.h"
#include "FreeListArray.h"
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

		FreeListArray* ptrFreeListArray = reinterpret_cast<FreeListArray*>(this->getFreeListArray());

		if ((this->getFreeBlocks() > 0U))
		{
			size_t address = ptrFreeListArray->pop(size);
			if (address != 0U)
			{
				MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)address));				

				if (!block.isValid())
				{
					assert(false);
					this->reportError(RTSHA_InvalidBlock);
					this->unlock();
					return ret;
				}

				size_t orig_size = block.getSize();
				if (orig_size >= size) 
				{					
					/*decrease the number of free blocks*/
					this->decFreeBlocks();					
					if (orig_size > size)
					{
						while (orig_size > size)
						{
							block.splitt_22();
							orig_size = block.getSize();
							assert(block.isValid());

							if (block.hasPrev())
							{
								MemoryBlock prev(block.getPrev());
								assert(prev.isValid());
								this->setFreeBlockAllocatorsAddress(prev.getFreeBlockAddress());
								ptrFreeListArray->push( reinterpret_cast<size_t>(prev.getBlock()), prev.getSize());
								this->incFreeBlocks();
							}
						}
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
		FreeListArray* ptrFreeListArray = reinterpret_cast<FreeListArray*>(this->getFreeListArray());

		this->lock();

		/*set as free*/
		block.setFree();

		if (this->isLastPageBlock(block))
		{
			this->setFreeBlockAllocatorsAddress(block.getFreeBlockAddress());
			ptrFreeListArray->push( reinterpret_cast<size_t>(block.getBlock()), block.getSize() );
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
				if( ptrFreeListArray->delete_address(reinterpret_cast<size_t>(prev.getAllocAddress()), prev.getBlock(), prev.getSize()) )
				{
					/*decrease the number of free blocks*/
					this->decFreeBlocks();
				}
				block.merge_left();
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
				if (ptrFreeListArray->delete_address(reinterpret_cast<size_t>(next.getAllocAddress()), next.getBlock(), next.getSize()))
				{
					/*decrease the number of free blocks*/
					this->decFreeBlocks();
				}
				block.merge_right();
				break;
			}
			else
			{
				break;
			}
		}
		if (block.isValid())
		{
			this->setFreeBlockAllocatorsAddress(block.getFreeBlockAddress());
			ptrFreeListArray->push(reinterpret_cast<size_t>(block.getBlock()), block.getSize());			
			this->incFreeBlocks();
		}
		else
		{
			assert(false);
			this->reportError(RTSHA_InvalidBlock);
		}		
		
		this->unlock();
	}

	void PowerTwoMemoryPage::createInitialFreeBlocks()
	{
		/*create initial free blocks*/
		size_t data_size = this->getEndPosition() - this->getStartPosition();
		size_t last_lbit = sizeof(SIZE_MAX) * 8U - 1U;
		size_t val = 1U << last_lbit;
		size_t rest = data_size;
		rtsha_block* prev = nullptr;
		FreeListArray* ptrFreeListArray = reinterpret_cast<FreeListArray*>(this->getFreeListArray());

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

				this->setFreeBlockAllocatorsAddress(block.getFreeBlockAddress());
				ptrFreeListArray->push((size_t)block.getBlock(), block.getSize());
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
