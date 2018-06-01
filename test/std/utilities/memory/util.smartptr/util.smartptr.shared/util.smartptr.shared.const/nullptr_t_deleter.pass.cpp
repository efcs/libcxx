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

// template<class D> shared_ptr(nullptr_t, D d);

#include <memory>
#include <cassert>
#include "test_macros.h"
#include "deleter_types.h"

struct A
{
    static int count;

    A() {++count;}
    A(const A&) {++count;}
    ~A() {--count;}
};

int A::count = 0;


#if TEST_STD_VER >= 11
struct AMoveOnlyDel {
  AMoveOnlyDel() = default;
  AMoveOnlyDel(AMoveOnlyDel&&) = default;

  void operator()(A* p) const { delete p; }
};

struct AMoveOnlyDelArr {
  AMoveOnlyDelArr() = default;
  AMoveOnlyDelArr(AMoveOnlyDelArr&&) = default;

  void operator()(A* p) const { delete [] p; }
};
void test_move_only() {
  {
    std::shared_ptr<A> S(nullptr, AMoveOnlyDel{});
    assert(A::count == 0);
  }
  assert(A::count == 0);
  {
    std::shared_ptr<A[]> S(nullptr, AMoveOnlyDelArr{});
    assert(A::count == 0);
  }
  assert(A::count == 0);
  {
    std::shared_ptr<A[3]> S(nullptr, AMoveOnlyDelArr{});
    assert(A::count == 0);
  }
  assert(A::count == 0);
}
#endif

int main()
{
    {
    std::shared_ptr<A> p(nullptr, test_deleter<A>(3));
    assert(A::count == 0);
    assert(p.use_count() == 1);
    assert(p.get() == 0);
    test_deleter<A>* d = std::get_deleter<test_deleter<A> >(p);
    assert(test_deleter<A>::count == 1);
    assert(test_deleter<A>::dealloc_count == 0);
    assert(d);
    assert(d->state() == 3);
    }
    assert(A::count == 0);
    assert(test_deleter<A>::count == 0);
    assert(test_deleter<A>::dealloc_count == 1);
#if TEST_STD_VER >= 11
  {
    test_move_only();
  }
#endif
}
