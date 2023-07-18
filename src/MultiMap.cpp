#include "allocator.h"
#include "errors.h"
#include "InternMapAllocator.h"
#include "FastPlusAllocator.h"

#include <cstdlib>
#include <xstddef>
#include <map>
#include <utility>
#include <vector>


using namespace internal;

static InternMapAllocator<std::pair<const size_t, size_t>>* _allocators[64];
using mmap = std::multimap<size_t, size_t, std::less<size_t>, internal::InternMapAllocator<std::pair<const size_t, size_t>>>;

mmap* _maps[64];


//static std::vector<std::unique_ptr<mmap>> _mapsPtr;
static uint32_t _last_map = 0U;

uint16_t multimap_create(rtsha_page* page)
{
	using namespace std;
	if (_last_map >= MAX_PAGES)
	{
		return MAX_PAGES;
	}
	
	_allocators[_last_map]  = new InternMapAllocator<std::pair<const size_t, size_t>>(page);

	_maps[_last_map] = new mmap(*_allocators[_last_map]);

	_last_map++;
}

bool multimap_insert(uint16_t handle, const size_t key,  size_t block)
{
	if (handle < _last_map)
	{
		_maps[handle]->insert(std::pair<const size_t, size_t>(key, block));
		return true;
	}
	return false;
}

void multimap_destroy(uint16_t handle)
{
	if (handle < _last_map)
	{
		delete _maps[handle];
		delete _allocators[handle];
	}
}