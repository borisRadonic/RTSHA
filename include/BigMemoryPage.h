#pragma once
#include <stdint.h>
#include "MemoryPage.h"


namespace internal
{
	using namespace std;

	
	class BigMemoryPage : public MemoryPage
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

		void splitBlock(MemoryBlock& block, size_t size);

		void mergeLeft(MemoryBlock& block);

		void mergeRight(MemoryBlock& block);
	};



}