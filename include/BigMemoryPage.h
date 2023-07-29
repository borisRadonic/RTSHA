#pragma once
#include <stdint.h>
#include "MemoryPage.h"


namespace internal
{
	using namespace std;

	
	class BigMemoryPage : MemoryPage
	{
	public:
		BigMemoryPage() = delete;

		BigMemoryPage(rtsha_page* page) : MemoryPage(page)
		{
		}

		virtual ~BigMemoryPage()
		{
		}

		virtual void* allocate_block(size_t size) final;

		virtual void free_block(MemoryBlock& block) final;

		void try_merge_left(MemoryBlock& block, bool& merged);

		void try_merge_right(MemoryBlock& block, bool& merged);
	};



}