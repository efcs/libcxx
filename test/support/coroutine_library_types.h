// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TEST_SUPPORT_COROUTINE_LIBRARY_TYPES_H
#define TEST_SUPPORT_COROUTINE_LIBRARY_TYPES_H

#include <experimental/coroutine>


namespace coro = std::experimental::coroutines_v1;

template <class ValueTy> struct generator_promise_type;
template <class ValueTy, class PromiseTy = generator_promise_type<ValueTy> >
  struct co_generator;

// Requires: `PromiseTy` meets the requirements of `ValuePromise`
template <class PromiseTy>
struct co_iterator {
  using promise_type = PromiseTy;
  using handle_type = coro::coroutine_handle<PromiseTy>;

  // iterator typedefs
  using iterator_category = std::input_iterator_tag;
  using value_type = typename promise_type::value_type;
  using pointer = value_type const*;
  using reference = value_type const&;
  using difference_type = std::ptrdiff_t;

private:
  template <class PT>
  friend bool operator==(co_iterator<PT> const& lhs,
                         co_iterator<PT> const &rhs);

public:
  co_iterator() : coro_(), done_(true) {}

  // Requires: The coroutine referenced by `Coro`, if any, must be suspended.
  co_iterator(handle_type Coro)
        : coro_(Coro), done_(Coro ? Coro.done() : true) {
  }

public:

    // Requires:
    //  The iterator refers to a suspended coroutine.
    co_iterator &operator++() {
      coro_.resume();
      done_ = coro_.done();
      return *this;
    }

    reference operator*() const { return coro_.promise().get(); }
    pointer operator->() const { return std::addressof(**this); }
private:
  handle_type coro_;
  bool done_;
};

template <class PT>
inline bool operator==(co_iterator<PT> const& lhs,
                       co_iterator<PT> const &rhs) {
      return lhs.done_ == rhs.done_;
}

template <class PT>
inline bool operator!=(co_iterator<PT> const& lhs,
                       co_iterator<PT> const& rhs) {
      return !(lhs == rhs);
}

template <class ValueTy>
struct generator_promise_type {
  using value_type = ValueTy;

  template <class Tp>
  coro::suspend_always yield_value(Tp&& value) {
      current_value = std::forward<Tp>(value);
      return {};
  }

  coro::suspend_always initial_suspend() { return {}; }
  coro::suspend_always final_suspend() { return {}; }

  void return_void() {}
  static void unhandled_exception() {}

  decltype(auto) get()       { return current_value; }
  decltype(auto) get() const { return current_value; }
private:
  value_type current_value;
};

template <class ValueTy, class PromiseTy>
struct co_generator {
  struct promise_type : public PromiseTy {
    using value_type = ValueTy;
    using PromiseTy::PromiseTy;
    co_generator get_return_object() { return co_generator<ValueTy, PromiseTy>{this}; }
  };

public:
  co_generator(co_generator &&rhs) : p(rhs.p) { rhs.p = nullptr; }
  ~co_generator() { if (p) p.destroy(); }

public:
  using iterator = co_iterator<promise_type>;

  iterator begin() {
    p.resume();
    return {p};
  }

  iterator end() { return {}; }

private:
  using handle_type = coro::coroutine_handle<promise_type>;
  explicit co_generator(promise_type *p) : p(handle_type::from_promise(*p)) {}
  handle_type p;
};

#endif // TEST_SUPPORT_COROUTINE_LIBRARY_TYPES_H
