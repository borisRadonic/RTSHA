#include "internal.h"

#pragma once

/** @defgroup RTSHA_ERRORS RTSHA Error Codes
 *  These are the error codes used throughout the RTSHA system.
 *  @{
 */

 /** @brief Represents a successful operation or status. */
#define RTSHA_OK							(0U)

/** @brief Error code indicating an initialization error. */
#define RTSHA_ErrorInit						(16U)

/** @brief Error code indicating an invalid page size during initialization. */
#define RTSHA_ErrorInitPageSize				(32U)

/** @brief Error code indicating an out-of-heap error during initialization. */
#define RTSHA_ErrorInitOutOfHeap			(33U)

/** @brief Error code indicating the system has run out of memory. */
#define RTSHA_OutOfMemory					(64U)

/** @brief Error code indicating no pages are available. */
#define RTSHA_NoPages						(128U)

/** @brief Error code indicating a specific page is not available. */
#define RTSHA_NoPage						(129U)

/** @brief Error code indicating there is no free page available. */
#define RTSHA_NoFreePage					(130U)

/** @brief Error code indicating the memory block is invalid. */
#define RTSHA_InvalidBlock					(256U)

/** @brief Error code indicating an invalid distance between blocks. */
#define RTSHA_InvalidBlockDistance			(257U)

/** @brief Error code indicating an invalid number of free blocks. */
#define RTSHA_InvalidNumberOfFreeBlocks		(258U)

/** @} */ // end of RTSHA_ERRORS group

