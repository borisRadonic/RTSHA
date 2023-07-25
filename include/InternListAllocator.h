#pragma once
#include "MemoryPage.h"
#include <cstdlib>
#include <new>
#include <iostream>

namespace internal
{
    template<class T>
    struct InternListAllocator
    {
        typedef T value_type;

        InternListAllocator(rtsha_page* page, size_t* _ptrSmallStorage)
            : _page(page),
            _allocated_intern(0U),
            _ptrInternalSmallStorage(_ptrSmallStorage)
        {  
        }

        template<class U>
        constexpr InternListAllocator(const InternListAllocator <U>& rhs) noexcept
        {
            this->_page             = rhs._page;
            this->_allocated_intern = rhs._allocated_intern;
            this->_ptrInternalSmallStorage = rhs._ptrInternalSmallStorage;
        }

        [[nodiscard]] T* allocate(std::size_t n)  noexcept
        {
            /*max. 1 block*/
            if (n != 1U)
            {
                return nullptr;
            }
            if ((_allocated_intern == 0U) && (_page->lastFreeBlockAddress == 0U))
            {
                if (n > 1)
                {
                    return nullptr;
                }
                _allocated_intern++;
                auto p = static_cast<T*>((void*)_ptrInternalSmallStorage);
                return p;
            }
            auto p = static_cast<T*>( (void*) _page->lastFreeBlockAddress);
            return p;
        }

        void deallocate(T* /*p*/, std::size_t /*n*/) noexcept
        {
        }

        rtsha_page* _page;
        size_t      _allocated_intern           = 0U;
        size_t*     _ptrInternalSmallStorage    = NULL;

    private:
        void report(T* p, std::size_t n, bool alloc = true) const
        {
            std::cout << (alloc ? "LAlloc: " : "LDealloc: ") << sizeof(T) * n
                << " bytes at " << std::hex << std::showbase
                << reinterpret_cast<void*>(p) << std::dec << '\n';
        }
    };
       
    template<class T, class U>
    bool operator==(const InternListAllocator <T>&, const InternListAllocator <U>&) { return true; }

    template<class T, class U>
    bool operator!=(const InternListAllocator <T>&, const InternListAllocator <U>&) { return false; }
}

