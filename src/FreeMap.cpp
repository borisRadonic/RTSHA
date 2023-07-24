#include "FreeMap.h"

namespace internal
{	
	FreeMap::FreeMap(rtsha_page* page)
		:_page(page)
	{
		/*create objects on stack using new in place*/
		_fnc = new (_storage_fnc) fmap_get_local_mem;
		_mallocator = new (_storage_allocator) mmap_allocator(page, _fnc);
		_ptrMap = new (_storage_map) mmap(*_mallocator);
	}

	void FreeMap::insert(const uint64_t key, size_t block)
	{
		if ((_ptrMap != nullptr))
		{
			_ptrMap->insert(std::pair<const uint64_t, size_t>(key, block));
		}
	}

	void FreeMap::del(const uint64_t key, size_t block)
	{
		if ((_ptrMap != nullptr))
		{
			mmap::iterator it = _ptrMap->find(key);
			if (it != _ptrMap->end())
			{
				while (it->first == key)
				{
					if ((it->first == key) && (it->second == block))
					{
						_ptrMap->erase(it);
						break;
					}
					it++;
				}
			}

		}
	}

	size_t FreeMap::find(const uint64_t key)
	{
		if ((_ptrMap != nullptr))
		{
			mmap::iterator it = _ptrMap->lower_bound(key);
			if (it != _ptrMap->end())
			{
				return it->second;
			}
		}
		return 0U;
	}
}
