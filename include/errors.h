/******************************************************************************
The MIT License(MIT)

Real Time Safety Heap Allocator (RTSHA)
https://github.com/borisRadonic/RTSHA

Copyright(c) 2023 Boris Radonic

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/


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

/** @brief Error code indicating that the block i too small or too big for selected page. */
#define RTSHA_BlockSizeNotAllowed		(512U)

/** @} */ // end of RTSHA_ERRORS group

