// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TEST_SUPPORT_COROUTINE_CONCEPT_ARCHETYPES_H
#define TEST_SUPPORT_COROUTINE_CONCEPT_ARCHETYPES_H

/// Library Concepts Archetypes -- Specification and Examples
///   * ValuePromise
///   * YieldablePromise
///   *

#include <experimental/coroutine>

inline namespace Archetypes {

namespace coro = std::experimental::coroutines_v1;

struct BasicPromise {
  coro::suspend_always initial_suspend() { return {}; }
  coro::suspend_always final_suspend() { return {}; }
  void return_void() {}
  static void unhandled_exception() {}
};

/**
 * YieldablePromise concept
 *
 * Requirements:
 *   - 'p.yield_value(x)' must be valid for some value of 'x'
 *
 */
template <class Tp>
struct YieldablePromise {
  // Required by YieldablePromise
  coro::suspend_always yield_value(Tp) {
    return {};
  }

  // Required by TS
  coro::suspend_always initial_suspend() { return {}; }
  coro::suspend_always final_suspend() { return {}; }
  void return_void() {}
  static void unhandled_exception() {}
};


/**
 * ValuePromise
 * Requirements:
 *   typedef [...] value_type;
 *   value_type& get();
 *   value_type const& get() const;
 */
template <class Tp>
struct ValuePromise {
  // Required by 'ValuePromise'
  using value_type = Tp;
  value_type& get() { return object; }
  value_type const& get() const { return object; }

  // Required by TS
  coro::suspend_always initial_suspend() { return {}; }
  coro::suspend_always final_suspend() { return {}; }
  void return_void() {}
  static void unhandled_exception() {}

private:
  value_type object;
};

} // namespace Archetypes
#endif // TEST_SUPPORT_COROUTINE_CONCEPT_ARCHETYPES_H
