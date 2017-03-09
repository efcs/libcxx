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
    using Alloc = ex::polymorphic_allocator<int>;
    using Traits = std::allocator_traits<Alloc>;
    NullResource R;
    AllocController &C = R.getController();
    Alloc a(&R);
    const std::size_t maxSize = Traits::max_size(a);

    // Fake the allocation so that the de-allocation doesn't underflow
    // within the allocator controllers bookkeeping.
    C.countAlloc(nullptr, maxSize * sizeof(int), alignof(int));

    a.deallocate(nullptr, maxSize);
    try {
      a.deallocate(nullptr, maxSize + 1u);
      assert(false);
    } catch (...) {}
}
