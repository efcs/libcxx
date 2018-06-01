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

// template<class Y, class D> shared_ptr(Y* p, D d);

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
  { // OK
    A *a = nullptr;
    std::shared_ptr<A> S(a, ADelGood());
  }
  { // OK
    A *a = nullptr;
    std::shared_ptr<A[]> S(a, ADelGood());
  }
  { // OK
    A *a = nullptr;
    std::shared_ptr<A[42]> S(a, ADelGood());
  }
  {
    A *a = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A>'}}
    std::shared_ptr<A> S(a, ADelBadDelete());
  }
  {
    A *a = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A []>'}}
    std::shared_ptr<A[]> S(a, ADelBadDelete());
  }
  {
    A *a = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A [42]>'}}
    std::shared_ptr<A[42]> S(a, ADelBadDelete());
  }
  {
    A *a = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A>'}}
    std::shared_ptr<A> S(a, ADelNoMove());
  }
  {
    A *a = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A []>'}}
    std::shared_ptr<A[]> S(a, ADelNoMove());
  }
  {
    A *a = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A [42]>'}}
    std::shared_ptr<A[42]> S(a, ADelNoMove());
  }
}
