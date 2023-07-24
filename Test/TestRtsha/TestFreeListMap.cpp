#include "gtest/gtest.h"
#include "FreeList.h"
#include "FreeMap.h"
#include "MemoryPage.h"
#include "heap.h"

using namespace internal;

TEST(TestFreeList, TestCreatePushPop)
{	
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB

	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageType32, 65536U));
	rtsha_page* page = heap.select_page(rtsha_page_size_type::PageType32, 32);
	heap.free(heap.malloc(20));
		
}
TEST(TestFreeMap, TestCreateInsertDeleteFind)
{
	size_t size = 0x1F4000;
	void* heapMemory = malloc(size); //allocate 2MB

	
	Heap heap;
	EXPECT_TRUE(heap.init(heapMemory, size));
	EXPECT_TRUE(heap.add_page(rtsha_page_size_type::PageTypeBig, 4U * 65536U));
	rtsha_page* page = heap.select_page(rtsha_page_size_type::PageTypeBig, 1024);

	heap.free(heap.malloc(2100));
}