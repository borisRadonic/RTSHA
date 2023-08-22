#include "pch.h"
#include "internal.h"
#include "allocator.h"
#include "heap.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include "VisualizePage.h"
#include "FastPlusAllocator.h"
#include "InternMapAllocator.h"
#include "errors.h"
#include "BigMemoryPage.h"
#include "PowerTwoMemoryPage.h"
#include "time.h"

using namespace std;
using namespace std::chrono;

TEST(TestCaseClassHeap, TestHeap)
{	
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);
	
	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size) );

	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType32, 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType64, 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType128, 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType256, 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType512, 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageTypeBig, 4U * 65536U));

	size_t free = heap.get_free_space();
	//EXPECT_EQ(free, 1327104);


	EXPECT_EQ(rtsha_page_size_type::PageType32, heap.get_ideal_page(25U));
	EXPECT_EQ(rtsha_page_size_type::PageType32, heap.get_ideal_page(32U));
	EXPECT_EQ(rtsha_page_size_type::PageType64, heap.get_ideal_page(50U));
	EXPECT_EQ(rtsha_page_size_type::PageType64, heap.get_ideal_page(64U));
	EXPECT_EQ(rtsha_page_size_type::PageType128, heap.get_ideal_page(120U));
	EXPECT_EQ(rtsha_page_size_type::PageType128, heap.get_ideal_page(128U));
	EXPECT_EQ(rtsha_page_size_type::PageType256, heap.get_ideal_page(129U));
	EXPECT_EQ(rtsha_page_size_type::PageType256, heap.get_ideal_page(256U));
	EXPECT_EQ(rtsha_page_size_type::PageType512, heap.get_ideal_page(500U));
	EXPECT_EQ(rtsha_page_size_type::PageType512, heap.get_ideal_page(512U));
	EXPECT_EQ(rtsha_page_size_type::PageTypeBig, heap.get_ideal_page(513U));

	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType32, 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType64, 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType128, 65536U));



	rtsha_page* page32 = heap.select_page(rtsha_page_size_type::PageType32, 25);
	rtsha_page* page64 = heap.select_page(rtsha_page_size_type::PageType64, 50);
	rtsha_page* page128 = heap.select_page(rtsha_page_size_type::PageType128, 80);
	


	for (int i = 0; i < 100000; i++)
	{		
		void* memory1 = heap.malloc(11); /*32*/
		EXPECT_TRUE(memory1 != nullptr);

		void* memory2 = heap.malloc(10); /*32*/
		EXPECT_TRUE(memory2 != nullptr);

		void* memory3 = heap.malloc(21); /*64*/
		EXPECT_TRUE(memory3 != nullptr);

		void* memory4 = heap.malloc(20); /*32*/
		EXPECT_TRUE(memory4 != nullptr);

		void* memory5 = heap.malloc(40); /*64*/
		EXPECT_TRUE(memory5 != nullptr);

		void* memory6 = heap.malloc(50); /*64*/
		EXPECT_TRUE(memory6 != nullptr);

		void* memory7 = heap.malloc(50); /*64*/
		EXPECT_TRUE(memory7 != nullptr);

		void* memory8 = heap.malloc(48); /*64*/
		EXPECT_TRUE(memory8 != nullptr);

		void* memory9 = heap.malloc(80); /*128*/
		EXPECT_TRUE(memory9 != nullptr);

		heap.free(memory1);
		EXPECT_TRUE(page32->free_blocks == 1);
		
		heap.free(memory4);
		EXPECT_TRUE(page32->free_blocks == 2);

		heap.free(memory2);
		EXPECT_TRUE(page32->free_blocks == 3);

		heap.free(memory3);
		EXPECT_TRUE(page64->free_blocks == 1);

		heap.free(memory7);
		EXPECT_TRUE(page64->free_blocks == 2);

		heap.free(memory8);
		EXPECT_TRUE(page64->free_blocks == 3);

		heap.free(memory6);
		EXPECT_TRUE(page64->free_blocks == 4);

		heap.free(memory5);
		EXPECT_TRUE(page64->free_blocks == 5);

		heap.free(memory9);
		EXPECT_TRUE(page128->free_blocks == 1);
	}
}


