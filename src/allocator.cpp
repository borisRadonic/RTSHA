#include "allocator.h"
#include "Heap.h"

using namespace rtsha;

Heap* _heap = NULL;

PREALLOC_MEMORY<Heap> _heap_obj;
	
bool rtsha_create_heap(void* start, size_t size)
{
	if (_heap == nullptr)
	{
		_heap = new  (_heap_obj.get_ptr()) Heap();
		return _heap->init(start, size);
	}
	return false;
}
	
bool  rtsha_add_page(HeapCallbacksStruct* callbacks, uint16_t page_type, size_t size, size_t max_objects, size_t min_block_size, size_t max_block_size)
{
	if (_heap != nullptr)
	{
		return _heap->add_page(callbacks, (rtsha_page_size_type) page_type, size, max_objects, min_block_size, max_block_size);
	}
	return false;
}
	
void* rtsha_malloc(size_t size)
{
	if (_heap != nullptr)
	{
		return _heap->malloc(size);
	}
	return NULL;
}
	
void rtsha_free(void* ptr)
{
	if (_heap != nullptr)
	{
		_heap->free(ptr);
	}
}
	
void* rtsha_calloc(size_t nitems, size_t size)
{
	if (_heap != nullptr)
	{
		return _heap->calloc(nitems, size);
	}
	return NULL;
}
	
void* rtsha_realloc(void* ptr, size_t size)
{
	if (_heap != nullptr)
	{
		return _heap->realloc(ptr, size);
	}
	return NULL;
}

void* rtsha_memcpy(void* _Dst, void const* _Src, size_t _Size)
{
	if (_heap != nullptr)
	{
		return _heap->memcpy(_Dst, _Src, _Size);
	}
	return NULL;
}

void* rtsha_memset(void* _Dst, int _Val, size_t _Size)
{
	if (_heap != nullptr)
	{
		return _heap->memset(_Dst, _Val, _Size);
	}
	return NULL;
}