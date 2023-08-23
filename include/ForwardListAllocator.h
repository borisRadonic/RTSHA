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
#include "MemoryPage.h"
#include <cstdlib>

#ifdef _RTSHA_DIAGNOSTIK
#include <iostream>
#endif

namespace rtsha
{
    /**
    * @class ForwardListAllocator
    * @brief Custom allocator tailored STL forward_list
    *
    * This allocator is designed to efficiently manage memory for lists, leveraging
    * specific characteristics of the `rtsha_page` structure and a reserved small storage.
    *
    * @tparam T The data type the allocator is responsible for.
    */
    template<class T>
    struct ForwardListAllocator
    {
        /**
        * @brief Defines the type of elements managed by the allocator.
        */
        typedef T value_type;

        /**
        * @brief Constructs the allocator with a given memory page
        *
        * @param page Pointer to the rtsha_page the allocator should use to allocate the memory for 'forward_list'.
        */
        explicit ForwardListAllocator(MemoryPage& page ) noexcept
            : _page(page)                        
        {
        }

        /**
        * @brief Copy constructor allowing for type conversion.
        *
        * called from 'std::forward_list'
        * 
        * @tparam U The data type of the other allocator.
        * @param rhs The other allocator to be copied from.
        */
        template<class U>
        constexpr ForwardListAllocator(const ForwardListAllocator <U>& rhs) noexcept
        {
            this->_page             = rhs._page;
        }

        /**
        * @brief Allocates a memory for an array of `n` objects of type `T`.
        * It is called from 'forward_list' every time when 'push' method is called
        * Memory of the free block specified in lastFreeBlockAddress is used as a storage.
        *
        * @param n Number of objects to allocate memory for.
        * @return Pointer to the allocated block of memory.
        */
        [[nodiscard]] rtsha_attr_inline T* allocate(std::size_t n)  noexcept
        {            
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            {
                return nullptr;
            }
            /*max. 1 block for std::forward list*/
            if (n != 1U)
            {
                return nullptr;
            }                        

            if (auto p = static_cast<T*>(_page.allocate_block(n * sizeof(T)) ))
            {
                return p;
            }
            return nullptr;
        }

        /**
        * @brief Deallocates memory. In this custom allocator, the deallocation is a no-op.
        *
        * It is called from 'forward_list' every time when 'pop' method is called
        * 
        */
        rtsha_attr_inline void deallocate(T* p, std::size_t n) noexcept
        {
            if (p != nullptr)
            {
                size_t address = (size_t)ptr;
                address -= sizeof(rtsha_block); /*skip size and pointer to prev*/
                MemoryBlock block(reinterpret_cast<rtsha_block*>(address));
                if (block.isValid() && !block.isFree())
                {
                    _page.free_block(block);
                }
            }
        }

        MemoryPage& _page;                              ///< The memory page managed by the allocator.
    };
       
    /**
     * @brief Compares two `ForwardListAllocator` objects for equality.
     *
     * @tparam T Type associated with the first allocator.
     * @tparam U Type associated with the second allocator.
     * @return True as, by design, all instances of this allocator are interchangeable.
     */
    template<class T, class U>
    bool operator==(const ForwardListAllocator <T>&, const ForwardListAllocator <U>&) noexcept { return true; }

    /**
    * @brief Compares two `ForwardListAllocator` objects for inequality.
    *
    * @tparam T Type associated with the first allocator.
    * @tparam U Type associated with the second allocator.
    * @return False as, by design, all instances of this allocator are interchangeable.
    */
    template<class T, class U>
    bool operator!=(const ForwardListAllocator <T>&, const ForwardListAllocator <U>&) noexcept { return false; }
}

