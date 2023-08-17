#include "FreeMap.h"

namespace internal
{	
	FreeMap::FreeMap(rtsha_page* page)
		:_page(page)
	{
		/*create objects on stack using new in place*/		
		_mallocator = new (_storage_allocator.get_ptr())	mmap_allocator(page);
		_ptrMap		= new (_storage_map.get_ptr())			mmap(*_mallocator);
	}
}