TEST(TestCaseClassHeap, TestHeapCreatePowerTwoPage)
{
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size));


	
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageTypePowerTwo, 4U * 65536U, 100U, 32U, 2048U ));

	size_t free = heap.get_free_space();
	
	rtsha_page* page = heap.select_page(rtsha_page_size_type::PageTypePowerTwo, 64, true);

	void* memory1 = heap.malloc(50U);
	EXPECT_TRUE(memory1 != nullptr);

	/*first splitt*/
	void* memory2 = heap.malloc(51U);
	EXPECT_TRUE(memory2 != nullptr);

	
	void* memory3 = heap.malloc(51U);
	EXPECT_TRUE(memory3 != nullptr);

	/*second splitt*/
	void* memory4 = heap.malloc(51U);
	EXPECT_TRUE(memory4 != nullptr);

	 heap.free(memory1);

	heap.free(memory3);

	/*merge left*/
	heap.free(memory2);

	heap.free(memory1);

	/*merge left....*/
	heap.free(memory4);

	/*All blocks free as at the beginning and there are no fragmented blocks*/

	memory1 = heap.malloc(50U);
	EXPECT_TRUE(memory1 != nullptr);

	/*first splitt*/
	memory2 = heap.malloc(51U);
	EXPECT_TRUE(memory2 != nullptr);


	memory3 = heap.malloc(51U);
	EXPECT_TRUE(memory3 != nullptr);

	/*second splitt*/
	memory4 = heap.malloc(51U);
	EXPECT_TRUE(memory4 != nullptr);

	void* memory5 = heap.malloc(51U);
	EXPECT_TRUE(memory4 != nullptr);

	heap.free(memory1);

	/*merge right*/
	heap.free(memory2);

	heap.free(memory3);

	/*merge right ...*/
	heap.free(memory4);

	heap.free(memory5);

	/*at the end one block /64) is fragmented  ...F8192 F1024 F256 F128 'F64' F256*/

}


TEST(TestCaseMyMalloc, TestMyMallocPerformancePowerTwo1)
{
	size_t size = 0x1F4000*10;
	void* heapMemory = malloc(size); //allocate 20MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size));

	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageTypePowerTwo, size, 8192U, 1024, 0xFA0000));

	rtsha_page* p2 = heap.select_page(rtsha_page_size_type::PageTypePowerTwo, 1024, true);


	PowerTwoMemoryPage page(p2);

	size_t size1 = max(1024, std::rand() % 1024);
	void* memory1 = heap.malloc(size1);
	if (memory1 == nullptr)
	{

	}


}

