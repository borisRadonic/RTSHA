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
#include <cstdlib>
#include <new>
#include <limits>
#include <iostream>
#include "allocator.h"

namespace rtsha
{

    /**
    * @brief Custom allocator leveraging the `rtsha_malloc` function for memory management.
    *
    * This allocator provides a mechanism to utilize a specific memory management
    * method (`rtsha_malloc` and `rtsha_free`) for allocation and deallocation.
    * 
    * It can be used as custom allocator for std containers.
    * 
    * Singleton RTSH Heap instance must be created.
    *
    * @tparam T The type of elements being allocated.
    */
    template<class T>
    struct RTSHMallocator
    {
        typedef T value_type;   ///< Type of the elements being managed by the allocator.

        /**
        * @brief Default constructor.
        */
        RTSHMallocator() = default;

        
        /**
        * @brief Copy constructor.
        *
        * This constructor allows for the creation of an allocator of one type
        * from another type, provided they have the same base template.
        *
        * @tparam U The source type for the allocator.
        */
        template<class U>
        constexpr RTSHMallocator(const RTSHMallocator <U>&) noexcept {}

        /**
        * @brief Allocate memory.
        *
        * Attempt to allocate memory for `n` items of type `T`.
        *
        * @param n Number of items of type `T` to allocate memory for.
        * @return T* Pointer to the allocated memory.
        * @throws std::bad_alloc If memory allocation fails.
        * @throws std::bad_array_new_length If the allocation size exceeds system limits.
        */
        [[nodiscard]] T* allocate(std::size_t n)  noexcept
        {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            {
                return nullptr;
            }

            if (auto p = static_cast<T*>(rtsha_malloc(n * sizeof(T))))
            {
                //report(p, n);
                return p;
            }
            return nullptr;
        }

        /**
         * @brief Deallocate memory.
         *
         * Release previously allocated memory.
         *
         * @param p Pointer to the memory to be deallocated.
         * @param n Number of items originally requested during allocation.
         */
        void deallocate(T* p, std::size_t n) noexcept
        {
            //report(p, n, 0);
            rtsha_free(p);
        }
    private:

        /**
        * @brief Report memory allocation or deallocation details.
        *
        * Utility function to output information about memory operations.
        *
        * @param p Pointer to the memory block being reported.
        * @param n Number of items of type `T` in the memory block.
        * @param alloc Flag indicating whether the operation is an allocation (true) or deallocation (false).
        */
        void report(T* p, std::size_t n, bool alloc = true) const
        {
            std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T) * n
                << " bytes at " << std::hex << std::showbase
                << reinterpret_cast<void*>(p) << std::dec << '\n';
        }
    };

    /**
    * @brief Equality operator for RTSHMallocator.
    *
    * @tparam T Type parameter for the left-hand side allocator.
    * @tparam U Type parameter for the right-hand side allocator.
    * @return true Always returns true, indicating allocators are stateless.
    */
    template<class T, class U>
    bool operator==(const RTSHMallocator <T>&, const RTSHMallocator <U>&) { return true; }

    /**
    * @brief Inequality operator for RTSHMallocator.
    *
    * @tparam T Type parameter for the left-hand side allocator.
    * @tparam U Type parameter for the right-hand side allocator.
    * @return false Always returns false, indicating allocators are stateless.
    */
    template<class T, class U>
    bool operator!=(const RTSHMallocator <T>&, const RTSHMallocator <U>&) { return false; }
}