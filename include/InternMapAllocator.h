#pragma once
#include "structures.h"
#include <cstdlib>
#include <new>
#include <limits>
#include <iostream>
#include <vector>

uint8_t* get_map_allocator_intern_storage();

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

        [[nodiscard]] T* allocate(std::size_t n)
        {
            size_t size = n * sizeof(T);
            /*max. 1 block per call*/
            if (n != 1U)
            {
                throw std::bad_array_new_length();
            }
            if(_page->lastFreeBlockAddress == 0U )
            {
                if (_allocated_intern < sizeof(T))
                {
                    if (n > 1)
                    {
                        throw std::bad_array_new_length();
                    }                    
                    auto p = static_cast<T*>((void*)(get_map_allocator_intern_storage()));
                    _allocated_intern += sizeof(T);
                    return p;
                }
            }
            /*the number of bytes are never greater than we have on free_block_address - n must be 1*/
            auto p = static_cast<T*>((void*)_page->lastFreeBlockAddress);

            _page->lastFreeBlockAddress = 0U;
            if (p)
            {
                //report(p, n);
                return p;
            }

            throw std::bad_alloc();
        }

        void deallocate(T* p, std::size_t n) noexcept
        {
            //report(p, n, 0);
        }

        rtsha_page* _page;
        size_t _allocated_intern = 0U;

    private:
        void report(T* p, std::size_t n, bool alloc = true) const
        {
            std::cout << (alloc ? "MAlloc: " : "SMDealloc: ") << sizeof(T) * n
                << " bytes at " << std::hex << std::showbase
                << reinterpret_cast<void*>(p) << std::dec << '\n';
        }

        
    };

   
    template<class T, class U>
    bool operator==(const InternMapAllocator <T>&, const InternMapAllocator <U>&) { return true; }

    template<class T, class U>
    bool operator!=(const InternMapAllocator <T>&, const InternMapAllocator <U>&) { return false; }

}

