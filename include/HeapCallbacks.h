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

#pragma once

/**
 * @brief A function pointer type for 'lock' callback.
 */
typedef void (*rtshLockPagePtr)		(void);

/**
 * @brief A function pointer type for 'unlock' callback.
 */
typedef void (*rtshUnLockPagePtr)	(void);

/**
 * @brief A function pointer type for handling errors related to a page.
 *
 * @param error_code The error code associated with the error.
 */
typedef void (*rtshErrorPagePtr)	(uint32_t);

/**
 * @struct HeapCallbacksStruct
 * @brief Represents a collection of callback functions for heap operations.
 *
 * This structure aggregates function pointers that the heap system can
 * use to perform certain operations, such as locking, unlocking, or error handling.
 */
typedef struct HeapCallbacksStruct
{
	rtshLockPagePtr		ptrLockFunction;		    ///< Function to lock a page.
	rtshLockPagePtr		ptrUnLockFunction;			///< Function to unlock a page.
	rtshErrorPagePtr	ptrErrorFunction;			///< Function to handle errors.
} HeapCallbacks;

