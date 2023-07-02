#include "pch.h"
#include "VisualizePage.h"

extern uintptr_t	_heap_start;
extern size_t		_heap_top;
extern RTSHA_Error	_last_heap_error;

void VisualizePage::print(std::stringstream& textStream)
{
	if (_page)
	{
		textStream << "Size:" << _page->size << std::endl;
		textStream << "Free:" << _page->free << std::endl;
		textStream << "Free blocks:" << _page->free_blocks << std::endl;
		//textStream << "Reserved:" << _page->reserved << std::endl;
		
		size_t tempPos = _page->start_position;
		rtsha_block_struct* pBlock = (rtsha_block_struct*)tempPos;
		
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
		textStream << std::endl;

		textStream << "Free List:" << std::endl;
		
		if (_page->free_blocks > 0U)
		{
			size_t* ptrfree = (size_t*)(_heap_top - (_page->free_blocks * sizeof(size_t)));
			pBlock = (rtsha_block*)(*ptrfree);

			for (size_t t = 0; t < _page->free_blocks; t++)
			{
				if (pBlock->size == 0U)
				{
					break;
				}
				
				size_t blockDataSize = get_block_size(pBlock->size);
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

				ptrfree++;
				pBlock = (rtsha_block*)(*ptrfree);

			}
		}

		textStream << std::endl;
		textStream << std::endl;
	}
}