TEST(TestCaseMyMalloc, TestMyMallocPerformancePowerTwo)
{
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size));


	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageTypePowerTwo, size, 0U, 32U, 2048U));

	rtsha_page* p2 = heap.select_page(rtsha_page_size_type::PageTypePowerTwo, 64, true);

	PowerTwoMemoryPage page(p2);


	for (int i = 0; i < 100000; i++)
	{
		size_t size1 = max(64, std::rand() % 2000);
		void* memory1 = heap.malloc(size1);
		if (memory1 == nullptr)
		{
			break;
		}
		heap.memset(memory1, 1, size1);
		EXPECT_TRUE(memory1 != nullptr);

		EXPECT_TRUE(page.checkBlock((size_t)memory1));


		size_t size2 = max(64, std::rand() % 2000);
		void* memory2 = heap.malloc(size2);
		EXPECT_TRUE(memory2 != nullptr);
		if (memory2 == nullptr)
		{
			break;
		}
		heap.memset(memory2, 2, size2);

		size_t size3 = max(64, std::rand() % 2000);
		void* memory3 = heap.malloc(size3);
		EXPECT_TRUE(memory3 != nullptr);
		if (memory3 == nullptr)
		{
			break;
		}

		heap.memset(memory3, 3, size3);
		EXPECT_TRUE(page.checkBlock((size_t)memory1));
		EXPECT_TRUE(page.checkBlock((size_t)memory2));


		size_t size4 = max(64, std::rand() % 2000);

		void* memory4 = heap.malloc(size4);
		EXPECT_TRUE(memory4 != nullptr);
		if (memory4 == nullptr)
		{
			break;
		}
		heap.memset(memory4, 4, size4);

		size_t size5 = max(128, std::rand() % 2000);
		void* memory5 = heap.malloc(size5);
		EXPECT_TRUE(memory5 != nullptr);
		if (memory5 == nullptr)
		{
			break;
		}
		heap.memset(memory5, 5, size5);


		size_t size6 = max(256, std::rand() % 2000);
		void* memory6 = heap.malloc(size6);
		EXPECT_TRUE(memory6 != nullptr);
		if (memory6 == nullptr)
		{
			break;
		}
		heap.memset(memory6, 6, size6);


		size_t size7 = max(1024, std::rand() % 2000);
		void* memory7 = heap.malloc(size7);
		EXPECT_TRUE(memory7 != nullptr);
		if (memory7 == nullptr)
		{
			break;
		}
		heap.memset(memory7, 7, size7);

		EXPECT_TRUE(page.checkBlock((size_t)memory1));
		EXPECT_TRUE(page.checkBlock((size_t)memory2));
		EXPECT_TRUE(page.checkBlock((size_t)memory3));
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory5));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));



		heap.free(memory1);
		EXPECT_TRUE(page.checkBlock((size_t)memory2));
		EXPECT_TRUE(page.checkBlock((size_t)memory3));
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory5));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory2);
		EXPECT_TRUE(page.checkBlock((size_t)memory3));
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory5));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory3);
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory5));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory5);

		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory6);
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));


		heap.free(memory7);
		EXPECT_TRUE(page.checkBlock((size_t)memory4));

		heap.free(memory4);


	}
}
TEST(TestCaseClassHeap, TestBlockMergeLeft)
{
	return;
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size));

	
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageTypeBig, 16U * 65536U));
		

	rtsha_page* page_big = heap.select_page(rtsha_page_size_type::PageTypeBig, 80);


	void* memory1 = heap.malloc(1024-8);
	EXPECT_TRUE(memory1 != nullptr);

	void* memory2 = heap.malloc(2048-8);
	EXPECT_TRUE(memory2 != nullptr);

	void* memory3 = heap.malloc(4096-8);
	EXPECT_TRUE(memory3 != nullptr);

	void* memory4 = heap.malloc(8192-8);
	EXPECT_TRUE(memory4 != nullptr);

	void* memory5 = heap.malloc(16384-8);
	EXPECT_TRUE(memory5 != nullptr);

	void* memory6 = heap.malloc(1024-8);
	EXPECT_TRUE(memory6 != nullptr);

	void* memory7 = heap.malloc(2048-8);
	EXPECT_TRUE(memory7 != nullptr);

	void* memory8 = heap.malloc(4096-8);
	EXPECT_TRUE(memory8 != nullptr);

	void* memory9 = heap.malloc(8192-8);
	EXPECT_TRUE(memory9 != nullptr);

	/*delete the block before last block*/
	heap.free(memory8);
			
	/*delete the last block*/
	/*it must merge the previous free block on the left side */
	heap.free(memory9);

	size_t block_size = page_big->last_block->size;
	
	/*it must merge the last free block on the right side */
	heap.free(memory7);


	heap.free(memory2);
		
	heap.free(memory3);
	

	heap.free(memory6);
		

	heap.free(memory5);

}

