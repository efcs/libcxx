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

namespace coro = std::experimental::coroutines_v1;

struct minig {
  struct promise_type {
    int current_value;
    coro::suspend_always yield_value(int value) {
      this->current_value = value;
      return {};
    }
    coro::suspend_always initial_suspend() { return {}; }
    coro::suspend_always final_suspend() { return {}; }
    minig get_return_object() { return minig{this}; };
    void return_void() {}
    static void unhandled_exception();
  };

  bool move_next() {
    p.resume();
    return !p.done();
  }
  int current_value() { return p.promise().current_value; }

  minig(minig &&rhs) : p(rhs.p) { rhs.p = nullptr; }

  ~minig() {
    if (p)
      p.destroy();
  }

private:
  explicit minig(promise_type *p)
      : p(coro::coroutine_handle<promise_type>::from_promise(*p)) {}

  coro::coroutine_handle<promise_type> p;
};

minig fib(int n) {
  for (int i = 0; i < n; i++) {
    co_yield i;
  }
}

// CHECK-LABEL: define i32 @main()
int main() {
  int sum = 0;
  auto g = fib(5);
  while (g.move_next()) {
     sum += g.current_value();
  }
  // CHECK: ret i32 10
  assert(sum == 10);
}
