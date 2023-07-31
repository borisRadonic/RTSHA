#pragma once
#include <stdint.h>
#include "internal.h"
#include "MemoryPage.h"

namespace internal
{
	using namespace std;

	class PowerTwoMemoryPage : public MemoryPage
	{
	public:
		PowerTwoMemoryPage() = delete;

		PowerTwoMemoryPage(rtsha_page* page) : MemoryPage(page)
		{
		}

		virtual ~PowerTwoMemoryPage()
		{
		}

		virtual void* allocate_block(size_t size) final;

		virtual void free_block(MemoryBlock& block) final;

		void splitBlockPowerTwo(MemoryBlock& block, size_t end_size);

		void mergeLeft(MemoryBlock& block);

		void mergeRight(MemoryBlock& block);

		inline size_t getBinArea(MemoryBlock& block)
		{
			unsigned long result(0U);
			size_t pos = reinterpret_cast<size_t>(block.getBlock());
			size_t end_pos = this->getEndPosition();
			if ((end_pos - pos) <= 64U)
			{
				return 0U;
			}
			size_t diff = (end_pos - pos - 64U);
#ifdef ENV32BIT
			_BitScanReverse(&result, static_cast<unsigned long>(diff));
#else
			_BitScanReverse64(&result, static_cast<unsigned long>(diff));
#endif
			return result;
		}

	};
}
