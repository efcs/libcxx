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


//===----------------------------------------------------------------------===//
//                            co_input_iterator                                     //
//===----------------------------------------------------------------------===//

// Requires: `PromiseTy` meets the requirements of `ValuePromise`
template <class PromiseTy>
struct co_input_iterator {
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
  friend bool operator==(co_input_iterator<PT> const& lhs,
                         co_input_iterator<PT> const &rhs);

public:
  co_input_iterator() : coro_(), done_(true) {}

  // Requires: The coroutine referenced by `Coro`, if any, must be suspended.
  co_input_iterator(handle_type Coro)
        : coro_(Coro), done_(Coro ? Coro.done() : true) {
  }

public:

    // Requires:
    //  The iterator refers to a suspended coroutine.
    co_input_iterator &operator++() {
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
inline bool operator==(co_input_iterator<PT> const& lhs,
                       co_input_iterator<PT> const &rhs) {
      return lhs.done_ == rhs.done_;
}

template <class PT>
inline bool operator!=(co_input_iterator<PT> const& lhs,
                       co_input_iterator<PT> const& rhs) {
      return !(lhs == rhs);
}

//===----------------------------------------------------------------------===//
//                        co_yieldable_promise                                //
//===----------------------------------------------------------------------===//


// Implements ValuePromise and YieldablePromise
template <class ValueTy>
struct co_yieldable_promise {
  co_yieldable_promise() = default;

  // Should this be copyable or moveable?
  co_yieldable_promise(co_yieldable_promise const&) = delete;
  co_yieldable_promise& operator=(co_yieldable_promise const&) = delete;

  // Required by YieldablePromise
  template <class Tp>
  coro::suspend_always yield_value(Tp&& value) {
      current_value = std::forward<Tp>(value);
      return {};
  }

  // Required by ValuePromise
  using value_type = ValueTy;
  decltype(auto) get()       { return current_value; }
  decltype(auto) get() const { return current_value; }

  // Required for all Promise types
  coro::suspend_always initial_suspend() { return {}; }
  coro::suspend_always final_suspend() { return {}; }
  void return_void() {}
  static void unhandled_exception() {}
private:
  value_type current_value;
};


//===----------------------------------------------------------------------===//
//                            co_generator                                    //
//===----------------------------------------------------------------------===//

// Requires:
//  - PromiseTy must meet the requirements of ValuePromise
template <class ValueTy,
    class PromiseTy = co_yieldable_promise<ValueTy> >
struct co_generator {
  struct promise_type : public PromiseTy {
    using value_type = ValueTy;
    using PromiseTy::PromiseTy;
    co_generator get_return_object() {
      return co_generator<ValueTy, PromiseTy>{this};
    }
  };

public:
  co_generator(co_generator &&rhs) : p(rhs.p) { rhs.p = nullptr; }
  // This double destroys the object when the coroutine flows off the end
  // THIS IS FREAKING COMPLICATED!
  ~co_generator() { if (p) p.destroy(); }

public:
  using iterator = co_input_iterator<promise_type>;

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
