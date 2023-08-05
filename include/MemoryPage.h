#pragma once
#include "internal.h"
#include <stdint.h>
#include "MemoryBlock.h"


namespace rtsha
{
	using namespace std;
	using namespace internal;

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

		PageTypeBig			= 613U,
		PageTypePowerTwo	= 713U
	};
	
	struct rtsha_page
	{
		rtsha_page()		
		{
		}

		address_t					ptr_list_map			= 0U;
		
		uint32_t					flags					= 0U;

		address_t					start_position			= 0U;
		address_t					end_position			= 0U;
		
		address_t					position				= 0U;
		size_t						free_blocks				= 0U;
		
		rtsha_block*				last_block				= NULL;

		address_t					lastFreeBlockAddress	= 0U;

		address_t					start_map_data			= 0U;
		
		rtsha_page*					map_page				= 0U;

		size_t						max_blocks				= 0U;

		size_t						min_block_size			= 0U; /*used with PowerTwo Pages*/
		size_t						max_block_size			= 0U; /*used with PowerTwo Pages*/

		rtsha_page*					next					= NULL;

	};

	class MemoryPage
	{
	public:

		MemoryPage() = delete;

		explicit MemoryPage(rtsha_page* page ) noexcept : _page(page)
		{			
		}

		virtual ~MemoryPage()
		{
		}

		bool checkBlock(size_t address);

		virtual void* allocate_block(size_t size) = 0;
		
		virtual void free_block(MemoryBlock& block) = 0;

		void* allocate_block_at_current_pos(size_t size);

		inline void incFreeBlocks()
		{
			if (_page != nullptr)
			{
				_page->free_blocks++;
			}
		}
				
			
	protected:

		inline void setFreeBlockAllocatorsAddress(size_t address)
		{
			_page->lastFreeBlockAddress = address;
		}

		inline rtsha_page_size_type getPageType() const
		{
			return (rtsha_page_size_type) _page->flags;
		}

		inline void* getFreeList() const
		{
			return reinterpret_cast<void*>(_page->ptr_list_map);
		}

		inline void* getFreeMap() const
		{
			return reinterpret_cast<void*>(_page->ptr_list_map);
		}

		inline size_t getFreeBlocks() const
		{
			if (_page != nullptr)
			{
				return _page->free_blocks;
			}
			return 0U;
		}

		inline size_t getMinBlockSize() const
		{
			if (_page != nullptr)
			{
				return _page->min_block_size;
			}
			return 0U;			
		}

		inline address_t getPosition() const
		{
			if (_page != nullptr)
			{
				return _page->position;
			}
			return 0U;
		}

		inline void setPosition(address_t pos)
		{
			if (_page != nullptr)
			{
				_page->position = pos;
			}
		}

		inline void incPosition(size_t val)
		{
			if (_page != nullptr)
			{
				_page->position += val;
			}
		}

		inline void decPosition(size_t val)
		{
			if (_page != nullptr)
			{
				if (_page->position >= val)
				{
					_page->position -= val;
				}				
			}
		}

		
		inline void decFreeBlocks()
		{
			if ( (_page != nullptr) && (_page->free_blocks > 0U) )
			{
				_page->free_blocks--;
			}
		}
		
		inline address_t getEndPosition() const
		{
			return _page->end_position;
		}

		inline address_t getStartPosition() const
		{
			return _page->start_position;
		}

		inline bool fitOnPage(size_t size) const
		{
			if ((_page->position + size) < (_page->end_position))
			{
				if (_page->start_map_data == 0U)
				{
					return true;
				}
				else
				{
					if ((_page->position + size) < _page->start_map_data)
					{
						return true;
					}
				}
			}
			return false;
		}

		inline bool hasLastBlock() const
		{
			return (_page->last_block != nullptr);
		}

		inline bool isLastPageBlock(MemoryBlock& block) const
		{
			if (this->getPosition() == ((size_t)block.getBlock() + block.getSize()))
			{
				return (block.getBlock() == _page->last_block);
			}
			return false;
		}

		inline rtsha_block* getLastBlock() const
		{
			return _page->last_block;
		}

		inline void setLastBlock(const MemoryBlock& block)
		{
			_page->last_block = block.getBlock();
		}

		rtsha_page* _page;
	};
}

