#pragma once
#include <stdint.h>
#include "MemoryPage.h"

namespace internal
{
	using namespace std;


	class SmallFixMemoryPage : MemoryPage
	{
	public:
		SmallFixMemoryPage() = delete;

		SmallFixMemoryPage(rtsha_page* page) : MemoryPage(page)
		{
		}

		virtual ~SmallFixMemoryPage()
		{
		}

		virtual void* allocate_block(size_t size) final;

		virtual void free_block(MemoryBlock& block) final;
	};
}
