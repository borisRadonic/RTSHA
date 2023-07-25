#include "MemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "structures.h"
#include "internal.h"
#include "errors.h"

namespace internal
{
	void* MemoryPage::try_alloc_page_block(size_t size)
	{
		/*try to use next free block*/
		FreeList *ptrList = reinterpret_cast<FreeList*> (reinterpret_cast<void*>(_page->ptr_list_map));
		size_t address = ptrList->pop();
		if (address > 0U)
		{			
			if (getFreeBlocks() >= 1U)
			{
				_page->free_blocks -= 1U;
			}
			/*set block as allocated*/
			MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)address));
			block.setAllocated();

			_page->free = _page->free - size;
			return block.getAllocAddress();
		}
		return allocate_block_at_current_pos(size);
	}

	void* MemoryPage::try_alloc_big_page_block(size_t size)
	{
		FreeMap* ptrMap = reinterpret_cast<FreeMap*> (reinterpret_cast<void*>(_page->ptr_list_map));
		if ( (_page->free_blocks > 0U) || (ptrMap->size() > 0U) )
		{			
			size_t address = ptrMap->find((const uint64_t)size);
			if (address != 0U)
			{
				MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)address));
				size_t orig_size = block.getSize();
								
				if ( block.isValid() && (orig_size >= size))
				{
					size_t diff = orig_size - size;
					diff = rtsha_align(diff);
					if (diff >= MIN_BLOCK_SIZE_FOR_SPLIT)
					{
						/*splitt block*/
						MemoryBlock tempNode(block.splitt(size));
						/*insert new block to the map of free blocks*/


						ptrMap->insert((const uint64_t)tempNode.getSize(), (size_t)tempNode.getBlock());												
					}
					/*delete used block from the map of free blocks*/
					
					ptrMap->del(( const uint64_t)orig_size, (size_t)block.getBlock());
					_page->free_blocks -= 1U;

					/*set block as allocated*/
					block.setAllocated();
					
					_page->free = _page->free - size;
					return block.getAllocAddress();
				}
				return NULL;
			}
		}
		return allocate_block_at_current_pos(size);
	}

	void* MemoryPage::allocate_block_at_current_pos(size_t size)
	{
		if( !fitOnPage(size) )
		{
			return NULL;
		}
		
		MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)_page->position));
		block.setSize(size);

		if (_page->last_block != nullptr)
		{
			MemoryBlock last_block(_page->last_block);
			block.setPrev(last_block);
		}
		
		_page->last_block = block.getBlock();

		_page->position += size;
		_page->free = _page->free - size;
		
		return block.getAllocAddress();
	}

	void* MemoryPage::allocate_page_block(size_t size)
	{
		if ((0U == size) || (NULL == _page))
		{
			return NULL;
		}

		if (_page->flags == (uint16_t)rtsha_page_size_type::PageTypeBig)
		{
			return try_alloc_big_page_block(size);
		}
		return try_alloc_page_block(size);
	}

	void MemoryPage::free_block(MemoryBlock& block)
	{
		_page->free = _page->free + block.getSize();

		/*set as free*/
		block.setFree();

		if (isLastPageBlock(block))
		{
			block.setLast();
		}

		/*shrink only for big variable size */
		if (_page->flags == (uint16_t)rtsha_page_size_type::PageTypeBig)
		{
			if (getFreeBlocks() > 0U)
			{
				//try_merge_left(block);
			}
			if (getFreeBlocks() > 0U)
			{
				/*check if not last block*/
		
				if ( !block.isLast() && !isLastPageBlock(block) )
				{
					try_merge_right(block);
				}
			}
			if (getFreeBlocks() > 0U)
			{
				//try_merge_left(block);
			}			
			FreeMap* ptrMap = reinterpret_cast<FreeMap*> (reinterpret_cast<void*>(_page->ptr_list_map));
			ptrMap->insert((const uint64_t)block.getSize(), (size_t)block.getBlock());
			_page->free_blocks++;
			return;
		}
		_page->free_blocks += 1U;
		_page->lastFreeBlockAddress = block.getFreeBlockAddress();
		FreeList* ptrList = reinterpret_cast<FreeList*> (reinterpret_cast<void*>(_page->ptr_list_map));
		ptrList->push( reinterpret_cast<size_t>(reinterpret_cast<void*>(block.getBlock())) );
	}

	void MemoryPage::try_merge_left(MemoryBlock& block)
	{
		rtsha_block* pBlock = block.getBlock();
		
		size_t temp_size = 0U;
		rtsha_block* pLastFree = pBlock;

		/*merge all free neighbor blocks left together*/
		while ( (pBlock->prev != NULL) )
		{
			MemoryBlock myBlock(pBlock);
			/*left is free -> shrink those two blocks*/
			MemoryBlock prev(pBlock->prev);
			if ( !prev.isValid() || !prev.isFree() )
			{
				break;
			}
			temp_size = prev.getSize();
			prev.setSize(temp_size + myBlock.getSize());

			/*remove prev. block from map of the free blocks*/
			FreeMap* ptrMap = reinterpret_cast<FreeMap*> (reinterpret_cast<void*>(_page->ptr_list_map));
			ptrMap->del( (const uint64_t)temp_size, (size_t)prev.getBlock());

			if (this->getFreeBlocks() >= 1U)
			{
				_page->free_blocks -= 1U;
			}
			
			/*set as free*/
			if (prev.getSize() > 0U)
			{
				if (pBlock == _page->last_block)
				{
					/*set as last block*/
					setLastBlock(prev);
					prev.setLast();
				}
				prev.setFree();

				pLastFree = pBlock->prev;

				/*update previous of the block on the right side*/
				if (!isLastPageBlock(myBlock))
				{
					MemoryBlock temp_block((rtsha_block*)((size_t)((void*)pBlock) + myBlock.getSize()));
					if (temp_block.isValid() && temp_block.hasPrev())
					{
						temp_block.setPrev(prev);
					}
				}
			}
			pBlock = pBlock->prev;
		}
		block = MemoryBlock(pLastFree);
	}

	void MemoryPage::try_merge_right(MemoryBlock& block)
	{		
		size_t size = block.getSize();
		rtsha_block* pBlock = block.getBlock();

		if (!isLastPageBlock(block))
		{
			bool condition = true;
			while (condition)
			{
				MemoryBlock temp_block((rtsha_block*)((size_t)((size_t)(void*)pBlock) + size));
				if (temp_block.isValid() || (temp_block.getSize() == 0U) || !temp_block.isFree())
				{
					break;
				}

				/*right is free -> merge two blocks*/
				block.setSize(block.getSize() + temp_block.getSize());
				
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
					if (temp_blockR.isValid())
					{
						temp_blockR.setPrev(temp_block);
					}
				}

				if (this->getFreeBlocks() >= 1U)
				{
					FreeMap* ptrMap = reinterpret_cast<FreeMap*> (reinterpret_cast<void*>(_page->ptr_list_map));
					ptrMap->del((const uint64_t)temp_block.getSize(), (size_t)temp_block.getBlock());
					_page->free_blocks -= 1U;
				}
				temp_block.setSize(0U);

				/*set as free*/
				block.setFree();
				if (block.isLast())
				{
					break;
				}
				size = block.getSize();
				pBlock = (rtsha_block*) ((size_t) ((void*)pBlock) + temp_block.getSize());
			}
		}
	}
}

