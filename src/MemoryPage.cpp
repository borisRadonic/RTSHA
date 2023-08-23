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

#include "MemoryPage.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "internal.h"
#include "errors.h"

namespace rtsha
{
	void* MemoryPage::allocate_block_at_current_pos(const size_t& size) noexcept
	{
		if (this->fitOnPage(size))
		{
			MemoryBlock block(reinterpret_cast<rtsha_block*>((void*)this->getPosition()));
			block.prepare(); /*memory can contain old bits*/
			block.setSize(size);	
			if (this->hasLastBlock())
			{
				MemoryBlock last_block(this->getLastBlock());
				block.setPrev(last_block);
				last_block.clearIsLast();
			}
			else
			{
				block.setAsFirst();
			}
			block.setLast();
			this->setLastBlock(block);
			this->incPosition(size);
			//cout << "new allocated " << (size_t)block.getBlock() << " size " << block.getSize() << std::endl;
			return block.getAllocAddress();
		}
		return nullptr;
	}

	bool MemoryPage::checkBlock(size_t address) noexcept
	{
		size_t address2 = address - sizeof(rtsha_block); /*skip size and pointer to prev*/
		MemoryBlock block((rtsha_block*)(void*)address2);
		return block.isValid();
	}
}