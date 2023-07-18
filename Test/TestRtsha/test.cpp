#include "pch.h"
#include "internal.h"
#include "allocator.h"
#include <iostream>
#include <sstream>
#include <string>
#include "VisualizePage.h"
#include "FastPlusAllocator.h"
#include "InternMapAllocator.h"
#include "errors.h"


using namespace std;



TEST(TestCaseBtree, TestBtreeSLLRotation)
{
	void* memory = (void*)malloc(65536);

	size_t address = (size_t)memory;
	size_t nodee_size = sizeof(rtsha_free_list_node);


	uint8_t hlist = free_list_create();

	EXPECT_TRUE(hlist != RTSHA_FreeListInvalidHandle);

	/*
	free_list_insert(hlist, address);
	address += nodee_size;

	free_list_insert(hlist, address);
	address += nodee_size;

	free_list_insert(hlist, address);
	address += nodee_size;
	size_t address3 = address;

	free_list_insert(hlist, address);
	address += nodee_size;

	free_list_delete(hlist, address);
	free_list_delete(hlist, address3);
	*/

	free(memory);
}





TEST(TestCaseBtree, TestMyMallocFrteePerformance64)
{	
	multimap_create(0);

	/*
	std::multimap<size_t, size_t, less<size_t>, Mallocator<pair<size_t, size_t>>> map;
	std::multimap<size_t, size_t, less<size_t>, internal::InternMapAllocator<pair<size_t, size_t>>> map2;
	
	std::multimap<size_t, size_t, less<size_t>, Mallocator<pair<size_t, size_t>>>::iterator iter;

	map.insert(pair<size_t, size_t>(1, 40));
	map.insert(pair<size_t, size_t>(40, 50));
	map.insert(pair<size_t, size_t>(41, 51));

	iter = map.find( 40 );
	map.erase(40);
	*/
	rtsha_heap_t* heapPtr;
	size_t size = 100 * 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	/*Initialize heap allocator*/
	heapPtr = rtsha_heap_init(heapMemory, size);

	size_t free_space = rtsha_get_free_space();


	/*Add pages*/
	rtsha_page* pagePtr0 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_64, RTSHA_PAGE_SIZE_64K);
	free_space = rtsha_get_free_space();

	//rtsha_page* pagePtr1 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_16, RTSHA_PAGE_SIZE_64K);
	//free_space = rtsha_get_free_space();

	

	//rtsha_page* pagePtr9 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_BIG, free_space);
	//free_space = rtsha_get_free_space();

	for (int i = 0; i < 100000; i++)
	{
		
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
	}

}


TEST(TestCaseBtree, TestMallocFrteePerformance)
{
	for (int i = 0; i < 100000; i++)
	{
		void* memory1 = (void*)malloc(64);
		void* memory2 = (void*)malloc(128);
		void* memory3 = (void*)malloc(256);
		void* memory4 = (void*)malloc(1024);
		void* memory5 = (void*)malloc(1024);
		void* memory6 = (void*)malloc(10240);
		void* memory7 = (void*)malloc(10);
		void* memory8 = (void*)malloc(15);
		void* memory9 = (void*)malloc(1024);
		free(memory3);
		free(memory5);
		free(memory1);
		free(memory7);
		free(memory9);
		free(memory2);
		free(memory4);
		free(memory6);
		free(memory8);
	}
}


TEST(TestCaseBtree, TestMyMallocFrteePerformance)
{
	rtsha_heap_t* heapPtr;
	size_t size = 100 * 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	/*Initialize heap allocator*/
	heapPtr = rtsha_heap_init(heapMemory, size);

	size_t free_space = rtsha_get_free_space();


	/*Add pages*/
	rtsha_page* pagePtr0 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_16, RTSHA_PAGE_SIZE_64K);
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr1 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_16, RTSHA_PAGE_SIZE_64K);
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
		/*
		void* memory1 = (void*)rtsha_malloc(64);
		void* memory2 = (void*)rtsha_malloc(128);
		void* memory3 = (void*)rtsha_malloc(256);
		*/
		void* memory1 = (void*)rtsha_malloc(1024);
		void* memory2 = (void*)rtsha_malloc(1024);
		void* memory3 = (void*)rtsha_malloc(10240);
		//void* memory7 = (void*)rtsha_malloc(10);
		//void* memory8 = (void*)rtsha_malloc(15);
		void* memory4 = (void*)rtsha_malloc(1024);
		//rtsha_free(memory3);
		rtsha_free(memory2);
		//rtsha_free(memory1);
		//rtsha_free(memory7);
		rtsha_free(memory4);
		//rtsha_free(memory2);
		rtsha_free(memory3);
		rtsha_free(memory1);
		//rtsha_free(memory8);
	}

}


