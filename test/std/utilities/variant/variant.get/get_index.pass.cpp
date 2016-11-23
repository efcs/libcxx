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

// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>>& get(variant<Types...>& v);
// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v);
// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>> const& get(const variant<Types...>& v);
// template <size_t I, class... Types>
//  constexpr variant_alternative_t<I, variant<Types...>> const&& get(const variant<Types...>&& v);

#include <variant>
#include <utility>
#include <type_traits>
#include <cassert>
#include "test_macros.h"
#include "variant_test_helpers.hpp"

void test_const_lvalue_get() {
    {
        using V = std::variant<int>;
        constexpr V v(42);
        //ASSERT_NOT_NOEXCEPT(std::get<0>(v));
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int const&);
        static_assert(std::get<0>(v) == 42, "");
    }
    {
        using V = std::variant<int, long>;
        constexpr V v(42l);
        ASSERT_SAME_TYPE(decltype(std::get<1>(v)), long const&);
        static_assert(std::get<1>(v) == 42, "");
    }
// FIXME: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
    {
        using V = std::variant<int&>;
        int x = 42;
        const V v(x);
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int&);
        assert(&std::get<0>(v) == &x);
    }
    {
        using V = std::variant<int&&>;
        int x = 42;
        const V v(std::move(x));
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int&);
        assert(&std::get<0>(v) == &x);
    }
    {
        using V = std::variant<const int&&>;
        int x = 42;
        const V v(std::move(x));
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), const int&);
        assert(&std::get<0>(v) == &x);
    }
#endif
}

void test_lvalue_get()
{
    {
        using V = std::variant<int>;
        V v(42);
        ASSERT_NOT_NOEXCEPT(std::get<0>(v));
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int&);
        assert(std::get<0>(v) == 42);
    }
    {
        using V = std::variant<int, long>;
        V v(42l);
        ASSERT_SAME_TYPE(decltype(std::get<1>(v)), long&);
        assert(std::get<1>(v) == 42);
    }
    // FIXME: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
    {
        using V = std::variant<int&>;
        int x = 42;
        V v(x);
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int&);
        assert(&std::get<0>(v) == &x);
    }
    {
        using V = std::variant<const int&>;
        int x = 42;
        V v(x);
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), const int&);
        assert(&std::get<0>(v) == &x);
    }
    {
        using V = std::variant<int&&>;
        int x = 42;
        V v(std::move(x));
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), int&);
        assert(&std::get<0>(v) == &x);
    }
    {
        using V = std::variant<const int&&>;
        int x = 42;
        V v(std::move(x));
        ASSERT_SAME_TYPE(decltype(std::get<0>(v)), const int&);
        assert(&std::get<0>(v) == &x);
    }
#endif
}


void test_rvalue_get()
{
    {
        using V = std::variant<int>;
        V v(42);
        ASSERT_NOT_NOEXCEPT(std::get<0>(std::move(v)));
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int&&);
        assert(std::get<0>(std::move(v)) == 42);
    }
    {
        using V = std::variant<int, long>;
        V v(42l);
        ASSERT_SAME_TYPE(decltype(std::get<1>(std::move(v))), long&&);
        assert(std::get<1>(std::move(v)) == 42);
    }
    // FIXME: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
    {
        using V = std::variant<int&>;
        int x = 42;
        V v(x);
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int&);
        assert(&std::get<0>(std::move(v)) == &x);
    }
    {
        using V = std::variant<const int&>;
        int x = 42;
        V v(x);
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int&);
        assert(&std::get<0>(std::move(v)) == &x);
    }
    {
        using V = std::variant<int&&>;
        int x = 42;
        V v(std::move(x));
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int&&);
        int&& xref = std::get<0>(std::move(v));
        assert(&xref == &x);
    }
    {
        using V = std::variant<const int&&>;
        int x = 42;
        V v(std::move(x));
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int&&);
        const int&& xref = std::get<0>(std::move(v));
        assert(&xref == &x);
    }
#endif
}


void test_const_rvalue_get()
{
    {
        using V = std::variant<int>;
        const V v(42);
        ASSERT_NOT_NOEXCEPT(std::get<0>(std::move(v)));
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int&&);
        assert(std::get<0>(std::move(v)) == 42);
    }
    {
        using V = std::variant<int, long>;
        const V v(42l);
        ASSERT_SAME_TYPE(decltype(std::get<1>(std::move(v))), const long&&);
        assert(std::get<1>(std::move(v)) == 42);
    }
    // FIXME: Remove these once reference support is reinstated
#if !defined(TEST_VARIANT_HAS_NO_REFERENCES)
    {
        using V = std::variant<int&>;
        int x = 42;
        const V v(x);
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int&);
        assert(&std::get<0>(std::move(v)) == &x);
    }
    {
        using V = std::variant<const int&>;
        int x = 42;
        const V v(x);
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int&);
        assert(&std::get<0>(std::move(v)) == &x);
    }
    {
        using V = std::variant<int&&>;
        int x = 42;
        const V v(std::move(x));
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), int&&);
        int&& xref = std::get<0>(std::move(v));
        assert(&xref == &x);
    }
    {
        using V = std::variant<const int&&>;
        int x = 42;
        const V v(std::move(x));
        ASSERT_SAME_TYPE(decltype(std::get<0>(std::move(v))), const int&&);
        const int&& xref = std::get<0>(std::move(v));
        assert(&xref == &x);
    }
#endif
}

template <std::size_t I>
using Idx = std::integral_constant<size_t, I>;

void test_throws_for_all_value_categories()
{
#ifndef TEST_HAS_NO_EXCEPTIONS
    using V = std::variant<int, long>;
    V v0(42);
    const V& cv0 = v0;
    assert(v0.index() == 0);
    V v1(42l);
    const V& cv1 = v1;
    assert(v1.index() == 1);
    std::integral_constant<size_t, 0> zero;
    std::integral_constant<size_t, 1> one;
    auto test = [](auto idx, auto&& v) {
      using Idx = decltype(idx);
      try {
          std::get<Idx::value>(std::forward<decltype(v)>(v));
      } catch (std::bad_variant_access const&) {
          return true;
      } catch (...) { /* ... */ }
      return false;
    };
    { // lvalue test cases
        assert(test(one, v0));
        assert(test(zero, v1));
    }
    { // const lvalue test cases
        assert(test(one, cv0));
        assert(test(zero, cv1));
    }
    { // rvalue test cases
        assert(test(one, std::move(v0)));
        assert(test(zero, std::move(v1)));
    }
    { // const rvalue test cases
        assert(test(one, std::move(cv0)));
        assert(test(zero, std::move(cv1)));
    }
#endif
}

int main()
{
    test_const_lvalue_get();
    test_lvalue_get();
    test_rvalue_get();
    test_const_rvalue_get();
    test_throws_for_all_value_categories();
}
