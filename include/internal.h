#pragma once

#include <assert.h>
#include "structures.h"

#define RTSHA_ALIGMENT			4U	/*4U or 8U*/
#define RTSHA_PADDING_SIZE		4U	/*4U or 8U*/

#define is_bit(val,n) ( (val >> n) & 0x01U )

#define get_block_size(val) ( (size_t) ( (size_t) val >> 2U) << 2U )

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

rtsha_heap_t* rtsha_heap_init(void* start, size_t size);

rtsha_page* rtsha_add_page(rtsha_heap_t* heap, RTSHA_PageType page_type, size_t size);

void rtsha_free_page_block(rtsha_page* page, void* block);

void* rtsha_allocate_page_block(rtsha_page* page, size_t size);

uint8_t multimap_create(rtsha_page* page);

bool multimap_insert(uint16_t handle, const uint64_t key, size_t block);

void multimap_delete(uint16_t handle, const uint64_t key, size_t block);

size_t multimap_find(uint16_t handle, const uint64_t key);

void multimap_destroy(uint16_t handle);

void multimap_drop_all(uint16_t handle);

uint8_t list_create(rtsha_page* page);

bool list_push(uint16_t handle, const size_t address);

size_t list_pop(uint16_t handle);

size_t rtsha_get_free_space();

