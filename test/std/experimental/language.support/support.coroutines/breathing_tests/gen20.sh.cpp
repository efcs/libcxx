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
#include <cassert>

#include "coroutine_library_types.h"

co_generator<int> range(int from, int n) {
  for (int i = from; i < n; ++i)
    co_yield i;
}

int main() {
   int sum = 0;
   for (auto v: range(1, 20))
      sum += v;
  assert(sum == 190);
}
