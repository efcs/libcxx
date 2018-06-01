//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <memory>

// shared_ptr

// template<class D> shared_ptr(nullptr_t, D d, Alloc a);

#include <memory>
#include <cassert>
#include "deleter_types.h"

struct A {};

struct ADelGood {
  void operator()(...) const;
};

struct ADelBadDelete {
  void operator()(...) const = delete;
};

struct ADelNoMove {
  void operator()(...) const;
  ADelNoMove() {}
private:
  ADelNoMove(ADelNoMove const&);
};

int main()
{
  std::allocator<A> alloc;
  { // OK
    std::shared_ptr<A> S(nullptr, ADelGood(), alloc);
  }
  { // OK
    std::shared_ptr<A[]> S(nullptr, ADelGood(), alloc);
  }
  { // OK
    std::shared_ptr<A[42]> S(nullptr, ADelGood(), alloc);
  }
  {
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A>'}}
    std::shared_ptr<A> S(nullptr, ADelBadDelete(), alloc);
  }
  {
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A []>'}}
    std::shared_ptr<A[]> S(nullptr, ADelBadDelete(), alloc);
  }
  {
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A [42]>'}}
    std::shared_ptr<A[42]> S(nullptr, ADelBadDelete(), alloc);
  }
  {
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A>'}}
    std::shared_ptr<A> S(nullptr, ADelNoMove(), alloc);
  }
  {
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A []>'}}
    std::shared_ptr<A[]> S(nullptr, ADelNoMove(), alloc);
  }
  {
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A [42]>'}}
    std::shared_ptr<A[42]> S(nullptr, ADelNoMove(), alloc);
  }
}
