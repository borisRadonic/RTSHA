

#include "internal.h"
#include <stdint.h>

#pragma once

#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef uint32_t RTSHA_Error;
typedef uint16_t RTSHA_PageType;

#define MAX_BLOCKS_PER_PAGE UINT32_MAX

#define MAX_PAGES			64U



#define RTSHA_PAGE_SIZE_16K			0x4000
#define RTSHA_PAGE_SIZE_24K			0x6000
#define RTSHA_PAGE_SIZE_32K			0x8000
#define RTSHA_PAGE_SIZE_64K			0x10000
#define RTSHA_PAGE_SIZE_128K		0x20000
#define RTSHA_PAGE_SIZE_256K		0x40000
#define RTSHA_PAGE_SIZE_512K		0x80000
#define RTSHA_PAGE_SIZE_1M			0x100000
#define RTSHA_PAGE_SIZE_2M			0x200000
#define RTSHA_PAGE_SIZE_4M			0x400000
#define RTSHA_PAGE_SIZE_8M			0x800000
									

#define RTSHA_PAGE_INTERN	(1U)

#define RTSHA_PAGE_TYPE_16			(16U)
#define RTSHA_PAGE_TYPE_24			(24U)
#define RTSHA_PAGE_TYPE_32			(32U)
#define RTSHA_PAGE_TYPE_64			(64U)
#define RTSHA_PAGE_TYPE_128			(128U)
#define RTSHA_PAGE_TYPE_256			(256U)
#define RTSHA_PAGE_TYPE_512			(512U)

#define RTSHA_PAGE_TYPE_BIG			(4096U)


typedef struct rtsha_block_struct
{
	size_t					   size; /*size is alligned,last 2 bits are not used
										0 bit contains 1 when free
										1 bit contains 1 when last block*/
	struct rtsha_block_struct* prev;
} rtsha_block;

/* RTSHA_BLOCK_HEADER_SIZE - additional size for Free List Node*/
/*Additional bytes are required to store Free Node Data in Free block: ->(size of Free Structure - sizeof(size) - sizeof( rtsha_block_struct*) + Block Size) >= (sizeof(FreeListNode) */

#if defined _WIN64 || defined _ARM64
	#define RTSHA_BLOCK_HEADER_SIZE  (sizeof(size_t) + sizeof(size_t))
#else
	#define RTSHA_BLOCK_HEADER_SIZE  (sizeof(size_t))
#endif


typedef struct rtsha_free_list_struct
{
	struct rtsha_free_list_struct* next;
} rtsha_free_list_node;

typedef struct rtsha_page_struct
{	
	uint8_t						reserved;
	uint8_t						hfree;
	
	uint16_t					flags;
	
	size_t						start_position;
	size_t						size;
	size_t						free;
	size_t						position;

	size_t						free_blocks;
		
	rtsha_block*				last_block;

	size_t						reserved_start;
	size_t						max_blocks;

	struct rtsha_page_struct*	internal_page;

			
	rtsha_free_list_node* free_list;
	rtsha_free_list_node* free_list_last;

	struct rtsha_page_struct*	next;


} rtsha_page;

typedef struct rtsha_heap_struct
{
	size_t							number_pages;
	struct rtsha_page_struct*		pages;
}  rtsha_heap_t;



#endif