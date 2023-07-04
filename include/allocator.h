#pragma once

#include "internal.h"


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



#ifdef __cplusplus
extern "C"
{
#endif

	rtsha_decl_export void* rtsha_malloc(size_t size);
	rtsha_decl_export void rtsha_free(void* ptr);
	rtsha_decl_export void* rtsha_calloc(size_t nitems, size_t size);
	rtsha_decl_export void* rtsha_realloc(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif
