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

	void FreeMap::insert(const uint64_t key, size_t block)
	{
		if ((_ptrMap != nullptr))
		{			
			_ptrMap->insert(std::pair<const uint64_t, size_t>(key, block));
		}
	}

	bool FreeMap::del(const uint64_t key, size_t block)
	{
		if ((_ptrMap != nullptr))
		{
			mmap::iterator it = _ptrMap->find(key);
			while(it != _ptrMap->end())
			{
				if ((it->first == key) && (it->second == block))
				{
					it = _ptrMap->erase(it);						
					return true;
				}
				it++;
			}
		}
		return false;
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

	bool FreeMap::exists(const uint64_t key, size_t block)
	{
		if ((_ptrMap != nullptr))
		{
			mmap::iterator it = _ptrMap->find(key);
			if (it != _ptrMap->end())
			{
				if ((it->first == key) && (it->second == block))
				{					
					return true;
				}
			}
		}
		return false;
	}

	size_t FreeMap::size() const
	{
		if ((_ptrMap != nullptr))
		{
			return _ptrMap->size();
		}
		return 0U;
	}
}
