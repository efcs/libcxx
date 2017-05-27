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

namespace coro = std::experimental::coroutines_v1;

struct coro_t {
  struct promise_type {
    coro_t get_return_object() {
      coro::coroutine_handle<promise_type>{};
      return {};
    }
    coro::suspend_never initial_suspend() { return {}; }
    coro::suspend_never final_suspend() { return {}; }
    void return_void(){}
    static void unhandled_exception() {}
  };
};

struct B {
  ~B() {}
  bool await_ready() { return true; }
  B await_resume() { return {}; }
  template <typename F> void await_suspend(F) {}
};

extern "C" void print(int) {}

struct A {
  ~A(){}
  bool await_ready() { return true; }
  int await_resume() { return 42; }
  template <typename F> void await_suspend(F) {}
};

extern "C" coro_t f(int n) {
  if (n == 0) {
    print(0);
    co_return;
  }
  int val = co_await A{};
  print(42);
}

extern "C" coro_t g() { B val = co_await B{}; }


int main() {
  f(0);
  f(1);

}
