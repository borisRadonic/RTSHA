/**

@mainpage Real Time Safety Heap Allocator

@author Boris Radonic

Here is documentation of C Interface RTSHA functions.
*/


/*! \addtogroup RTSHA C Interface
 *
 * \ingroup INT
 * @{
 * \details Documentation of RTSHA C Interface functions.
 *
 */

/**
* \brief Small Memory Page Type (32 bytes together with block header)
*/
#define RTSHA_PAGE_TYPE_32				32U

/**
* \brief Small Memory Page Type (64 bytes together with block header)
*/
#define RTSHA_PAGE_TYPE_64				64U

/**
* \brief Small Memory Page Type (128 bytes together with block header)
*/
#define RTSHA_PAGE_TYPE_128				128U	

/**
* \brief Small Memory Page Type (256 bytes together with block header)
*/
#define RTSHA_PAGE_TYPE_256				256U

/**
* \brief Small Memory Page Type (512 bytes together with block header)
*/
#define RTSHA_PAGE_TYPE_512				512U

/**
* \brief Big Memory Page Type
*/
#define RTSHA_PAGE_TYPE_BIG				613U

/**
* \brief Power Two Memory Page Type
*/
#define RTSHA_PAGE_TYPE_POWER_TWO		713U

/**
* \brief This function creates heap. Only one heap is supported when using 'RTSHA C interface'
*
* This function creates and initializes a singleton instance of the heap.
*
* \param start The beginning of heap memory.
* \param size The size of heap memory.
* \return Returns true when the heap has been sucessfuly created.
*/
bool  rtsha_create_heap(void* start, size_t size);
	
/**
* \brief This function creates memory page and adds it to the heap. RTSHA supports more than one pages per heap.
*
* This function takes a singleton instance of the heap.
*
* \param callbacks The HeapCallbacksStruct with callbac functions when used. NULL if 'callback' functions are not used. The callback functions for 'lock' and 'unlock' must be specified when used in multithreding enviroment.
*
* \param size The size of heap memory.
*
* \param page_type The type of the memory page.
*
* \param max_objects The maximum number of blocks that can exist on the page. This parameter is used exclusively for 'Big Memory Page' and 'Power of Two Memory Page'. For 'Small Fixed Memory Page', this value can be set to 0. 
*
* \param min_block_size The minimal size of the page block. This parameter is used exclusively for 'Power of Two Memory Page'. A value of the parameter will be increased to the nearest value that is a power of 2.
* Please, set to 0 for 'Small Fixed Memory Page' and 'Big Memory Page'.
*
* \param max_block_size The maximum number of blocks that can exist on the page. This parameter is used exclusively for 'Big Memory Page' and 'Power of Two Memory Page'.
* When using 'Power Two Memory Page' a value of the parameter will be increased to the nearest value that is a power of 2.
* For 'Small Fixed Memory Page', this value can be set to 0. 
*
* \return Returns true when the page has been sucessfuly created.
*/
bool  rtsha_add_page(HeapCallbacksStruct* callbacks, uint16_t page_type, size_t size, size_t max_objects = 0U, size_t min_block_size = 0U, size_t max_block_size = 0U);

	
/**
* \brief This function allocates block of memory on the heap.
*
* The heap page will be automatically selected based on criteria such as size and availability.
*
* \param size Size of the memory block, in bytes. If the size is zero, a null pointer will be returned.
*
* \return On success, a pointer to the memory block allocated by the function.
* The type of this pointer is always void*, which can be cast to the desired type of data pointer in order to be dereferenceable.
* If the function failed to allocate the requested block of memory, a null pointer is returned.
*/
void* rtsha_malloc(size_t size);


/**
* \brief This function deallocates memory block.
*
* A block of memory previously allocated by a call to rtsha_malloc, rtsha_calloc or rtsha_realloc is deallocated.
*
* \param ptr Pointer to a previously allocated memory block. If ptr does not point to a valid block of memory allocated with rtsha_malloc, rtsha_calloc or rtsha_realloc,
* function does nothing.
*/
void  rtsha_free(void* ptr);

/**
* \brief This function allocates the block of memory on the heap and initializes it to zero. 
*
* The heap page will be automatically selected based on criteria such as size and availability.
*
* \param nitems  An unsigned integral value which represents number of elements. If the size is zero, a null pointer will be returned.
*
* \param size An unsigned integral value which represents the memory block in bytes. If the size is zero, a null pointer will be returned.
*
* \return On success, a pointer to the memory block allocated by the function or null pointer if allocation fails.
* 
*/
void* rtsha_calloc(size_t nitems, size_t size);


/**
* \brief This function reallocates the block of memory on the heap. 
*
* The heap page will be automatically selected based on criteria such as size and availability.
* The function may move the memory block to a new location.
* The content of the memory block is preserved up to the lesser of the new and old sizes. If the new size is larger, the value of the newly allocated portion is indeterminate.
*
*
* \param ptr Pointer to the memory allocated with 'rtsha_malloc' or 'rtsha_calloc'
*
* \param size An unsigned integral value which represents the memory block in bytes. If the size is zero, a null pointer will be returned.
*
* \return On success, a pointer to the memory block allocated by the function or null pointer if allocation fails.
* 
*/
void* rtsha_realloc(void* ptr, size_t size);


/**
* \brief This function copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination.
*
* Before copying memory from the source to the destination, the function checks if the source and destination memory addresses belong to the heap,
* whether the destination block is valid and not free, and if the size of the destination block is sufficiently large.
* If the destination does not belong to the heap memory, it will simply perform the copy function.
* Standard memcpy function is used.
*
* \param _Dst Pointer to the destination.
*
* \param _Src Pointer to the source.
*
* \param _Size Number of bytes to copy.
*
* \return On success, a pointer to the destination memory, or null pointer if the function fails.
* 
*/
void* rtsha_memcpy(void* _Dst, void const* _Src, size_t _Size);


/**
* \brief This function sets values of num bytes from the location pointed to by _Dst to the specified value.
*
* Before calling standard 'memset' function, this function checks if the destination memory addresses belong to the heap,
* whether the destination block is valid and not free, and if the size of the destination block is sufficiently large.
* If the destination does not belong to the heap memory, it will simply perform the function.
*
* \param _Dst Pointer to the destination.
*
* \param _Val Value to be set.
*
* \param _Size Number of bytes to be set to the specified value.
*
* \return On success, a pointer todestination memory or null pointer if the function fails.
* 
*/
void* rtsha_memset(void* _Dst, int _Val, size_t _Size);

 /*! @} */


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


