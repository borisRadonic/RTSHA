#pragma once

#include <assert.h>
#include "structures.h"

#define RTSHA_ALIGMENT			4U	/*4U or 8U*/
#define RTSHA_PADDING_SIZE		4U	/*4U or 8U*/

#define is_bit(val,n) ( (val >> n) & 0x01U )

#define get_block_size(val) ( (val >> 2U) << 2U )

#ifndef rtsha_assert
    #define rtsha_assert(x) assert(x)
#endif


#define set_block_as_free(b)  ( b->size = b->size | 1U )
#define set_block_as_last(b)  ( b->size = b->size | 2U )

#define MAX(a, b) ((a > b) ? a : b);
#define BALANCE(n) ( n->left->height - n->right->height);

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
uint8_t free_list_create();

bool free_list_insert(uint8_t h, size_t address, rtsha_free_list_node** free_list_ptr, rtsha_free_list_node** last_free_list_ptr);

void free_list_delete(uint8_t h, size_t address, rtsha_free_list_node** free_list_ptr, rtsha_free_list_node** last_free_list_ptr);

rtsha_heap_t* rtsha_heap_init(void* start, size_t size);

rtsha_page* rtsha_add_page(rtsha_heap_t* heap, RTSHA_PageType page_type, size_t size);

uint16_t multimap_create(rtsha_page* page);

bool multimap_insert(uint16_t handle, const size_t key, size_t block);

void multimap_destroy(uint16_t handle);

size_t rtsha_get_free_space();

