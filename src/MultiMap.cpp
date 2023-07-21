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
using mmap_allocator = InternMapAllocator<std::pair<const uint64_t, size_t>>;

using mmap = std::multimap<const uint64_t, size_t, std::less<const uint64_t>, internal::InternMapAllocator<std::pair<const uint64_t, size_t>>>;

static mmap_allocator* _allocators[MAX_PAGES];
static mmap* _maps[MAX_PAGES];

static uint8_t _storage_allocators[MAX_PAGES * sizeof(mmap_allocator)];
static uint8_t _storage_maps[MAX_PAGES * sizeof(mmap)];

/*only 40 bytes (on 32 bit platform) per list is used
by first two allocate call after construction
Somehaw STD will vreate two allocators
First allcator's allocate neads 8 bytes by 'first' (first is  also last) call 
Second allocator's allolcate is called first time automatically (32 bytes)
and every time on insert with 43 bytes
All other calls are 32 bytes per insert
*/
static uint8_t _internal_map_storage[MAX_PAGES * INTERNAL_MAP_STORAGE_SIZE * 2U];

static uint8_t _last_map = 0U;

uint8_t* get_map_allocator_intern_storage()
{
	uint8_t* ptr_internal_storage = _internal_map_storage;
	static size_t count = 0U;
	uint8_t* ret = ptr_internal_storage + count * INTERNAL_MAP_STORAGE_SIZE;
	count++;
	return ret;
}

uint8_t multimap_create(rtsha_page* page)
{
	uint8_t ret = _last_map;
	using namespace std;
	if (_last_map >= MAX_PAGES)
	{
		return MAX_PAGES;
	}

	uint8_t* ptr_stack_memory_alloc_storage = _storage_allocators + _last_map * sizeof(mmap_allocator);
	
	_allocators[_last_map]  = new (ptr_stack_memory_alloc_storage) InternMapAllocator<std::pair<const uint64_t, size_t>>(page);
	_maps[_last_map] = new mmap(*_allocators[_last_map]);

	_last_map++;
	return ret;
}

bool multimap_insert(uint16_t handle, const uint64_t key,  size_t block)
{
	if ((_last_map > 0U) && (handle < _last_map) && (_last_map > 0U) )
	{
		_maps[handle]->insert(std::pair<const uint64_t, size_t>(key, block));
		return true;
	}
	return false;
}


void multimap_delete(uint16_t handle, const uint64_t key, size_t block)
{
	if ((_last_map > 0U) && (handle < _last_map) && (_last_map > 0U))
	{
		mmap::iterator it = _maps[handle]->find(key);
		if (it != _maps[handle]->end())
		{
			while (it->first == key)
			{
				if ((it->first == key) && (it->second == block))
				{
					_maps[handle]->erase(it);
					break;
				}
				it++;
			}
		}
		
	}
}

size_t multimap_find(uint16_t handle, const uint64_t key)
{
	if ((_last_map > 0U) && (handle < _last_map))
	{
		mmap::iterator it = _maps[handle]->lower_bound(key);
		if (it != _maps[handle]->end())
		{
			return it->second;
		}
	}
	return 0U;
}


void multimap_drop_all(uint16_t handle)
{
	if (handle < _last_map && _maps[handle]->size() > 0)
	{
		_maps[handle]->clear();
	}
}

void multimap_destroy(uint16_t handle)
{
	if (handle < _last_map)
	{
		delete _maps[handle];
		delete _allocators[handle];
	}
}