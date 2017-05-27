// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TEST_SUPPORT_COROUTINE_GENERATOR_H
#define TEST_SUPPORT_COROUTINE_GENERATOR_H

#include <experimental/coroutine>


namespace coro = std::experimental::coroutines_v1;

template <class ValueTy> struct generator_promise_type;
template <class ValueTy, class PromiseTy = generator_promise_type<ValueTy> >
  struct generator;


template <class PromiseTy>
struct generator_iterator {
  using promise_type = PromiseTy;
  using handle_type = coro::coroutine_handle<PromiseTy>;

  // iterator typedefs
  using iterator_category = std::input_iterator_tag;
  using value_type = typename promise_type::value_type;
  using pointer = value_type const*;
  using reference = value_type const&;
  using difference_type = std::ptrdiff_t ;

private:
  template <class, class> friend struct generator;
  template <class PT>
  friend bool operator==(generator_iterator<PT> const& lhs,
                         generator_iterator<PT> const &rhs);

  generator_iterator(handle_type Coro, bool Done)
        : coro_(Coro), done_(Done) {}

public:
    generator_iterator &operator++() {
      coro_.resume();
      done_ = coro_.done();
      return *this;
    }

    reference operator*() const { return coro_.promise().get_value(); }
    pointer operator->() const { return std::addressof(**this); }
private:
  handle_type coro_;
  bool done_;
};

template <class PT>
inline bool operator==(generator_iterator<PT> const& lhs,
                       generator_iterator<PT> const &rhs) {
      return lhs.done_ == rhs.done_;
}

template <class PT>
inline bool operator!=(generator_iterator<PT> const& lhs,
                       generator_iterator<PT> const& rhs) {
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

  decltype(auto) get_value()       { return current_value; }
  decltype(auto) get_value() const { return current_value; }
private:
  value_type current_value;
};

template <class ValueTy, class PromiseTy>
struct generator {
  struct promise_type : public PromiseTy {
    using value_type = ValueTy;
    using PromiseTy::PromiseTy;
    generator get_return_object() { return generator{this}; }
  };

public:
  generator(generator &&rhs) : p(rhs.p) { rhs.p = nullptr; }
  ~generator() { if (p) p.destroy(); }

public:
  using iterator = generator_iterator<promise_type>;

  iterator begin() {
    p.resume();
    return {p, p.done()};
  }

  iterator end() { return {p, true}; }

private:
  using handle_type = coro::coroutine_handle<promise_type>;
  explicit generator(promise_type *p) : p(handle_type::from_promise(*p)) {}
  handle_type p;
};

#endif // TEST_SUPPORT_COROUTINE_GENERATOR
