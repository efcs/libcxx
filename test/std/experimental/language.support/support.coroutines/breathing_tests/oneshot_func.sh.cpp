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

// This file tests, one shot, movable std::function like thing using coroutine
// for compile-time type erasure and unerasure. At the moment supports only
// functions with no arguments.
template <typename R> struct func {
  struct promise_type {
    R result;
    func get_return_object() { return {this}; }
    coro::suspend_always initial_suspend() { return {}; }
    coro::suspend_always final_suspend() { return {}; }
    void return_value(R v) { result = v; }

    static void unhandled_exception() {}
  };

  R operator()() {
    h.resume();
    R result = h.promise().result;
    h.destroy();
    h = nullptr;
    return result;
  };

  func() {}
  func(func &&rhs) : h(rhs.h) { rhs.h = nullptr; }
  func(func const &) = delete;

  func &operator=(func &&rhs) {
    if (this != &rhs) {
      if (h)
        h.destroy();
      h = rhs.h;
      rhs.h = nullptr;
    }
    return *this;
  }

  template <typename F> static func Create(F f) { co_return f(); }

  template <typename F> func(F f) : func(Create(f)) {}

  ~func() {
    if (h)
      h.destroy();
  }

private:
  func(promise_type *promise)
      : h(coro::coroutine_handle<promise_type>::from_promise(*promise)) {}
  coro::coroutine_handle<promise_type> h;
};

extern "C" int yield(int) { return 42; }
extern "C" float fyield(int) { return 0; }

void Do1(func<int> f) { yield(f()); }
void Do2(func<double> f) { yield(f()); }

// CHECK-LABEL: @main(
int main() {
// CHECK: call i32 @yield(i32 43)
// CHECK: call i32 @yield(i32
  Do1([] { return yield(43); });

// CHECK: call float @fyield(i32 44)
// CHECK: call i32 @yield(i32
  Do2([] { return fyield(44); });
}
