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


struct MyCoro {
  struct promise_type : BasicPromise {
    MyCoro get_return_object() { return coro::coroutine_handle<promise_type>::from_promise(*this); }
  };
  using handle = coro::coroutine_handle<promise_type>;
  MyCoro(handle hh) : h(hh) {}
  handle h;
};


struct get_coro_handle_tag {
  bool await_ready() const { return false; }
  int await_suspend(coro::coroutine_handle<> h) { h_ = h; return false; }
  coro::coroutine_handle<> await_resume() const {
    return h_;
  }
  coro::coroutine_handle<> h_;
};

get_coro_handle_tag get_coro_handle() {
  return get_coro_handle_tag{};
}

MyCoro foo() {
  coro::coroutine_handle<> h = co_await get_coro_handle();
  assert(h);
  co_return;
}


int main() {
  auto x = foo();
  assert(!x.h.done());
  x.h.resume();
  assert(x.h.done());
}
