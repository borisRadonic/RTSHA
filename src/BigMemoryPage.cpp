/******************************************************************************
The MIT License(MIT)

Real Time Safety Heap Allocator (RTSHA)
https://github.com/borisRadonic/RTSHA

Copyright(c) 2023 Boris Radonic

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "BigMemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "internal.h"
#include "errors.h"
#include <cstdio>

namespace rtsha
{
	using namespace internal;

	void* BigMemoryPage::allocate_block(const size_t& size) noexcept
	{
		void* ret(nullptr);
		RTSHA_EXPECTS(_page);
		if(0U == size)
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
					
					size_t diff = orig_size - size;
					if (diff >= (MIN_BLOCK_SIZE_FOR_SPLIT ))
					{						
						this->splitBlock(block, size);
					}

					if (!block.isValid())
					{
						assert(false);
						this->reportError(RTSHA_InvalidBlock);
						this->unlock();
						return ret;						
					}										
					/*set block as allocated*/
					block.setAllocated();
					ret = block.getAllocAddress();
				}
			}
		}
		this->unlock();
		return ret;
	}

	void BigMemoryPage::free_block(MemoryBlock& block) noexcept
	{
		this->lock();

		FreeMap* ptrMap = reinterpret_cast<FreeMap*>(this->getFreeMap());
	
		/*set as free*/
		block.setFree();
						
		if (this->isLastPageBlock(block))
		{
			/*it should never happen -  the last 64B internal block can not be free*/
			assert(false);
			this->unlock();
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
		if ( ptrMap->size() != this->getFreeBlocks() )
		{
			assert(false);
			this->reportError(RTSHA_InvalidNumberOfFreeBlocks);
		}

		if (block.isValid())
		{					
			if (!merged)
			{
				if (false == ptrMap->exists(static_cast<const uint64_t>(block.getSize()), reinterpret_cast<size_t>(block.getBlock())))
				{
					ptrMap->insert(static_cast<const uint64_t>(block.getSize()), reinterpret_cast<size_t>(block.getBlock()));
					this->incFreeBlocks();
				}
			}
		}
		else
		{
			assert(false);
			this->reportError(RTSHA_InvalidBlock);
		}
		this->unlock();
	}

	void BigMemoryPage::splitBlock(MemoryBlock& block, size_t size)  noexcept
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

	void BigMemoryPage::mergeLeft(MemoryBlock& block) noexcept
	{
		MemoryBlock prev(block.getPrev());

		if (((size_t)block.getBlock() - (size_t)prev.getBlock()) > prev.getSize())
		{
			assert(false);
			this->reportError(RTSHA_InvalidBlockDistance);
			return;			
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
		}
	}

	void BigMemoryPage::mergeRight(MemoryBlock& block) noexcept
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
		}
	}

	void BigMemoryPage::createInitialFreeBlocks() noexcept
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

		/*create one 64B blockat the end of page*/
		/*this block will be not used, but will ensure that the page has an appropriate end block*/
		size_t lastBlock = this->getEndPosition() - 64U;

		size_t* pLastBlock = reinterpret_cast<size_t*>(lastBlock);
		*pLastBlock = 0U;
		MemoryBlock block(reinterpret_cast<rtsha_block*>(pLastBlock));
		block.setSize(64U);
		block.setPrev(first);
		block.setAllocated();
		block.setLast();

		MemoryBlock last_block(block.getBlock());
		this->setLastBlock(last_block);
		this->setPosition( this->getEndPosition() );
		this->incFreeBlocks();
	}
}
