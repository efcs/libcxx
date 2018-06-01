//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <memory>

// template<class Y> explicit shared_ptr(Y* p);

#include <memory>
#include <cassert>

struct A {
  static void operator delete(void*, std::size_t) = delete;
};
struct B {
  static void operator delete[](void*, std::size_t) = delete;
};

void test_delete_valid()
{
  {
    A* a = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<A>'}}
    std::shared_ptr<A> S(a);
  }
  { // OK
  // A *a = nullptr;
   //std::shared_ptr<A[42]> S(a);
  }
  { // OK
    A *a = nullptr;
    std::shared_ptr<A[]> S(a);
  }
  { // OK
    B *b = nullptr;
    std::shared_ptr<B> S(b);
  }
  {
    B *b = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<B [42]>'}}
    std::shared_ptr<B[42]> S(b);
  }
  {
    B *b = nullptr;
    // expected-error@+1 {{no matching constructor for initialization of 'std::shared_ptr<B []>'}}
    std::shared_ptr<B[]> S(b);
  }
}
