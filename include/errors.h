#include "structures.h"

#pragma once


#define RTSHA_OK							(0U)
#define RTSHA_ErrorInit						(16U)
#define RTSHA_ErrorInitPageSize				(32U)
#define RTSHA_ErrorInitOutOfHeap			(33U)
#define RTSHA_OutOfMemory					(64U)
#define RTSHA_NoPages						(128U)
#define RTSHA_NoPage						(129U)
#define RTSHA_NoFreePage					(130U)
#define RTSHA_FreeListAllocError			(131U)
#define RTSHA_FreeListInsertError			(132U)
#define RTSHA_FreeListError					(133U)
#define RTSHA_FreeListInvalidHandle			(134U)

#define RTSHA_InvalidBlock					(256U)
#define RTSHA_InvalidBlockDistance			(257U)
#define RTSHA_InvalidNumberOfFreeBlocks		(258U)
	

