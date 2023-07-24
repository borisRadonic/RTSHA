#include "pch.h"
#include "internal.h"
#include "heap.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include "VisualizePage.h"
#include "FastPlusAllocator.h"
#include "InternMapAllocator.h"
#include "errors.h"


using namespace std;
using namespace std::chrono;


TEST(TestCaseClassHeap, TestHeap)
{	
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);
	
	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size) );

	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType16, 65536U));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType24, 65536U));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType32, 65536U));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType64, 65536U));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType128, 65536U));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType256, 65536U));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType512, 65536U));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageTypeBig, 4U * 65536U));

	size_t free = heap.get_free_space();
	EXPECT_EQ(free, 1327104);

	EXPECT_EQ(rtsha_page_size_type::PageType16, heap.get_ideal_page(15U) );
	EXPECT_EQ(rtsha_page_size_type::PageType24, heap.get_ideal_page(17U));
	EXPECT_EQ(rtsha_page_size_type::PageType24, heap.get_ideal_page(24U));
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

	void *mem = heap.malloc(15);
	rtsha_page* page = heap.select_page(rtsha_page_size_type::PageType24, 15);
	EXPECT_EQ(65472, page->free);


	/*
	

	/*Add pages*/
	//rtsha_page* pagePtr0 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_64, RTSHA_PAGE_SIZE_64K);
	//free_space = rtsha_get_free_space();

	//rtsha_page* pagePtr1 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_24, RTSHA_PAGE_SIZE_64K);
	//free_space = rtsha_get_free_space();

	

	//rtsha_page* pagePtr9 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_BIG, free_space);
	//free_space = rtsha_get_free_space();

	//return;

	//for (int i = 0; i < 100000; i++)
	//{
		/*
		void* memory1 = (void*)rtsha_malloc(35);
		void* memory2 = (void*)rtsha_malloc(64);
		void* memory3 = (void*)rtsha_malloc(64);
		void* memory4 = (void*)rtsha_malloc(64);
		void* memory5 = (void*)rtsha_malloc(64);
		void* memory6 = (void*)rtsha_malloc(64);
		void* memory7 = (void*)rtsha_malloc(50);
		void* memory8 = (void*)rtsha_malloc(61);
		void* memory9 = (void*)rtsha_malloc(60);

		rtsha_free(memory3);
		EXPECT_TRUE(pagePtr0->free_blocks == 1);
		
		rtsha_free(memory5);
		EXPECT_TRUE(pagePtr0->free_blocks == 2);

		rtsha_free(memory1);
		EXPECT_TRUE(pagePtr0->free_blocks == 3);

		rtsha_free(memory7);
		EXPECT_TRUE(pagePtr0->free_blocks == 4);

		rtsha_free(memory9);
		EXPECT_TRUE(pagePtr0->free_blocks == 5);

		rtsha_free(memory2);
		EXPECT_TRUE(pagePtr0->free_blocks == 6);

		rtsha_free(memory4);
		EXPECT_TRUE(pagePtr0->free_blocks == 7);

		rtsha_free(memory6);
		EXPECT_TRUE(pagePtr0->free_blocks == 8);

		rtsha_free(memory8);
		EXPECT_TRUE(pagePtr0->free_blocks == 9);
		*/
	//}

}


