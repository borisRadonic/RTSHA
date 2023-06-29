#pragma once

#include "structures.h"

#define RTSHA_ALIGMENT			4U	/*4U or 8U*/
#define RTSHA_PADDING_SIZE		4U	/*4U or 8U*/

#define is_bit(val,n) ( (val >> n) & 0x01U )

#define get_block_size(val) ( (val >> 2U) << 2U )

static inline bool rtsha_is_aligned(void* ptr)
{
     return ( ((uintptr_t) ptr % RTSHA_ALIGMENT) == 0U );
}

static inline uintptr_t rtsha_align(uintptr_t ptr)
{
    uintptr_t mask = RTSHA_ALIGMENT - 1U;

    if ((RTSHA_ALIGMENT & mask) == 0U)
    {
        return ((ptr + mask) & ~mask);
    }
    return (((ptr + mask) / RTSHA_ALIGMENT) * RTSHA_ALIGMENT);
   
}

rtsha_heap_t* rtsha_heap_init(void* start, size_t size);

rtsha_page* rtsha_add_page(rtsha_heap_t* heap, RTSHA_PageType page_type, size_t size);

size_t rtsha_get_free_space();