TEST(TestStandardMalloc, TestStandardMallocSmallMemory)
{
	auto start = high_resolution_clock::now();

	for (int i = 0; i < 100000; i++)
	{
		void* memory1 = (void*)malloc(std::rand() % 24 + 1);
		void* memory2 = (void*)malloc(std::rand() % 24 + 1);
		void* memory3 = (void*)malloc(std::rand() % 24 + 1);
		void* memory4 = (void*)malloc(std::rand() % 24 + 1);
		void* memory5 = (void*)malloc(std::rand() % 24 + 1);
		void* memory6 = (void*)malloc(std::rand() % 24 + 1);
		void* memory7 = (void*)malloc(std::rand() % 24 + 1);
		void* memory8 = (void*)malloc(std::rand() % 24 + 1);
		void* memory9 = (void*)malloc(std::rand() % 24 + 1);
		void* memory10 = (void*)malloc(std::rand() % 24 + 1);
		void* memory11 = (void*)malloc(std::rand() % 24 + 1);

		free(memory1);

		free(memory5);

		free(memory7);

		free(memory3);
		free(memory2);
		free(memory9);
		free(memory10);
		free(memory4);
		free(memory11);
		free(memory6);
		
		free(memory8);
	}

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	cout << "TestMallocFrteePerformance took " << duration.count() << " microseconds\n";
}



TEST(TestCaseMyMalloc, TestMyMallocSmallMemory)
{
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size));

	//EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType16, 65536U));
	//EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType24, 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType32, 2U * 65536U));
	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageType64, 2U * 65536U));
		
	auto start = high_resolution_clock::now();
	
	for (int i = 0; i < 100000; i++)
	{		
		void* memory1 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory1 != nullptr);

		void* memory2 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory2 != nullptr);

		void* memory3 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory3 != nullptr);

		void* memory4 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory4 != nullptr);

		void* memory5 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory5 != nullptr);

		void* memory6 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory6 != nullptr);

		void* memory7 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory7 != nullptr);

		void* memory8 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory8 != nullptr);

		void* memory9 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory9 != nullptr);

		void* memory10 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory10 != nullptr);

		void* memory11 = heap.malloc(std::rand() % 24 + 1);
		EXPECT_TRUE(memory11 != nullptr);
	
		heap.free(memory1);

		heap.free(memory5);
		
		heap.free(memory7);
		
		heap.free(memory3);

		heap.free(memory2);

		heap.free(memory9);
		
		heap.free(memory10);
		
		heap.free(memory4);
		
		heap.free(memory11);
		
		heap.free(memory6);
		
		heap.free(memory8);
	}
	
	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "TestMyMallocSmallMemory took " << duration.count() << " microseconds\n";

}


TEST(TestCaseMyMalloc, TestMallocPerformanceBigBlocks)
{
	for (int i = 0; i < 100000; i++)
	{
		size_t size1 = max(513, std::rand() % 10240);
		void* memory1 = malloc(size1);
		if (memory1 == nullptr)
		{
			break;
		}
		memset(memory1, 1, size1);
		EXPECT_TRUE(memory1 != nullptr);
					


		size_t size2 = max(513, std::rand() % 20000);
		void* memory2 = malloc(size2);
		EXPECT_TRUE(memory2 != nullptr);
		if (memory2 == nullptr)
		{
			break;
		}
		memset(memory2, 2, size2);

		size_t size3 = max(513, std::rand() % 10240);
		void* memory3 = malloc(size3);
		EXPECT_TRUE(memory3 != nullptr);
		if (memory3 == nullptr)
		{
			break;
		}

		memset(memory3, 3, size3);
			
		size_t size4 = max(513, std::rand() % 50000);

		void* memory4 = malloc(size4);
		EXPECT_TRUE(memory4 != nullptr);
		if (memory4 == nullptr)
		{
			break;
		}
		memset(memory4, 4, size4);

		size_t size5 = max(513, std::rand() % 5240);
		void* memory5 = malloc(size5);
		EXPECT_TRUE(memory5 != nullptr);
		if (memory5 == nullptr)
		{
			break;
		}
		memset(memory5, 5, size5);


		size_t size6 = max(513, std::rand() % 1000);
		void* memory6 = malloc(size6);
		EXPECT_TRUE(memory6 != nullptr);
		if (memory6 == nullptr)
		{
			break;
		}
		memset(memory6, 6, size6);


		size_t size7 = max(513, std::rand() % 4000);
		void* memory7 = malloc(size7);
		EXPECT_TRUE(memory7 != nullptr);
		if (memory7 == nullptr)
		{
			break;
		}
		memset(memory7, 7, size7);
				
		free(memory1);
		free(memory2);
		free(memory3);
		free(memory5);
		free(memory6);
		free(memory7);
		free(memory4);		
	}
}

