
#include "allocator.h"


/*override standard C library Heap functions*/

void* malloc(size_t size)
{
	return rtsha_malloc(size);
}

void * calloc (size_t nitems, size_t size)
{
	return rtsha_calloc( nitems, size );
}

void* realloc(void* ptr, size_t size)
{
	return rtsha_realloc(ptr, size);
}

void free (void *ptr )
{
	rtsha_free(ptr);
}

/*override FreeRTOS Heap functions*/

void *pvPortMalloc( size_t xWantedSize )
{
	return rtsha_malloc( xWantedSize );
}

void vPortFree( void *pv )
{
	return rtsha_free(pv);
}

void vPortInitialiseBlocks( void )
{
	return;
}
