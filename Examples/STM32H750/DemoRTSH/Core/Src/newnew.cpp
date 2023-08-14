#include <new>
#include <cstdlib>
#include "allocator.h"

// global overrides of new and delete


void* operator new (std::size_t sz)
{
    void *mem = rtsha_malloc(sz);
    return mem;
}

void* operator new (std::size_t sz, unsigned int arg)
{
    void *mem = rtsha_malloc(sz);
    return mem;
}

void* operator new[] (std::size_t sz)
{
    void *mem = rtsha_malloc(sz);
    return mem;
}

void* _new(std::size_t sz)
{
    void *mem = rtsha_malloc(sz);
    return mem;
}

void operator delete (void *ptr)
{
	rtsha_free(ptr);  // nullptr-safe
}

void operator delete (void *ptr, unsigned int arg)
{
	rtsha_free(ptr);  // nullptr-safe
}

void operator delete[] (void *ptr)
{
	rtsha_free(ptr);  // nullptr-safe
}

