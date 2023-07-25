#include "internal.h"
#include <stdint.h>

#pragma once

#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef uint32_t RTSHA_Error;
typedef uint16_t RTSHA_PageType;

#define MAX_BLOCKS_PER_PAGE UINT32_MAX

#define MAX_SMALL_PAGES			32U
#define MAX_BIG_PAGES			2U
#define MAX_PAGES				(MAX_SMALL_PAGES+MAX_BIG_PAGES)

#if defined _WIN64 || defined _ARM64
	#define RTSHA_BLOCK_HEADER_SIZE  (2 * sizeof(size_t))
#define MIN_BLOCK_SIZE_FOR_SPLIT	56U /*todo*/
#else
	#define RTSHA_LIST_ITEM_SIZE  (2 * sizeof(size_t))
	#define MIN_BLOCK_SIZE_FOR_SPLIT	512U /*todo*/
#endif


#endif