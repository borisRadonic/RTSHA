#pragma once

#include <assert.h>
#include "structures.h"
#include  <cstring>

#define RTSHA_ALIGMENT			sizeof(size_t)	/*4U or 8U*/

#define is_bit(val,n) ( (val >> n) & 0x01U )

#ifndef rtsha_assert
    #define rtsha_assert(x) assert(x)
#endif

template<typename T, size_t n = 1U>
struct alignas(sizeof(size_t)) PREALLOC_MEMORY
{
public:

    PREALLOC_MEMORY()
    {
    }

    PREALLOC_MEMORY(uint8_t init)
    {
        std::memset(_memory, init, sizeof(_memory));
    }
public:

    inline void* get_ptr()
    {
        return (void*)_memory;
    }

    inline void* get_next_ptr()
    {
        if (_count < n)
        {
            void* ret = (void*)(_memory + _count * sizeof(T));
            _count++;
            return ret;
        }
        return nullptr;
    }
private:
    uint8_t _memory[n * sizeof(T)];
    size_t _count = 0U;
};

static inline uintptr_t rtsha_align(uintptr_t ptr)
{
    uintptr_t mask = RTSHA_ALIGMENT - 1U;

    if ((RTSHA_ALIGMENT & mask) == 0U)
    {
        return ((ptr + mask) & ~mask);
    }
    return (((ptr + mask) / RTSHA_ALIGMENT) * RTSHA_ALIGMENT);   
}