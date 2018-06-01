//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <memory>

// template<class Y, class D, class A> shared_ptr(Y* p, D d, A a);

#include <memory>
#include <cassert>
#include "test_macros.h"
#include "deleter_types.h"
#include "test_allocator.h"
#include "min_allocator.h"

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
  std::allocator<A> alloc;
  {
    A *a = new A();
    std::shared_ptr<A> S(a, AMoveOnlyDel{}, alloc);
    assert(A::count == 1);
  }
  assert(A::count == 0);
  {
    A *a = new A[10];
    std::shared_ptr<A[]> S(a, AMoveOnlyDelArr{}, alloc);
    assert(A::count == 10);
  }
  assert(A::count == 0);
  {
    A *a = new A[3];
    std::shared_ptr<A[3]> S(a, AMoveOnlyDelArr{}, alloc);
    assert(A::count == 3);
  }
  assert(A::count == 0);
}
#endif


int main()
{
    {
    A* ptr = new A;
    std::shared_ptr<A> p(ptr, test_deleter<A>(3), test_allocator<A>(5));
    assert(A::count == 1);
    assert(p.use_count() == 1);
    assert(p.get() == ptr);
    test_deleter<A>* d = std::get_deleter<test_deleter<A> >(p);
    assert(test_deleter<A>::count == 1);
    assert(test_deleter<A>::dealloc_count == 0);
    assert(d);
    assert(d->state() == 3);
    assert(test_allocator<A>::count == 1);
    assert(test_allocator<A>::alloc_count == 1);
    }
    assert(A::count == 0);
    assert(test_deleter<A>::count == 0);
    assert(test_deleter<A>::dealloc_count == 1);
    assert(test_allocator<A>::count == 0);
    assert(test_allocator<A>::alloc_count == 0);
    test_deleter<A>::dealloc_count = 0;
    // Test an allocator with a minimal interface
    {
    A* ptr = new A;
    std::shared_ptr<A> p(ptr, test_deleter<A>(3), bare_allocator<void>());
    assert(A::count == 1);
    assert(p.use_count() == 1);
    assert(p.get() == ptr);
    test_deleter<A>* d = std::get_deleter<test_deleter<A> >(p);
    assert(test_deleter<A>::count == 1);
    assert(test_deleter<A>::dealloc_count == 0);
    assert(d);
    assert(d->state() == 3);
    }
    assert(A::count == 0);
    assert(test_deleter<A>::count == 0);
    assert(test_deleter<A>::dealloc_count == 1);
    test_deleter<A>::dealloc_count = 0;
#if TEST_STD_VER >= 11
    test_move_only();

    // Test an allocator that returns class-type pointers
    {
    A* ptr = new A;
    std::shared_ptr<A> p(ptr, test_deleter<A>(3), min_allocator<void>());
    assert(A::count == 1);
    assert(p.use_count() == 1);
    assert(p.get() == ptr);
    test_deleter<A>* d = std::get_deleter<test_deleter<A> >(p);
    assert(test_deleter<A>::count == 1);
    assert(test_deleter<A>::dealloc_count == 0);
    assert(d);
    assert(d->state() == 3);
    }
    assert(A::count == 0);
    assert(test_deleter<A>::count == 0);
    assert(test_deleter<A>::dealloc_count == 1);
#endif
}
