#pragma onc
#include "structures.h"
#include <cstdlib>
#include <new>
#include <limits>
#include <iostream>
#include <vector>

namespace internal
{

    template<class T>
    struct InternMapAllocator
    {
        typedef T value_type;

        InternMapAllocator(rtsha_page* page):_page(page)
        {

        }

        template<class U>
        constexpr InternMapAllocator(const InternMapAllocator <U>&) noexcept {}

        [[nodiscard]] T* allocate(std::size_t n)
        {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
                throw std::bad_array_new_length();

            if (auto p = static_cast<T*>(std::malloc(n * sizeof(T))))
            {
                report(p, n);
                return p;
            }

            throw std::bad_alloc();
        }

        void deallocate(T* p, std::size_t n) noexcept
        {
            report(p, n, 0);
            std::free(p);
        }
    private:
        void report(T* p, std::size_t n, bool alloc = true) const
        {
            std::cout << (alloc ? "Subpage Alloc: " : "Subpage Dealloc: ") << sizeof(T) * n
                << " bytes at " << std::hex << std::showbase
                << reinterpret_cast<void*>(p) << std::dec << '\n';
        }

        rtsha_page* _page;
    };

   
    template<class T, class U>
    bool operator==(const InternMapAllocator <T>&, const InternMapAllocator <U>&) { return true; }

    template<class T, class U>
    bool operator!=(const InternMapAllocator <T>&, const InternMapAllocator <U>&) { return false; }

}

