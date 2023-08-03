#include "BigMemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "structures.h"
#include "internal.h"
#include "errors.h"
#include <cstdio>

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
						cout << "a-deleted  " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
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

					cout << "allocated u" << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
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
	
		cout << "free " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
		/*set as free*/
		block.setFree();
				
		if (!block.isValid())
		{
			return;
		}
		/*
		if (block.isLast() && this->isLastPageBlock(block))
		{
			size_t dec = block.getSize();
			if (this->getPosition() == ((size_t)block.getBlock() + dec))
			{
				if (block.hasPrev())
				{
					MemoryBlock prev(block.getPrev());
					if (prev.getNextBlock() == block.getBlock())
					{
						prev.setLast();												
						this->decPosition(dec);
						this->decPosition(prev.getSize());
						this->setLastBlock();
						this->incPosition(prev.getSize());
						MemoryBlock old(block.getBlock());
						old.destroy();
						return;
					}
					else
					{
						return;
					}
				}
			}			
		}
		*/
		bool merged(false);
		/*merging loop left*/
		while (block.hasPrev() && (this->getFreeBlocks() > 0U))
		{
			MemoryBlock prev(block.getPrev());
		
			size_t psize = this->getEndPosition() - this->getStartPosition();
		
			assert(prev.getSize() <= psize);
			

			if ( /*(prev.getSize() < psize) && */prev.isValid() && prev.isFree() && (prev.getSize() > 0U))
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
			if (fitOnPage(block.getSize()))
			{
				MemoryBlock next(block.getNextBlock());

				if (next.isValid() && next.isFree() && (next.getSize() > 0U) && next.hasPrev() && (next.getPrev() == block.getBlock()))
				{
					/*merge two blocks*/
					mergeRight(block);
					merged = true;
					//break;
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

		assert(ptrMap->size() == this->getFreeBlocks());
			
		

		if (block.isValid() && !merged)
		{					
			if (false == ptrMap->exists((const uint64_t)block.getSize(), (size_t)block.getBlock()))
			{
				ptrMap->insert((const uint64_t)block.getSize(), (size_t)block.getBlock());
				this->incFreeBlocks();
			}			
		}		

	
	}

	void BigMemoryPage::splitBlock(MemoryBlock& block, size_t size)
	{
		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
				
		if (block.isLast())
		{
			cout << "splitt last  " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
		}

		cout << "splitt " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
		
			
		block.splitt(size, this->isLastPageBlock(block));


		cout << "splitted " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
		if (block.isValid() && !block.isLast())
		{
			MemoryBlock next(block.getNextBlock());
			if (next.isValid())
			{
				cout << "inserted " << (size_t)next.getBlock() << " size " << next.getSize() << std::endl;
				ptrMap->insert((const uint64_t)next.getSize(), (size_t)next.getBlock());
				this->incFreeBlocks();
			}
		}
	}

	void BigMemoryPage::mergeLeft(MemoryBlock& block)
	{
		MemoryBlock prev(block.getPrev());


		if (block.isLast())
		{
			cout << "merge last  " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
		}

		if (((size_t)block.getBlock() - (size_t)prev.getBlock()) > prev.getSize())
		{
			assert(false);
		}

		if (prev.isFree())
		{
			cout << "block - prev  " << (size_t)block.getBlock() - (size_t) prev.getBlock() << std::endl;

			FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
			if (ptrMap->del((const uint64_t)prev.getSize(), (size_t)prev.getBlock()))
			{
				cout << "m deleted prev " << (size_t)prev.getBlock() << " size " << prev.getSize() << std::endl;
				/*decrease the number of free blocks*/
				this->decFreeBlocks();

				if (ptrMap->del((const uint64_t)block.getSize(), (size_t)block.getBlock()))
				{
					cout << "m deleted block " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
					/*decrease the number of free blocks*/
					this->decFreeBlocks();
				}
						
			}
			else
			{
				assert(false);
			}
			cout << "merge left  2 blocks " << (size_t)prev.getBlock() << " size " << prev.getSize() << " and " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
			block.merge_left();
			cout << "merged left " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;

			assert(block.isValid());

			if (block.isValid()) 
			{
				ptrMap->insert((const uint64_t)block.getSize(), (size_t)block.getBlock());
				cout << "inserted " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
				this->incFreeBlocks();
			}

		}

		//if (block.isLast())
		//{
		//	cout << "set last: " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;			
		//}
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
		cout << "merged right: " << (size_t)next.getBlock() << " size " << next.getSize() << " and left " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
		block.merge_right();
		
	}
}