#pragma once
#include "MemoryPage.h"
#include <cstdlib>
#include <new>
#include <iostream>

namespace internal
{
    using namespace rtsha;

    /**
    * @class InternListAllocator
    * @brief Custom allocator tailored for internal list management.
    *
    * This allocator is designed to efficiently manage memory for lists, leveraging
    * specific characteristics of the `rtsha_page` structure and a reserved small storage.
    *
    * @tparam T The data type the allocator is responsible for.
    */
    template<class T>
    struct InternListAllocator
    {
        /**
        * @brief Defines the type of elements managed by the allocator.
        */
        typedef T value_type;

        /**
        * @brief Constructs the allocator with a given page and a pointer to a small storage.
        *
        * @param page Pointer to the rtsha_page the allocator should use to allocate the memory for 'free list'.
        * @param _ptrSmallStorage Pointer to a small reserved storage.
        */
        explicit InternListAllocator(rtsha_page* page, size_t* _ptrSmallStorage)
            : _page(page),
            
            _allocated_intern(0U),
            _ptrInternalSmallStorage(_ptrSmallStorage)
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
        constexpr InternListAllocator(const InternListAllocator <U>& rhs) noexcept
        {
            this->_page             = rhs._page;
            this->_allocated_intern = rhs._allocated_intern;
            this->_ptrInternalSmallStorage = rhs._ptrInternalSmallStorage;
        }

        /**
        * @brief Allocates a memory for an array of `n` objects of type `T`.
        * It is called from 'forward_list' every time when 'push' method is called
        * Memory of the free block specified in lastFreeBlockAddress is used as a storage.
        *
        * @param n Number of objects to allocate memory for.
        * @return Pointer to the allocated block of memory.
        */
        [[nodiscard]] T* allocate(std::size_t n)  noexcept
        {
            /*max. 1 block*/
            if (n != 1U)
            {
                return nullptr;
            }
            if ((_allocated_intern == 0U) && (_page->lastFreeBlockAddress == 0U))
            {
                if (n > 1U)
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

        /**
        * @brief Deallocates memory. In this custom allocator, the deallocation is a no-op.
        *
        * It is called from 'forward_list' every time when 'pop' method is called
        * 
        */
        void deallocate(T* /*p*/, std::size_t /*n*/) noexcept
        {
        }

        rtsha_page* _page;                              ///< The memory page managed by the allocator.
        size_t      _allocated_intern           = 0U;   ///< Counter to track a first few internal allocations.
        size_t*     _ptrInternalSmallStorage    = NULL; ///< Pointer to the small reserved storage.

    private:

        /**
         * @brief Reports allocation or deallocation events. Useful for debugging.
         *
         * @param p Pointer to the memory block in question.
         * @param n Number of objects associated with the operation.
         * @param alloc Flag to determine if it's allocation (true) or deallocation (false).
         */
        void report(T* p, std::size_t n, bool alloc = true) const
        {
            std::cout << (alloc ? "LAlloc: " : "LDealloc: ") << sizeof(T) * n
                << " bytes at " << std::hex << std::showbase
                << reinterpret_cast<void*>(p) << std::dec << '\n';
        }
    };
       
    /**
     * @brief Compares two `InternListAllocator` objects for equality.
     *
     * @tparam T Type associated with the first allocator.
     * @tparam U Type associated with the second allocator.
     * @return True as, by design, all instances of this allocator are interchangeable.
     */
    template<class T, class U>
    bool operator==(const InternListAllocator <T>&, const InternListAllocator <U>&) { return true; }

    /**
    * @brief Compares two `InternListAllocator` objects for inequality.
    *
    * @tparam T Type associated with the first allocator.
    * @tparam U Type associated with the second allocator.
    * @return False as, by design, all instances of this allocator are interchangeable.
    */
    template<class T, class U>
    bool operator!=(const InternListAllocator <T>&, const InternListAllocator <U>&) { return false; }
}

