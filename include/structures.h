

#include "internal.h"
#include <stdint.h>

#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef uint32_t RTSHA_Error;
typedef uint16_t RTSHA_PageType;

#define RTSHA_PAGE_SIZE_16K			0x4000
#define RTSHA_PAGE_SIZE_32K			0x8000
#define RTSHA_PAGE_SIZE_64K			0x10000
#define RTSHA_PAGE_SIZE_128K		0x20000
#define RTSHA_PAGE_SIZE_256K		0x40000
#define RTSHA_PAGE_SIZE_512K		0x80000

#define RTSHA_PAGE_NEW	(1U)

#define RTSHA_PAGE_TYPE_16			(16U)
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

typedef struct rtsha_page_struct
{	
	uint16_t					reserved;
	uint16_t					flags;
	
	size_t						start_position;
	size_t						size;
	size_t						free;
	size_t						position;
		
	rtsha_block*				last_block;

	size_t						free_blocks;

	struct rtsha_page_struct*	next;
} rtsha_page;


typedef struct rtsha_heap_struct
{
	size_t							number_pages;
	struct rtsha_page_struct*		pages;
}  rtsha_heap_t;

#endif



