#pragma once
#include "SmallFixMemoryPage.h"
#include <cstdlib>
#include <new>
#include <iostream>

namespace internal
{   
    #if defined _WIN64 || defined _ARM64
        #define INTERNAL_MAP_STORAGE_SIZE	64U
    #else
        #define INTERNAL_MAP_STORAGE_SIZE	32U
    #endif
      

    template<class T>
    struct InternMapAllocator
    {
        typedef T value_type;

        InternMapAllocator(rtsha_page* page)
            :_page(page),
            _allocated_intern(0U)
        {
        }

        template<class U>
        constexpr InternMapAllocator(const InternMapAllocator <U>& rhs) noexcept
        {
            this->_page              = rhs._page;
            this->_allocated_intern  = rhs._allocated_intern;
        }

        [[nodiscard]] T* allocate(std::size_t n)   noexcept
        {
            if (_page->map_page != nullptr)
            {
                SmallFixMemoryPage map_page(_page->map_page);
                if ((size_t)_page->map_page->flags >= (n * sizeof(T)))
                {
                    auto p = reinterpret_cast<T*>(map_page.allocate_block((size_t)_page->map_page->flags));
                    if (p != nullptr)
                    {
                        //report(p, n, 1);
                        return p;
                    }
                }
            }
            return NULL;
        }

        void deallocate(T*p, std::size_t n) noexcept
        {
            if (_page->map_page != nullptr)
            {
                SmallFixMemoryPage map_page(_page->map_page);

                size_t address = reinterpret_cast<size_t>(p);
                address -= (2U * sizeof(size_t)); /*skip size and pointer to prev*/

                MemoryBlock block((rtsha_block*)(void*)address);

                if (block.isValid())
                {
                    map_page.free_block(block);
                    //report(p, n, 0);
                }
            }
        }

        rtsha_page* _page;
        size_t _allocated_intern = 0U;

    private:
        void report(T* p, std::size_t n, bool alloc = true) const
        {
            std::cout << (alloc ? "MAlloc: " : "MDealloc: ") << sizeof(T) * n
                << " bytes at " << std::hex << std::showbase
                << reinterpret_cast<void*>(p) << std::dec << '\n';
        }
    };

   
    template<class T, class U>
    bool operator==(const InternMapAllocator <T>&, const InternMapAllocator <U>&) { return true; }

    template<class T, class U>
    bool operator!=(const InternMapAllocator <T>&, const InternMapAllocator <U>&) { return false; }

}

