#include "pch.h"
#include "internal.h"
#include "allocator.h"
#include <iostream>
#include <sstream>
#include <string>
#include "VisualizePage.h"

using namespace std;

TEST(TestCaseName, TestName)
{
	stringstream textStream;
	
	rtsha_heap_t* heapPtr;
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB for heap
	if (heapMemory != NULL)
	{

	}
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
	rtsha_free(ptr2);
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