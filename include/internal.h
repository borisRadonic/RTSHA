#pragma once

#include <assert.h>
#include "structures.h"
#include <cstring>
#include <bit>

#define RTSHA_ALIGMENT			sizeof(size_t)	/*4U or 8U*/

#define is_bit(val,n) ( (val >> n) & 0x01U )

#ifndef rtsha_assert
    #define rtsha_assert(x) assert(x)
#endif


#if _WIN32 || _WIN64
    #if defined(WIN64) || defined(__amd64__)
        #define ENV64BIT
    #else
        #define ENV32BIT
    #endif
#else
    #if __GNUC__
        #if __x86_64__ || __ppc64__
            #define ENV64BIT
        #else
            #define ENV32BIT
        #endif
    #endif
#endif


#ifdef __arm__ //ARM architecture
#define ARCH_ARM
#endif

#ifdef __aarch64__ //ARM 64-bit
#define ARCH_ARM
#define ARCH_ARM_64
#define ARCH_64BIT
#endif


namespace internal
{
    using address_t = uintptr_t;

#if defined(WIN64) // The _BitScanReverse64 intrinsic is only available for 64 bit builds because it depends on x64

    inline uint64_t ExpandToPowerOf2(uint64_t Value)
    {
        unsigned long Index;
        _BitScanReverse64(&Index, Value - 1);
        return (1ULL << (Index + 1));
    }
#else

    inline uint32_t ExpandToPowerOf2(uint32_t Value)
    {
        unsigned long Index;
        _BitScanReverse(&Index, Value - 1);
        return (1U << (Index + 1));
    }
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
        static_assert(RTSHA_ALIGMENT > 0);
       
        uintptr_t mask = RTSHA_ALIGMENT - 1U;

        if ((RTSHA_ALIGMENT & mask) == 0U)
        {
            return ((ptr + mask) & ~mask);
        }
        return (((ptr + mask) / RTSHA_ALIGMENT) * RTSHA_ALIGMENT);
    }

}