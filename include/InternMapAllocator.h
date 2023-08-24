/******************************************************************************
The MIT License(MIT)

Real Time Safety Heap Allocator (RTSHA)
https://github.com/borisRadonic/RTSHA

Copyright(c) 2023 Boris Radonic

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#pragma once
#include "SmallFixMemoryPage.h"
#include <cstdlib>
#ifdef _RTSHA_DIAGNOSTIK
#include <iostream>
#endif

namespace internal
{   
    #if defined _WIN64 || defined _ARM64
        #define INTERNAL_MAP_STORAGE_SIZE	64U
    #else
        #define INTERNAL_MAP_STORAGE_SIZE	32U
    #endif
      
    using namespace rtsha;

    /**
    * @brief Allocator designed to handle internal memory allocations used with FreeMap classthat is used to manage key-value pairs in memory.
    *
    * This custom allocator is optimized for managing memory in a specific
    * context where memory is provided by an instance of `rtsha_page`.
    *
    * @tparam T The type of elements being allocated.
    */
    template<class T>
    struct InternMapAllocator
    {
        /**
        * @brief Defines the type of elements managed by the allocator.
        */
        typedef T value_type;

        /**
        * @brief Construct a new Intern Map Allocator object.
        *
        * @param page The memory page context in which the allocator will operate.
        */
        InternMapAllocator(rtsha_page* page)
            :_page(page)
        {
        }

        /**
        * @brief Copy constructor.
        *
        * This constructor allows for the creation of an allocator of one type
        * from another type, provided they have the same base template.
        *
        * * called from 'std::multimap'
        * 
        * @tparam U The source type for the allocator.
        * @param rhs The source allocator.
        */
        template<class U>
        constexpr InternMapAllocator(const InternMapAllocator <U>& rhs) noexcept
        {
            this->_page              = rhs._page;
        }

        /**
        * @brief Allocate memory.
        *
        * Attempt to allocate memory for `n` items of type `T`.
        * 
        * Memory in predefined  'map_page' which is page of memory page will be used as storage.
        * Allocator uses SmallFixMemoryPage together with 64 bytes blocks.
        * 
        * * It is called from 'std::multimap' every time when 'insert' method is called.
        * 
        * @param n Number of items of type `T` to allocate memory for.
        * @return T* Pointer to the allocated memory, or nullptr if allocation failed.
        */
        [[nodiscard]] rtsha_attr_inline T* allocate(std::size_t n)   noexcept
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
            return nullptr;
        }

        /**
        * @brief Deallocate memory.
        *
        * Release previously allocated memory back to the 'map_page' pool.
        *
        * @param p Pointer to the memory to be deallocated.
        */
        rtsha_attr_inline void deallocate(T*p, std::size_t /*n*/) noexcept
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

        rtsha_page* _page;              ///< Memory page context.
       
    private:
#ifdef _RTSHA_DIAGNOSTIK
        /**
        * @brief Report memory allocation or deallocation details.
        *
        * Utility function to output information about memory operations.
        *
        * @param p Pointer to the memory block being reported.
        * @param n Number of items of type `T` in the memory block.
        * @param alloc Flag indicating whether the operation is an allocation (true) or deallocation (false).
        */
        void report(T* p, std::size_t n, bool alloc = true) noexcept const
        {
            std::cout << (alloc ? "MAlloc: " : "MDealloc: ") << sizeof(T) * n
                << " bytes at " << std::hex << std::showbase
                << reinterpret_cast<void*>(p) << std::dec << '\n';
        }
#endif
    };

    /**
    * @brief Equality operator for InternMapAllocator.
    *
    * @tparam T Type parameter for the left-hand side allocator.
    * @tparam U Type parameter for the right-hand side allocator.
    * @return true Always returns true, indicating allocators are stateless.
    */
    template<class T, class U>
    bool operator==(const InternMapAllocator <T>&, const InternMapAllocator <U>&) noexcept { return true; }

    /**
    * @brief Inequality operator for InternMapAllocator.
    *
    * @tparam T Type parameter for the left-hand side allocator.
    * @tparam U Type parameter for the right-hand side allocator.
    * @return false Always returns false, indicating allocators are stateless.
    */
    template<class T, class U>
    bool operator!=(const InternMapAllocator <T>&, const InternMapAllocator <U>&) noexcept { return false; }

}
