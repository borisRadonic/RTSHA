#include "allocator.h"
#include "errors.h"
#include "InternListAllocator.h"
#include <cstdlib>
#include <xstddef>
#include <forward_list>
#include <utility>

using namespace internal;

static InternListAllocator<std::size_t>* _lallocators[64];
using flist = std::forward_list<size_t, InternListAllocator<size_t>>;

flist* _lists[MAX_PAGES];

static size_t _internal_list_storage[MAX_PAGES]; /*only sizeof(size_t) per list is used by first allocate call after construction*/

static uint8_t _last_list = 0U;

uint8_t list_create(rtsha_page* page)
{
	uint8_t ret = _last_list;
	using namespace std;
	if ((_last_list >= MAX_PAGES) || (page == NULL))
	{
		return MAX_PAGES;
	}

	_lallocators[_last_list] = new InternListAllocator<size_t>(page, &(_internal_list_storage[_last_list]) );
	_lists[_last_list] = new flist(InternListAllocator<size_t>(*_lallocators[_last_list]));
	
	_last_list++;
	return ret;
}

bool list_push(uint16_t handle, const size_t address)
{
	if (handle < _last_list)
	{
		_lists[handle]->emplace_front(address);
		return true;
	}
	return false;
}

size_t list_pop(uint16_t handle)
{	
	if (handle < _last_list)
	{
		flist::iterator ptr = _lists[handle]->begin();
		if (ptr != _lists[handle]->end())
		{
			size_t address = _lists[handle]->front();
			_lists[handle]->pop_front();
			return address;
		}
	}
	return 0U;
}