TEST(TestCaseInterfaceC, TestCInterface)
{
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	EXPECT_TRUE(rtsha_create_heap(heapMemory, size));

	EXPECT_TRUE(rtsha_add_page(NULL, RTSHA_PAGE_TYPE_BIG, size));

	size_t size1 = max(513, std::rand() % 10240);
	void* memory1 = rtsha_malloc(size1);

	rtsha_free(memory1);
}

TEST(TestCaseMyMalloc, TestMyMallocPerformanceBigBlocks)
{ 
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size));

	EXPECT_TRUE(heap.add_page(NULL, rtsha_page_size_type::PageTypeBig, size));

	rtsha_page* page_big = heap.select_page(rtsha_page_size_type::PageTypeBig, 80);
	BigMemoryPage page(page_big);


	for (int i = 0; i < 100000; i++)
	{
		size_t size1 = max(513, std::rand() % 10240);
		void* memory1 = heap.malloc(size1 );
		//prefetch(memory1);
		if (memory1 == nullptr)
		{
			break;
		}
		memset(memory1, 1, size1);
		EXPECT_TRUE(memory1 != nullptr);

		EXPECT_TRUE(page.checkBlock((size_t)memory1));


		size_t size2 = max(513, std::rand() % 20000);
		void* memory2 = heap.malloc(size2);
		EXPECT_TRUE(memory2 != nullptr);
		if (memory2 == nullptr)
		{
			break;
		}
		memset(memory2, 2, size2);

		size_t size3 = max(513, std::rand() % 10240);
		void* memory3 = heap.malloc(size3);
		EXPECT_TRUE(memory3 != nullptr);
		if (memory3 == nullptr)
		{
			break;
		}
		
		memset(memory3, 3, size3);
		EXPECT_TRUE(page.checkBlock((size_t)memory1));
		EXPECT_TRUE(page.checkBlock((size_t)memory2));


		size_t size4 = max(513, std::rand() % 50000);

		void* memory4 = heap.malloc(size4);
		EXPECT_TRUE(memory4 != nullptr);
		if (memory4 == nullptr)
		{
 			break;
		}
		memset(memory4, 4, size4);

		size_t size5 = max(513, std::rand() % 5240);
		void* memory5 = heap.malloc(size5);
		EXPECT_TRUE(memory5 != nullptr);
		if (memory5 == nullptr)
		{
			break;
		}
		memset(memory5, 5, size5);


		size_t size6 = max(513, std::rand() % 1000);
		void* memory6 = heap.malloc(size6);
		EXPECT_TRUE(memory6 != nullptr);
		if (memory6 == nullptr)
		{
			break;
		}
		memset(memory6, 6, size6);


		size_t size7 = max(513, std::rand() % 4000);
		void* memory7 = heap.malloc(size7);
		EXPECT_TRUE(memory7 != nullptr);
		if (memory7 == nullptr)
		{			
			break;
		}
		memset(memory7, 7, size7);

		EXPECT_TRUE(page.checkBlock((size_t)memory1));
		EXPECT_TRUE(page.checkBlock((size_t)memory2));
		EXPECT_TRUE(page.checkBlock((size_t)memory3));
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory5));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));



		heap.free(memory1);
		EXPECT_TRUE(page.checkBlock((size_t)memory2));
		EXPECT_TRUE(page.checkBlock((size_t)memory3));
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory5));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory2);
		EXPECT_TRUE(page.checkBlock((size_t)memory3));
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory5));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory3);
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory5));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory5);
		
		EXPECT_TRUE(page.checkBlock((size_t)memory4));
		EXPECT_TRUE(page.checkBlock((size_t)memory6));
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory6);
		EXPECT_TRUE(page.checkBlock((size_t)memory4));	
		EXPECT_TRUE(page.checkBlock((size_t)memory7));

		heap.free(memory7);
		EXPECT_TRUE(page.checkBlock((size_t)memory4));

		heap.free(memory4);
	}		
}
