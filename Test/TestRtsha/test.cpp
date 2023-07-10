#include "pch.h"
#include "internal.h"
#include "allocator.h"
#include <iostream>
#include <sstream>
#include <string>
#include "VisualizePage.h"
#include "brtree.h"


using namespace std;


TEST(TestCaseBtree, TestBtree)
{
	void* memory = (void*) malloc(65536);

	size_t address = (size_t)memory;
	size_t nodee_size = sizeof(rtsha_btree_node);

	HBTREE btree = rtsha_btree_create();

	EXPECT_TRUE(btree != INVALID_BTREE_HANDLE);


	btree_node_insert(btree, address, 13);
	address += nodee_size;
	btree_node_insert(btree, address, 11);
	address += nodee_size;
	btree_node_insert(btree, address, 12);
	address += nodee_size;


	btree_node_insert(btree, address, 80 );
	address += nodee_size;


	btree_node_insert(btree, address, 82);
	address += nodee_size;

	btree_node_insert(btree, address, 44);
	address += nodee_size;

	btree_node_insert(btree, address, 78);
	address += nodee_size;

	btree_node_insert(btree, address, 50);
	address += nodee_size;

	btree_node_insert(btree, address, 32);
	address += nodee_size;

	btree_node_insert(btree, address, 17);
	address += nodee_size;

	btree_node_insert(btree, address, 88);
	address += nodee_size;

	btree_node_insert(btree, address, 84);
	address += nodee_size;

	btree_node_insert(btree, address, 92);
	address += nodee_size;

	//btree_node_delete(btree, address, 78);


	btree_node_insert(btree, address, 11);
	address += nodee_size;

	btree_node_insert(btree, address, 12);
	address += nodee_size;

	btree_node_insert(btree, address, 13);
	address += nodee_size;

	btree_node_insert(btree, address, 13);
	address += nodee_size;

	btree_node_insert(btree, address, 13);
	address += nodee_size;



	
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