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

#include "SmallFixMemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "internal.h"
#include "errors.h"

namespace rtsha
{
	using namespace internal;

	void* SmallFixMemoryPage::allocate_block(const size_t& size) noexcept
	{
		void* ret = nullptr;
		RTSHA_EXPECTS(_page);
		if( 0U == size )
		{
			return nullptr;
		}

		this->lock();

		/*try to use next free block*/
		FreeList* ptrList = reinterpret_cast<FreeList*>(this->getFreeList());
		size_t address = ptrList->pop();
		if (address > 0U)
		{
			this->decFreeBlocks();
			/*set block as allocated*/
			MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)address));
			block.setAllocated();
			//this->decreaseFree(size);
			ret = block.getAllocAddress();
			this->unlock();
			return ret;
		}
		ret = allocate_block_at_current_pos(size);
		this->unlock();
		return ret;
	}

	void SmallFixMemoryPage::free_block(MemoryBlock& block) noexcept
	{
		this->lock();

		/*set as free*/
		block.setFree();

		if (this->isLastPageBlock(block))
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

		FreeList* ptrList = reinterpret_cast<FreeList*>(this->getFreeList());
		this->setFreeBlockAllocatorsAddress(block.getFreeBlockAddress());
		ptrList->push(reinterpret_cast<size_t>(reinterpret_cast<void*>(block.getBlock())));
		this->incFreeBlocks();

		this->unlock();
	}
}
