/******************************************************************************
The MIT License(MIT)

Real Time Safety Heap Allocator (RTSHA)
https://github.com/borisRadonic/RTSHA

Copyright(c) 2023 Boris Radonic

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "allocator.h"
#include "Heap.h"

using namespace rtsha;

Heap* _heap = nullptr;

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
	return nullptr;
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
	return nullptr;
}
	
void* rtsha_realloc(void* ptr, size_t size)
{
	if (_heap != nullptr)
	{
		return _heap->realloc(ptr, size);
	}
	return nullptr;
}

void* rtsha_memcpy(void* _Dst, void const* _Src, size_t _Size)
{
	if (_heap != nullptr)
	{
		return _heap->memcpy(_Dst, _Src, _Size);
	}
	return nullptr;
}

void* rtsha_memset(void* _Dst, int _Val, size_t _Size)
{
	if (_heap != nullptr)
	{
		return _heap->memset(_Dst, _Val, _Size);
	}
	return nullptr;
}