TEST(TestCaseBtree, TestMallocFrteePerformance)
{
	auto start = high_resolution_clock::now();

	for (int i = 0; i < 100000; i++)
	{
		void* memory1 = (void*)malloc(std::rand() % 31 + 1);
		void* memory2 = (void*)malloc(std::rand() % 31 + 1);
		void* memory3 = (void*)malloc(std::rand() % 31 + 1);
		void* memory4 = (void*)malloc(std::rand() % 31 + 1);
		void* memory5 = (void*)malloc(std::rand() % 31 + 1);
		void* memory6 = (void*)malloc(std::rand() % 31 + 1);
		void* memory7 = (void*)malloc(std::rand() % 31 + 1);
		void* memory8 = (void*)malloc(std::rand() % 31 + 1);
		void* memory9 = (void*)malloc(std::rand() % 31 + 1);
		void* memory10 = (void*)malloc(std::rand() % 31 + 1);
		void* memory11 = (void*)malloc(std::rand() % 31 + 1);

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
/*
	rtsha_heap_t* heapPtr;
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);
*/
	/*Initialize heap allocator*/
	/*
	heapPtr = rtsha_heap_init(heapMemory, size);

	size_t free_space = rtsha_get_free_space();


	
	rtsha_page* pagePtr0 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_32, RTSHA_PAGE_SIZE_256K);
	free_space = rtsha_get_free_space();
	
	auto start = high_resolution_clock::now();
	
	for (int i = 0; i < 100000; i++)
	{		
		void* memory1 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory2 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory3 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory4 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory5 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory6 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory7 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory8 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory9 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory10 = (void*)rtsha_malloc(std::rand() % 31 + 1);
		void* memory11 = (void*)rtsha_malloc(std::rand() % 31 + 1);
	
		rtsha_free(memory1);
		rtsha_free(memory5);
		rtsha_free(memory7);
		rtsha_free(memory3);
		rtsha_free(memory2);
		rtsha_free(memory9);
		rtsha_free(memory10);
		rtsha_free(memory4);
		rtsha_free(memory11);
		rtsha_free(memory6);
		rtsha_free(memory1);
		rtsha_free(memory8);
	}
	*/
	//auto stop = high_resolution_clock::now();
	//auto duration = duration_cast<microseconds>(stop - start);

	//cout << "TestMyMallocSmallMemory took " << duration.count() << " microseconds\n";
}


TEST(TestCaseMyMalloc, TestMallocPerformanceBigBlocks)
{

	for (int i = 0; i < 100000; i++)
	{
		void* memory1 = (void*)malloc(max(512, std::rand() % 1024));
		void* memory2 = (void*)malloc(max(512, std::rand() % 1024));
		void* memory3 = (void*)malloc(max(512, std::rand() % 10240));
		void* memory4 = (void*)malloc(max(512, std::rand() % 50000));
		void* memory5 = (void*)malloc(max(512, std::rand() % 5240));
		void* memory6 = (void*)malloc(max(512, std::rand() % 1000));
		void* memory7 = (void*)malloc(max(512, std::rand() % 4000));

		free(memory5);
		free(memory7);
		free(memory6);
		free(memory2);
		free(memory4);
		free(memory3);
		free(memory1);

	}
}

TEST(TestCaseMyMalloc, TestMyMallocPerformanceBigBlocks)
{
	/*
	rtsha_heap_t* heapPtr;
	size_t size = 0x1F4000 * 100;
	void* heapMemory = malloc(size);
	EXPECT_TRUE(heapMemory != NULL);


	heapPtr = rtsha_heap_init(heapMemory, size);

	size_t free_space = rtsha_get_free_space();

	rtsha_page* pagePtr0 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_24, RTSHA_PAGE_SIZE_64K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr1 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_24, RTSHA_PAGE_SIZE_64K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr3 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_32, RTSHA_PAGE_SIZE_64K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr4 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_32, RTSHA_PAGE_SIZE_64K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr5 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_64, RTSHA_PAGE_SIZE_256K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr6 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_128, RTSHA_PAGE_SIZE_256K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr7 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_256, RTSHA_PAGE_SIZE_256K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr8 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_512, RTSHA_PAGE_SIZE_512K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr9 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_BIG, free_space);
	free_space = rtsha_get_free_space();


	for (int i = 0; i < 100000; i++)
	{
		void* memory1 = (void*)rtsha_malloc(max(513, std::rand() % 1024));
		void* memory2 = (void*)rtsha_malloc(max(513, std::rand() % 1024));
		void* memory3 = (void*)rtsha_malloc(max(513, std::rand() % 10240));
		void* memory4 = (void*)rtsha_malloc(max(513, std::rand() % 50000));
		void* memory5 = (void*)rtsha_malloc(max(513, std::rand() % 5240));
		void* memory6 = (void*)rtsha_malloc(max(513, std::rand() % 1000));
		void* memory7 = (void*)rtsha_malloc(max(513, std::rand() % 4000));

		rtsha_free(memory5);
		rtsha_free(memory7);
		rtsha_free(memory6);
		rtsha_free(memory2);
		rtsha_free(memory4);
		rtsha_free(memory3);
		rtsha_free(memory1);
	}
	*/
}

TEST(TestCasePage16, TestName)
{
	return;
	stringstream textStream;

/*
	rtsha_heap_t* heapPtr;
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size);
	EXPECT_TRUE(heapMemory != NULL);


	
	heapPtr = rtsha_heap_init(heapMemory, size);

	size_t free_space = rtsha_get_free_space();

	
	
	rtsha_page* pagePtr0 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_24, RTSHA_PAGE_SIZE_64K);
	rtsha_page* pagePtr1 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_24, RTSHA_PAGE_SIZE_64K);
	
	VisualizePage visPage9(pagePtr0);
	visPage9.print(textStream);
	
	std::vector<void*> ptrMemory;

	uint32_t max = pagePtr0->free / (16U + sizeof(rtsha_block) );

	for (uint32_t i = 0; i < 5000; i++)
	{
		void* ptr = rtsha_malloc(16U);
		if (ptr)
		{
			ptrMemory.push_back(ptr);
		}
		else
		{
			EXPECT_EQ(i, 4678);
			break;
		}
	}
	*/
	
	/*clear first 100*/
	for (uint32_t i = 0; i < 100; i++)
	{
		//rtsha_free(ptrMemory[i]);
	}
	/*

	EXPECT_EQ(100U, pagePtr0->free_blocks);
	EXPECT_EQ( 2412U, pagePtr0->free );

 	for (uint32_t i = 100; i < 4678; i++)
	{
		rtsha_free(ptrMemory[i]);
	}
	*/
	//visPage9.print(textStream);

	//EXPECT_EQ(pagePtr0->free, 65504U);

	//EXPECT_EQ(pagePtr1->free, 62496U);


	for (uint32_t i = 0; i < 9000; i++)
	{
		//void* ptr = rtsha_malloc(16U);
		//if (!ptr)
		//{
		//	EXPECT_EQ(i, 4094U);
		//	break;
		//}
	}

	//textStream << std::endl;
	//textStream << "Page0 free:" << pagePtr0->free << std::endl;
	//textStream << "Page1 free:" << pagePtr1->free << std::endl;

	//visPage9.print(textStream);
	//cout << textStream.str();
}


TEST(TestCaseName, TestName)
{
	/*
	stringstream textStream;
	
	rtsha_heap_t* heapPtr;
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size);
	EXPECT_TRUE(heapMemory != NULL);
	

	heapPtr = rtsha_heap_init(heapMemory, size);

	size_t free_space = rtsha_get_free_space();


	rtsha_page* pagePtr0 = rtsha_add_page( heapPtr, RTSHA_PAGE_TYPE_24, RTSHA_PAGE_SIZE_64K );
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr1 = rtsha_add_page( heapPtr, RTSHA_PAGE_TYPE_24, RTSHA_PAGE_SIZE_64K );
	free_space = rtsha_get_free_space();
	
	rtsha_page* pagePtr3 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_32, RTSHA_PAGE_SIZE_64K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr4 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_32, RTSHA_PAGE_SIZE_64K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr5 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_64, RTSHA_PAGE_SIZE_256K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr6 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_128, RTSHA_PAGE_SIZE_256K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr7 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_256, RTSHA_PAGE_SIZE_256K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr8 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_512, RTSHA_PAGE_SIZE_512K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr9 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_BIG, free_space);
	free_space = rtsha_get_free_space();

	void* ptr1 = rtsha_malloc(2000);
	void* ptr2 = rtsha_malloc(1200);
	void* ptr3 = rtsha_malloc(1300);
	void* ptr4 = rtsha_malloc(2000);
	void* ptr5 = rtsha_malloc(1200);
	
	VisualizePage visPage9(pagePtr9);
	visPage9.print(textStream);
	
	textStream << "F 1200" << std::endl;
	rtsha_free(ptr2);

	visPage9.print(textStream);
	

	rtsha_free(ptr2);
	*/
	/*test best fit -identical*/
	//ptr2 = rtsha_malloc(1200);
	//textStream << "A 1200" << std::endl;
	//visPage9.print(textStream);
		
	//textStream << "F 1200" << std::endl;
	//rtsha_free(ptr2);
	//visPage9.print(textStream);

	/*
	ptr2 = rtsha_malloc(1204);
	textStream << "A 1204" << std::endl;
	visPage9.print(textStream);

	rtsha_free(ptr2);
	textStream << "F 1204" << std::endl;
	visPage9.print(textStream);
		

	textStream << "F 2000" << std::endl;
	rtsha_free(ptr4);
	visPage9.print(textStream);
		
	textStream << "F 1300 (should shrink left and right)" << std::endl;
	rtsha_free(ptr3);
	visPage9.print(textStream);
	
	textStream << "F 1200" << std::endl;
	rtsha_free(ptr5);
	visPage9.print(textStream);

	textStream << "F 2000" << std::endl;
	rtsha_free(ptr1);
	visPage9.print(textStream);
	
	cout << textStream.str();
	*/

  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}