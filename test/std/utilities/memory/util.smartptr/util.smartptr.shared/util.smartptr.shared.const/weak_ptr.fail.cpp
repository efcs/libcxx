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

// template<class Y> explicit shared_ptr(const weak_ptr<Y>& r);

#include <memory>
#include <cassert>

#include "test_macros.h"

struct B {};
struct A : B {};
struct C {};

int main()
{
  { // OK
    std::weak_ptr<A> W;
    std::shared_ptr<B> S(W);
  }
  {
    std::weak_ptr<B> W;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A>'}}
    std::shared_ptr<A> S(W);
  }
}