TEST(TestCasePage16, TestName)
{
	stringstream textStream;

	rtsha_heap_t* heapPtr;
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);

	/*Initialize heap allocator*/
	heapPtr = rtsha_heap_init(heapMemory, size);

	size_t free_space = rtsha_get_free_space();

	
	/*Add pages*/
	rtsha_page* pagePtr0 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_16, RTSHA_PAGE_SIZE_64K);
	rtsha_page* pagePtr1 = rtsha_add_page(heapPtr, RTSHA_PAGE_TYPE_16, RTSHA_PAGE_SIZE_64K);
	
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
	
	/*clear first 100*/
	for (uint32_t i = 0; i < 100; i++)
	{
		rtsha_free(ptrMemory[i]);
	}


	EXPECT_EQ(100U, pagePtr0->free_blocks);
	EXPECT_EQ( 2412U, pagePtr0->free );

 	for (uint32_t i = 100; i < 4678; i++)
	{
		rtsha_free(ptrMemory[i]);
	}

	visPage9.print(textStream);

	//EXPECT_EQ(pagePtr0->free, 65504U);

	//EXPECT_EQ(pagePtr1->free, 62496U);


	for (uint32_t i = 0; i < 9000; i++)
	{
		void* ptr = rtsha_malloc(16U);
		if (!ptr)
		{
			EXPECT_EQ(i, 4094U);
			break;
		}
	}

	textStream << std::endl;
	textStream << "Page0 free:" << pagePtr0->free << std::endl;
	textStream << "Page1 free:" << pagePtr1->free << std::endl;

	visPage9.print(textStream);
	cout << textStream.str();
}


TEST(TestCaseName, TestName)
{
	stringstream textStream;
	
	rtsha_heap_t* heapPtr;
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	EXPECT_TRUE(heapMemory != NULL);
	
	/*Initialize heap allocator*/
	heapPtr = rtsha_heap_init(heapMemory, size);

	size_t free_space = rtsha_get_free_space();


	/*Add pages*/
	rtsha_page* pagePtr0 = rtsha_add_page( heapPtr, RTSHA_PAGE_TYPE_16, RTSHA_PAGE_SIZE_64K );
	free_space = rtsha_get_free_space();

	rtsha_page* pagePtr1 = rtsha_add_page( heapPtr, RTSHA_PAGE_TYPE_16, RTSHA_PAGE_SIZE_64K );
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

	/*test best fit -identical*/
	ptr2 = rtsha_malloc(1200);
	textStream << "A 1200" << std::endl;
	visPage9.print(textStream);
		
	textStream << "F 1200" << std::endl;
	rtsha_free(ptr2);
	visPage9.print(textStream);

	/*test create new when no best fit*/
	ptr2 = rtsha_malloc(1204);
	textStream << "A 1204" << std::endl;
	visPage9.print(textStream);

	rtsha_free(ptr2);
	textStream << "F 1204" << std::endl;
	visPage9.print(textStream);
		
	/*test shrink*/
	textStream << "F 2000" << std::endl;
	rtsha_free(ptr4);
	visPage9.print(textStream);
		
	textStream << "F 1300 (should shrink left and right)" << std::endl;
	rtsha_free(ptr3);
	visPage9.print(textStream);

	cout << textStream.str();
	
	textStream << "F 1200" << std::endl;
	rtsha_free(ptr5);
	visPage9.print(textStream);

	textStream << "F 2000" << std::endl;
	rtsha_free(ptr1);
	visPage9.print(textStream);
	
	cout << textStream.str();

  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}