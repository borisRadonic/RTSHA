#include "FreeList.h"

namespace internal
{
	FreeList::FreeList(rtsha_page* page)
		:_page(page)
	{

		
#ifdef USE_STL_LIST
		/*create objects on stack using new in place*/
		_lallocator = new (_storage_allocator.get_ptr()) InternListAllocator<size_t>( page, reinterpret_cast<size_t*>(&_internal_list_storage));
		ptrLlist = new (_storage_list.get_ptr()) flist(*_lallocator);
#else
		ptrLlist = new (_storage_list.get_ptr()) flist(page);
#endif
	}
}
