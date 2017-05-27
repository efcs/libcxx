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
#include "coroutine_generator.h"

generator<int> fib(int n) {
  for (int i = 0; i < n; ++i)
    co_yield i;
}

int main() {
  std::vector<int> vec;
  const std::vector<int> expect = {
      0, 1, 2, 3, 4
  };
  for (auto v : fib(5))
    vec.push_back(v);
  assert(vec == expect);
}
