//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03
// UNSUPPORTED: libcpp-no-exceptions
// MODULES_DEFINES: _LIBCPP_DEBUG=0
// MODULES_DEFINES: _LIBCPP_DEBUG_USE_EXCEPTIONS

// <experimental/memory_resource>

// template <class T> class polymorphic_allocator

// T* polymorphic_allocator<T>::deallocate(T*, size_t size)

#define _LIBCPP_DEBUG 0
#define _LIBCPP_DEBUG_USE_EXCEPTIONS
#include <experimental/memory_resource>
#include <type_traits>
#include <cassert>

#include "test_memory_resource.hpp"

namespace ex = std::experimental::pmr;

int main()
{
    using Alloc = NullAllocator<char>;
    using R = ex::resource_adaptor<Alloc>;
    AllocController P;
    ex::resource_adaptor<Alloc> r(Alloc{P});
    ex::memory_resource & m1 = r;

    std::size_t maxSize = std::numeric_limits<std::size_t>::max()
                            - alignof(std::max_align_t);
    m1.allocate(maxSize);
    m1.deallocate(nullptr, maxSize);

  try {
    m1.deallocate(nullptr, maxSize + 1);
    assert(false);
  } catch (...) {}
}
