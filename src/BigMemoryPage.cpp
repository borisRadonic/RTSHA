#include "BigMemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "structures.h"
#include "internal.h"
#include "errors.h"
#include <cstdio>

namespace rtsha
{
	using namespace internal;

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
			size_t address = ptrMap->find(static_cast<const uint64_t>(size));
			if (address != 0U)
			{
				MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)address));
				size_t orig_size = block.getSize();

				if (block.isValid() && (orig_size >= size))
				{
					/*delete used block from the map of free blocks*/
					const uint64_t k = static_cast<const uint64_t>(orig_size);
					if (ptrMap->del(k, reinterpret_cast<size_t>(block.getBlock())))
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
					assert(block.isValid());
					
					/*set block as allocated*/
					block.setAllocated();
										
					return block.getAllocAddress();
				}
			}
		}
		return NULL;
	}

	void BigMemoryPage::free_block(MemoryBlock& block)
	{
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
	
		/*set as free*/
		block.setFree();
						
		if (this->isLastPageBlock(block))
		{
			/*it should never happen -  the last 64B internal block can not be free*/
			assert(false);			
			return;
		}
		
		bool merged(false);
		/*merging loop left*/
		while (block.hasPrev() && (this->getFreeBlocks() > 0U))
		{
			MemoryBlock prev(block.getPrev());
	
			if ( prev.isValid() && prev.isFree() && (prev.getSize() > 0U))
			{
				/*merge two blocks*/
				mergeLeft(block);
				merged = true;				
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

			if (next.isValid() && next.isFree() && !next.isLast() && next.hasPrev() && (next.getPrev() == block.getBlock()))
			{
				/*merge two blocks*/
				//mergeRight(block);
				merged = true;
				break;
			}
			else
			{
				break;
			}
		}
		assert(ptrMap->size() == this->getFreeBlocks());		

		if (block.isValid() && !merged)
		{					
			if (false == ptrMap->exists(static_cast<const uint64_t>(block.getSize()), reinterpret_cast<size_t>(block.getBlock())))
			{
				ptrMap->insert(static_cast<const uint64_t>(block.getSize()), reinterpret_cast<size_t>(block.getBlock()));
				this->incFreeBlocks();
			}			
		}
	}

	void BigMemoryPage::splitBlock(MemoryBlock& block, size_t size)
	{
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());		
			
		block.splitt(size, this->isLastPageBlock(block));
				
		if (block.isValid() && !block.isLast())
		{
			MemoryBlock next(block.getNextBlock());
			if (next.isValid())
			{				
				ptrMap->insert(static_cast<const uint64_t>(next.getSize()), reinterpret_cast<size_t>(next.getBlock()));
				this->incFreeBlocks();
			}
		}
	}

	void BigMemoryPage::mergeLeft(MemoryBlock& block)
	{
		MemoryBlock prev(block.getPrev());

		if (((size_t)block.getBlock() - (size_t)prev.getBlock()) > prev.getSize())
		{
			assert(false);
		}

		if (prev.isFree())
		{			
			FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
			if (ptrMap->del( static_cast<const uint64_t>(prev.getSize()), reinterpret_cast<size_t>(prev.getBlock()) ) )
			{
				/*decrease the number of free blocks*/
				this->decFreeBlocks();	
			}
	
			block.merge_left();

			assert(block.isValid());

			if (block.isValid()) 
			{				
				ptrMap->insert(static_cast<const uint64_t>(block.getSize()), reinterpret_cast<size_t>(block.getBlock()));
				this->incFreeBlocks();
			}
		}
	}

	void BigMemoryPage::mergeRight(MemoryBlock& block)
	{
		MemoryBlock next(block.getNextBlock());
		if (next.isValid() && next.isFree())
		{
			FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
			if (ptrMap->del(static_cast<const uint64_t>(next.getSize()), reinterpret_cast<size_t>(next.getBlock())))
			{
				/*decrease the number of free blocks*/
				this->decFreeBlocks();
			}
			block.merge_right();

			assert(block.isValid());

			if (block.isValid())
			{
				ptrMap->insert(static_cast<const uint64_t>(block.getSize()), reinterpret_cast<size_t>(block.getBlock()));
				this->incFreeBlocks();
			}
		}
	}

	void BigMemoryPage::createInitialFreeBlocks()
	{
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
		/*create one big block (size is page_size-64)*/
		/*this block will be splitted...*/
		size_t* pFirstBlock = reinterpret_cast<size_t*>(this->getPosition());
		*pFirstBlock = 0U;
		MemoryBlock first(reinterpret_cast<rtsha_block*>(pFirstBlock));
		size_t bigSize = this->getEndPosition() - this->getPosition() - 64U;
		first.prepare();
		first.setSize(bigSize);
		first.setAsFirst();
		first.setFree();

		ptrMap->insert((const uint64_t)first.getSize(), (size_t)first.getBlock());

		/*create one 65B blockat the end of page*/
		/*this block will be not used*/
		size_t lastBlock = this->getEndPosition() - 64U;

		size_t* pLastBlock = reinterpret_cast<size_t*>(lastBlock);
		*pLastBlock = 0U;
		MemoryBlock block(reinterpret_cast<rtsha_block*>(pLastBlock));
		block.setSize(64U);
		block.setPrev(first);
		block.setAllocated();
		block.setLast();
		this->setLastBlock(block.getBlock());
		this->setPosition( this->getEndPosition() );
		this->incFreeBlocks();
	}
}