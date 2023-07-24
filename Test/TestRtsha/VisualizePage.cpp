#include "pch.h"
#include "VisualizePage.h"
#include "MemoryBlock.h"

extern uintptr_t	_heap_start;
extern size_t		_heap_top;
extern RTSHA_Error	_last_heap_error;

void VisualizePage::print(std::stringstream& textStream)
{
	if (_page)
	{
		//textStream << "Size:" << _page->size << std::endl;
		//textStream << "Free:" << _page->free << std::endl;
		//textStream << "Free blocks:" << _page->free_blocks << std::endl;
		//textStream << "Reserved:" << _page->reserved << std::endl;
		/*
		size_t tempPos = _page->start_position;
		rtsha_block* pBlock = (rtsha_block_struct*)tempPos;
		
		while ((pBlock != NULL) )
		{
			if (pBlock->size == 0U)
			{
				break;
			}
			if (pBlock->size < 16U)
			{
				textStream << "00000" << " ";
				break;
			}
			size_t blockDataSize = get_block_size( pBlock->size );
			if (is_bit(pBlock->size, 1))
			{
				textStream << "L";
			}
			if (is_bit(pBlock->size, 0))
			{
				textStream << "F" << blockDataSize << " ";
			}
			else
			{
				textStream << "B" << blockDataSize << " ";
			}			
			tempPos = tempPos + blockDataSize; 
			if (tempPos >= _page->position)
			{
				break;
			}
			if (is_bit(pBlock->size, 1))
			{				
				break;
			}
			pBlock = (rtsha_block*)((void*)tempPos);
		}
	*/
		textStream << std::endl;

	}
}

