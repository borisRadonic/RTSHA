#pragma once
#include <stdint.h>
#include "MemoryBlock.h"

namespace internal
{
	using namespace std;

	enum struct rtsha_page_size_type : uint16_t
	{
		PageTypeNotDefined = 0U, /*not used*/

		PageType16	= 16U,
		PageType24	= 24U,
		PageType32	= 32U,
		PageType64	= 64U,
		PageType128 = 128U,
		PageType256 = 256U,
		PageType512 = 512U,
		PageTypeBig = 4096U
	};
		
	struct rtsha_page
	{
		rtsha_page()		
		{
		}

		size_t						ptr_list_map			= 0U;
		
		uint32_t					flags					= 0U;

		size_t						start_position			= 0U;
		size_t						size					= 0U;
		size_t						free					= 0U;
		size_t						position				= 0U;
		size_t						free_blocks				= 0U;

		rtsha_block*				last_block				= NULL;

		size_t						lastFreeBlockAddress	= 0U;

		rtsha_page*					next					= NULL;

	};

	class MemoryPage
	{
	public:

		MemoryPage() = delete;

		MemoryPage(rtsha_page* page ) : _page(page)
		{
			
		}

		~MemoryPage()
		{
		}

		void* allocate_page_block(size_t size);

		void* allocate_block_at_current_pos(size_t size);

		void* try_alloc_big_page_block(size_t size);

		void* try_alloc_page_block(size_t size);

		void free_block(MemoryBlock& block);

		void try_merge_left(MemoryBlock& block);

		void try_merge_right(MemoryBlock& block);


	
	protected:

		inline size_t getFreeBlocks() const
		{
			if (_page != nullptr)
			{
				return _page->free_blocks;
			}
			return 0U;
		}

		inline bool fitOnPage(size_t size) const
		{
			return ((_page->position + size) < (_page->size + _page->start_position));
		}

		inline bool isLastBlock(MemoryBlock& block) const
		{
			return (block.getBlock() == _page->last_block);
		}

		inline void setLastBlock( const MemoryBlock& block)
		{
			_page->last_block = block.getBlock();
		}


	private:
		rtsha_page* _page;
	};
}

