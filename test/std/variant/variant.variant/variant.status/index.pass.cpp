// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14

// <variant>

// template <class ...Types> class variant;

// constexpr size_t index() const noexcept;

#include <variant>
#include <string>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "variant_test_helpers.hpp"

int main()
{
    {
        using V = std::variant<int, void>;
        constexpr V v;
        static_assert(v.index() == 0, "");
    }
    {
        using V = std::variant<int, void, long>;
        constexpr V v(42l);
        static_assert(v.index() == 2, "");
    }
    {
        using V = std::variant<int, std::string>;
        V v("abc");
        assert(v.index() == 1);
        v = 42;
        assert(v.index() == 0);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        using V = std::variant<int, MakeEmptyT>;
        V v;
        assert(v.index() == 0);
        makeEmpty(v);
        assert(v.index() == std::variant_npos);
    }
#endif
}
