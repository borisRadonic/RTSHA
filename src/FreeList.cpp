#include "FreeList.h"

namespace internal
{
	FreeList::FreeList(rtsha_page* page)
		:_page(page)
	{
		/*create objects on stack using new in place*/
		_lallocator = new (_storage_allocator.get_ptr()) InternListAllocator<size_t>( page, reinterpret_cast<size_t*>(&_internal_list_storage));
		ptrLlist = new (_storage_list.get_ptr()) flist(*_lallocator);
	}
	
	void FreeList::push(const size_t address)
	{
		ptrLlist->emplace_front(address);
	}

	size_t FreeList::pop()
	{
		flist::iterator ptr = ptrLlist->begin();
		if (ptr != ptrLlist->end())
		{
			size_t address = ptrLlist->front();
			ptrLlist->pop_front();
			return address;
		}
		return 0U;
	}
}