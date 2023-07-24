#pragma once
#include "MemoryPage.h"
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

    class fmap_get_local_mem
    {
    public:
        fmap_get_local_mem() = default;

        void* operator()()
        {
            uint8_t* ptr_internal_storage = _internal_map_storage;
            static size_t count = 0U;
            uint8_t* ret = ptr_internal_storage + count * INTERNAL_MAP_STORAGE_SIZE;
            count++;
            return (void*) ret;
        };
        /*only 40 bytes (on 32 bit platform) per list is used
        by first two allocate calls after construction
        Somehow multimap will create two allocators
        First allcator's allocate neads 8 bytes by 'first' (first is also last) call
        Second allocator's allolcate is called first time automatically (32 bytes)
        and every time on insert with additional 32 bytes
        */
        uint8_t _internal_map_storage[INTERNAL_MAP_STORAGE_SIZE * 2U * MAX_BIG_PAGES] ;
    };


    template<class T>
    struct InternMapAllocator
    {
        typedef T value_type;

        InternMapAllocator(rtsha_page* page, internal::fmap_get_local_mem* fnc)
            :_page(page),
            _allocated_intern(0U),
            _fnc(fnc)
        {
        }

        template<class U>
        constexpr InternMapAllocator(const InternMapAllocator <U>& rhs) noexcept
        {
            this->_page              = rhs._page;
            this->_allocated_intern  = rhs._allocated_intern;
            this->_fnc               = rhs._fnc;
        }

        [[nodiscard]] T* allocate(std::size_t n)
        {
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
                    void* t = (*_fnc)();
                    auto p = static_cast<T*>( t );
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
        fmap_get_local_mem* _fnc;

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

