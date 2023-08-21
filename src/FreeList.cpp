#include "FreeList.h"
#include <cmath>

namespace internal
{
	FreeList::FreeList(rtsha_page* page)
		:_page(page)
	{
		ptrLlist = new (_storage_list.get_ptr()) flist(page);
	}		
}
