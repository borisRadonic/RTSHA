#pragma once
#include <stdint.h>
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

	};
}
