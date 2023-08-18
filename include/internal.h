#pragma once

#include <assert.h>
#include <cstring>
#include "stdint.h"

#ifdef _MSC_VER
#  include <immintrin.h>
#  include <intrin.h>
#else

#endif



/**

@mainpage Real Time Safety Heap Allocator

@author Boris Radonic

Here is documentation of RTSHA.

RTSHA Algorithms

There are several different algorithms that can be used for heap allocation supported by RTSHA:

1. Small Fix Memory Pages
This algorithm is an approach to memory management that is often used in specific situations where objects of a certain size are frequently allocated and deallocated. By using of uses 'Fixed chunk size' algorithm greatly simplies the memory allocation process and reduce fragmentation.
The memory is divided into pages of chunks(blocks) of a fixed size (32, 64, 128, 256 and 512 bytes). When an allocation request comes in, it can simply be given one of these blocks. This means that the allocator doesn't have to search through the heap to find a block of the right size, which can improve performance. The free blocks memory is used as 'free list' storage.
Deallocations are also straightforward, as the block is added back to the list of available chunks. There's no need to merge adjacent free blocks, as there is with some other allocation strategies, which can also improve performance.
However, fixed chunk size allocation is not a good fit for all scenarios. It works best when the majority of allocations are of the same size, or a small number of different sizes. If allocations requests are of widely varying sizes, then this approach can lead to a lot of wasted memory, as small allocations take up an entire chunk, and large allocations require multiple chunks.
Small Fix Memory Page is also used internaly by "Power Two Memory Page" and "Big Memory Page" algorithms.

2. Power Two Memory Pages
This is a more complex system, which only allows blocks of sizes that are powers of two. This makes merging free blocks back together easier and reduces fragmentation. A specialised binary search tree data structures (red-black tree) for fast storage and retrieval of ordered information are stored at the end of the page using fixed size Small Fix Memory Page.
This is a fairly efficient method of allocating memory, particularly useful for systems where memory fragmentation is an important concern. The algorithm divides memory into partitions to try to minimize fragmentation and the 'Best Fit' algorithm searches the page to find the smallest block that is large enough to satisfy the allocation.
Furthermore, this system is resistant to breakdowns due to its algorithmic approach to allocating and deallocating memory. The coalescing operation helps ensure that large contiguous blocks of memory can be reformed after they are freed, reducing the likelihood of fragmentation over time.
Coalescing relies on having free blocks of the same size available, which is not always the case, and so this system does not completely eliminate fragmentation but rather aims to minimize it.


3. Big Memory Pages
"Similar to the 'Power Two Memory Page', this algorithm employs the 'Best Fit' algorithm, in conjunction with a 'Red-Black' balanced tree, which offers worst-case guarantees for insertion, deletion, and search times. The only distinction between the 'Power Two Memory Page' and this system is that the memory need not be divided into power-of-two blocks; variable block sizes are permitted. It promptly merges or coalesces memory blocks larger than 512 bytes after they are released.
The use of 'Small Fixed Memory Pages' in combination with 'Power Two Memory Pages' is recommended for all real time systems.
*/


#define MULTITHREADING_SUPPORT


#define MAX_BLOCKS_PER_PAGE UINT32_MAX

#define MAX_SMALL_PAGES			32U
#define MAX_BIG_PAGES			2U
#define MAX_PAGES				(MAX_SMALL_PAGES+MAX_BIG_PAGES)

#if (__MSC_VER >= 1930 )
#define rtsha_attr_inline inline __forceinline
#else
#define rtsha_attr_inline inline 
#endif



#if defined _WIN64 || defined _ARM64
#define RTSHA_BLOCK_HEADER_SIZE  (2 * sizeof(size_t))
#define MIN_BLOCK_SIZE_FOR_SPLIT	56U /*todo*/
#else
#define RTSHA_LIST_ITEM_SIZE  (2 * sizeof(size_t))
#define MIN_BLOCK_SIZE_FOR_SPLIT	512U /*todo*/
#endif

#ifdef __cplusplus
#if (__cplusplus >= 201103L) || (_MSC_VER >= 1930)
#define rtsha_attr_noexcept   noexcept
#else
#define rtsha_attr_noexcept   throw()
#endif
#else
#define rtsha_attr_noexcept
#endif


#if (__cplusplus >= 201103L) || (_MSC_VER >= 1930)
#define rtsha_decl_nodiscard    [[nodiscard]]
#elif (defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__)
#define rtsha_decl_nodiscard    __attribute__((warn_unused_result))
#elif defined(_HAS_NODISCARD)
#else
#define rtsha_decl_nodiscard
#endif





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


#ifdef __arm__ //ARM architecture

    rtsha_attr_inline uint32_t ExpandToPowerOf2(uint32_t Value)
    {
    	unsigned long leading_zeros = __builtin_clz(Value);
        return (1U << (32U - leading_zeros));
    }

#else
    inline uint32_t ExpandToPowerOf2(uint32_t Value)
    {
        unsigned long Index;
        _BitScanReverse(&Index, Value - 1);
        return (1U << (Index + 1));
    }
#endif
#endif


    /**
    * @brief Memory storage template for pre-allocation.
    *
    * This template is designed to pre-allocate memory for objects on the stack.
    *
    * @tparam T Type of the elements the storage will manage.
    * @tparam n Number of elements of type T the storage will manage. Default is 1.
    */
    template<typename T, size_t n = 1U>
    struct alignas(sizeof(size_t)) PREALLOC_MEMORY
    {
    public:

        /**
        * @brief Default constructor.
        */
        PREALLOC_MEMORY()
        {
        }

        /**
        * @brief Constructor that initializes memory with a given value.
        *
        * @param init Value used to initialize the memory.
        */
        PREALLOC_MEMORY(uint8_t init)
        {
            std::memset(_memory, init, sizeof(_memory));
        }

    public:

        /**
        *@brief Retrieves the pointer to the beginning of the memory block.
        *
        * @return Void pointer to the beginning of the memory block.
        */
        inline void* get_ptr()
        {
            return (void*)_memory;
        }

        /**
        * @brief Retrieves the pointer to the next available memory block.
        *
        * It increments the internal count to keep track of utilized memory blocks.
        *
        * @return Void pointer to the next available memory block, or nullptr if no block is available.
        */
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
        uint8_t _memory[n * sizeof(T)]; ///< Pre-allocated memory storage.
        size_t _count = 0U;             ///< Counter for used blocks in the storage.
    };

    /**
     * @brief Aligns a pointer to a defined alignment.
     *
     * This function ensures that the returned address conforms to the `RTSHA_ALIGMENT`.
     *
     * @param ptr Pointer to be aligned.
     * @return An aligned uintptr_t.
     */
    static inline uintptr_t rtsha_align(uintptr_t ptr, size_t aligment)
    {
        uintptr_t mask = aligment - 1U;

        if ((aligment & mask) == 0U)
        {
            return ((ptr + mask) & ~mask);
        }
        return (((ptr + mask) / aligment) * aligment);
    }

}
