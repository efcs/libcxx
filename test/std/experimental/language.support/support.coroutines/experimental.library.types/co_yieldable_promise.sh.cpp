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

// A sample type that uses co_iterator to build a generator.
// The promise_type
struct MyGenerator {
  struct promise_type : co_yieldable_promise<int> {
    using co_yieldable_promise<int>::co_yieldable_promise;
    MyGenerator get_return_object() { return MyGenerator{this}; }
  };

public:
  using iterator = co_iterator<promise_type>;
  iterator begin() {
    p.resume();
    return {p};
  }

  iterator end() { return {}; }
private:
  using handle_type = coro::coroutine_handle<promise_type>;
  explicit MyGenerator(promise_type *p) : p(handle_type::from_promise(*p)) {}
  handle_type p;
};

MyGenerator fib(int n) {
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
