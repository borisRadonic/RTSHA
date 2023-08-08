#pragma once
#include <stdint.h>
#include <array>
#include "MemoryPage.h"
#include "structures.h"
#include "errors.h"
#include "FreeList.h"
#include "FreeMap.h"

namespace internal
{
	using namespace std;

	class HeapInternal
	{
	public:
		/**
	* \brief Standard constructor.
	*/
		HeapInternal()
		{
			for (size_t i = 0; i < _pages.size(); i++)
			{
				_pages[i] = nullptr;
			}
		}

		/**
		* \brief Standard destructor.
		*/
		~HeapInternal()
		{

		}


		/**
		* \brief This function creates a 'Free List Object' that will be used for the management of the 'free blocks'
		* Free List object is simple 'forward_list' used wit custom memory allocator
		*  The object is created in the predifined place on the stack using 'new placement' operator.
		*
		* This function is not intended to be used by users of RTSHA library!
		*
		* \param page Pointer to page object's memory.
		*
		*
		* \return On success, a pointer to 'FreeList' object. If the function fails, it returns a null pointer.
		*/
		FreeList* createFreeList(rtsha_page* page);

		/**
		\brief This function creates a 'Free Map Object' that will be used for the management of the 'free blocks'
		* Free Map is C++ Standard Library 'multimap' object that implements a sorted associative container,
		* allowing for fast retrieval of values based on key.
		*
		* The object is created in the predifined place on the stack using 'new placement' operator.
		*
		* This function is not intended to be used by users of RTSHA library!
		*
		*\param page Pointer to page object's memory.
		*
		*
		*\return On success, a pointer to 'FreeList' object.If the function fails, it returns a null pointer.
		*/
		FreeMap* createFreeMap(rtsha_page* page);


	protected:
				
		void init_small_fix_page(rtsha_page* page, size_t a_size);
		void init_power_two_page(rtsha_page* page, size_t a_size, size_t max_objects, size_t min_block_size, size_t max_block_size);
		void init_big_block_page(rtsha_page* page, size_t a_size, size_t max_objects);

	protected:

		std::array<rtsha_page*, MAX_PAGES>	_pages;

		size_t		_number_pages = 0U;
		address_t	_heap_start = NULL;
		size_t		_heap_size = 0U;
		address_t	_heap_current_position = NULL;
		address_t	_heap_top = NULL;
		bool		_heap_init = false;
		RTSHA_Error _last_heap_error = RTSHA_OK;


		/*bytes on stack to store map and list objects created with new inplace*/
		PREALLOC_MEMORY<FreeList, (MAX_SMALL_PAGES + MAX_BIG_PAGES)>	_storage_free_lists = 0U;
		PREALLOC_MEMORY<FreeMap, MAX_BIG_PAGES>		_storage_free_maps = 0U;
	};
}


namespace rtsha
{
	using namespace std;
	using namespace internal;

	class Heap : HeapInternal
	{
	public:

		/**
		* \brief Standard constructor.
		*/
		Heap();


		/**
		* \brief Standard destructor.
		*/
		~Heap();


		/**
		* \brief This function initializes heap.
		*
		*
		* \param start The beginning of heap memory.
		* \param size The size of heap memory.
		* \return Returns true when the heap has been sucessfuly created.
		*/
		bool init(void* start, size_t size);


		/**
		* \brief This function creates memory page and adds it to the heap. RTSHA supports more than one pages per heap.
		*
		* This function takes a singleton instance of the heap.
		*
		* \param callbacks The HeapCallbacksStruct with callbac functions when used. NULL if 'callback' functions are not used. The callback functions for 'lock' and 'unlock' must be specified when used in multithreding enviroment.
		*
		* \param size The size of the page.
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
		bool add_page(HeapCallbacksStruct* callbacks, rtsha_page_size_type size_type, size_t size, size_t max_objects = 0U, size_t min_block_size = 0U, size_t max_block_size = 0U);

		/**
		* \brief This function reurns deww space of the heap.
		*
		* \return Returns the number of free bytes on the heap.
		*/
		size_t get_free_space() const;

		/**
		* \brief This function returns first Big Memory Page page on the heap.
		*
		* \return Returns pointer to rtsha_page structure.
		*/
		rtsha_page* get_big_memorypage() const;

		/**
		* \brief This function returns the page of allocated block.
		*
		* \return Returns pointer to rtsha_page structure or null pointer if fails.
		*/
		rtsha_page* get_block_page(address_t block_address);


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
		void* malloc(size_t size);

		/**
		* \brief This function deallocates memory block.
		*
		* A block of memory previously allocated by a call to rtsha_malloc, rtsha_calloc or rtsha_realloc is deallocated.
		*
		* \param ptr Pointer to a previously allocated memory block. If ptr does not point to a valid block of memory allocated with rtsha_malloc, rtsha_calloc or rtsha_realloc,
		* function does nothing.
		*/
		void free(void* ptr);

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
		void* calloc(size_t nitems, size_t size);

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
		void* realloc(void* ptr, size_t size);

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
		void* memcpy(void* _Dst, void const* _Src, size_t _Size);

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
		void* memset(void* _Dst, int _Val, size_t _Size);


		/**
		* \brief This function returns ideal page type based on size criteria.
		*
		* This function is not intended to be used by users of the library.
		*
		* \param size Size of the memory block, in bytes.
		*
		* \return Returns rtsha_page_size_type.
		*/
		rtsha_page_size_type get_ideal_page(size_t size) const;

		/**
		* \brief This function gets  page from the list of heap pages.
		*
		* This function is not intended to be used by users of the library.
		*
		* \param ideal_page Appropriate 'Page Type'.
		*
		* \param size Size of the memory block, in bytes.
		*
		* \param no_big Indicates to not use Big Memory Page.
		*
		* \return On success, a pointer to memory page or null pointer if the function fails.
		*/
		rtsha_page* select_page(rtsha_page_size_type ideal_page, size_t size, bool no_big = false) const;

	};
}

