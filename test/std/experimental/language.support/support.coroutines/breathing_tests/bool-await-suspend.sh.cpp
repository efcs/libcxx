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

struct NoSuspend {
  bool await_ready() { return false; }
  void await_resume() {}
  template <typename F> bool await_suspend(F) { return false; }
};

extern "C" void print(int) {}

struct DoSuspend {
  bool await_ready() { return false; }
  void await_resume() {}
  template <typename F> bool await_suspend(F) { return true; }
};

extern "C" coro_t f() {
  print(1);
  co_await DoSuspend{};
  print(2);
}

extern "C" coro_t g() {
  print(3);
  co_await NoSuspend{};
  print(4);
}

// CHECK-LABEL: @main(
int main() {
  f();
  g();
// CHECK:    call void @print(i32 1)
// CHECK:    call void @print(i32 3)
// CHECK:    call void @print(i32 4)
// CHECK:    ret i32 0
}
