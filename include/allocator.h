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

/** \cond */

// This entire file will not be documented by Doxygen.


#include "internal.h"
#include "HeapCallbacks.h"


#if defined(_MSC_VER) || defined(__MINGW32__)

#if !defined(RTSHA_SHARED_LIB)
#define rtsha_decl_export
#elif defined(RTSHA_SHARED_LIB_EXPORT)
#define rtsha_decl_export              __declspec(dllexport)
#else
#define rtsha_decl_export              __declspec(dllimport)
#endif

#define rtsha_cdecl                      __cdecl
#elif defined(__GNUC__)
#if defined(rtsha_SHARED_LIB) && defined(rtsha_SHARED_LIB_EXPORT)
#define rtsha_decl_export              __attribute__((visibility("default")))
#else
#define rtsha_decl_export
#endif
#define rtsha_cdecl
#else
#define rtsha_cdecl
#define rtsha_decl_export
#endif

 

#define RTSHA_PAGE_TYPE_32			32U
#define RTSHA_PAGE_TYPE_64			64U
#define RTSHA_PAGE_TYPE_128			128U	
#define RTSHA_PAGE_TYPE_256			256U
#define RTSHA_PAGE_TYPE_512			512U
#define RTSHA_PAGE_TYPE_BIG			613U
#define RTSHA_PAGE_TYPE_POWER_TWO	713U



#ifdef __cplusplus
extern "C"
{
#endif	
	/*rtsha_decl_nodiscard rtsha_decl_export*/	bool  rtsha_create_heap(void* start, size_t size);
	
	rtsha_decl_export						bool  rtsha_add_page(HeapCallbacksStruct* callbacks, uint16_t page_type, size_t size, size_t max_objects = 0U, size_t min_block_size = 0U, size_t max_block_size = 0U);
	
	rtsha_decl_nodiscard rtsha_decl_export	void* rtsha_malloc(size_t size);

	rtsha_decl_export						void  rtsha_free(void* ptr);

	rtsha_decl_nodiscard rtsha_decl_export	void* rtsha_calloc(size_t nitems, size_t size);

	rtsha_decl_nodiscard rtsha_decl_export	void* rtsha_realloc(void* ptr, size_t size);

	rtsha_decl_export						void* rtsha_memcpy(void* _Dst, void const* _Src, size_t _Size);

	rtsha_decl_export						void* rtsha_memset(void* _Dst, int _Val, size_t _Size);

#ifdef __cplusplus
}
#endif

/** \endcond */
