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

