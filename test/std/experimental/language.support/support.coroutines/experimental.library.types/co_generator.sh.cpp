// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11
// REQUIRES: fcoroutines-ts

// RUN: %build -fcoroutines-ts
// RUN: %run

// <experimental/coroutine>

#include <experimental/coroutine>
#include <vector>
#include <cassert>

#include "test_macros.h"
#include "coroutine_library_types.h"
#include "coroutine_concept_archetypes.h"

struct CustomPromise : ValuePromise<int> {
  using ValuePromise<int>::ValuePromise;
  template <class Arg> coro::suspend_always yield_value(Arg&& arg) {
    this->get() = std::forward<Arg>(arg);
    return {};
  }
};

co_generator<int> fib(int n) {
  for (int i = 0; i < n; ++i)
    co_yield i;
}

co_generator<int, CustomPromise> fib2(int n) {
  for (int i = 0; i < n; ++i)
    co_yield i;
}

template <class GenT>
void test_gen(GenT g) {
  std::vector<int> vec;
  const std::vector<int> expect = {
      0, 1, 2, 3, 4
  };
  for (auto v : g)
    vec.push_back(v);
  assert(vec == expect);
}

int main() {
  test_gen(fib(5));
  test_gen(fib2(5));
}
