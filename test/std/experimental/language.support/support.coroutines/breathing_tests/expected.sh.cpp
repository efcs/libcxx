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


struct error {};

template <typename T, typename Error = int>
struct expected {

  struct Data {
    T val;
    Error error;
  };
  Data data;

  struct DataPtr {
    Data *p;
    ~DataPtr() { delete p; }
  };

  expected() {}
  expected(T val) : data{std::move(val),{}} {}
  expected(struct error, Error error) : data{{}, std::move(error)} {}
  expected(DataPtr & p) : data{std::move(p.p->val), std::move(p.p->error)} {}

  struct promise_type {
    Data* data;
    DataPtr get_return_object() { data = new Data; return {data}; }
    coro::suspend_never initial_suspend() { return {}; }
    coro::suspend_never final_suspend() { return {}; }
    void return_value(T v) { data->val = std::move(v); data->error = {};}
  };

  bool await_ready() { return !data.error; }
  T await_resume() { return std::move(data.val); }
  void await_suspend(coro::coroutine_handle<promise_type> h) {
    h.promise().data->error =std::move(data.error);
    h.destroy();
  }

  T const& value() { return data.val; }
  Error const& error() { return data.error; }
};

extern "C" expected<int> g() { return {0}; }
extern "C" expected<int> h() { return {error{}, 42}; }

extern "C" void print(int) {}

extern "C" expected<int> f1() {
  print(11);
  co_await g();
  print(12);
  co_return 100;
}

extern "C" expected<int> f2() {
  print(21);
  co_await h();
  print(22);
  co_return 200;
}

// CHECK-LABEL: @main(
int main() {
// CHECK: call void @print(i32 11)
// CHECK: call void @print(i32 12)
// CHECK: call void @print(i32 100)
// CHECK: call void @print(i32 0)

  auto c1 = f1();
  print(c1.value());
  print(c1.error());

// CHECK: call void @print(i32 21)
// CHECK: call void @print(i32
// CHECK: call void @print(i32 42)
  auto c2 = f2();
  print(c2.value());
  print(c2.error());
}
