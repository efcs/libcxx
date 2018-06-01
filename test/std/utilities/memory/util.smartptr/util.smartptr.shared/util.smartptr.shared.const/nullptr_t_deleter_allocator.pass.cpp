//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <memory>

// template<class D, class A> shared_ptr(nullptr_t, D d, A a);

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
    std::shared_ptr<A> S(nullptr, AMoveOnlyDel{}, alloc);
    assert(A::count == 0);
  }
  assert(A::count == 0);
  {
    std::shared_ptr<A[]> S(nullptr, AMoveOnlyDelArr{}, alloc);
    assert(A::count == 0);
  }
  assert(A::count == 0);
  {
    std::shared_ptr<A[3]> S(nullptr, AMoveOnlyDelArr{}, alloc);
    assert(A::count == 0);
  }
  assert(A::count == 0);
}
#endif

int main()
{
    {
    std::shared_ptr<A> p(nullptr, test_deleter<A>(3), test_allocator<A>(5));
    assert(A::count == 0);
    assert(p.use_count() == 1);
    assert(p.get() == 0);
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
    std::shared_ptr<A> p(nullptr, test_deleter<A>(1), bare_allocator<void>());
    assert(A::count == 0);
    assert(p.use_count() == 1);
    assert(p.get() == 0);
    test_deleter<A>* d = std::get_deleter<test_deleter<A> >(p);
    assert(test_deleter<A>::count ==1);
    assert(test_deleter<A>::dealloc_count == 0);
    assert(d);
    assert(d->state() == 1);
    }
    assert(A::count == 0);
    assert(test_deleter<A>::count == 0);
    assert(test_deleter<A>::dealloc_count == 1);
    test_deleter<A>::dealloc_count = 0;
#if TEST_STD_VER >= 11
    test_move_only();
    // Test an allocator that returns class-type pointers
    {
    std::shared_ptr<A> p(nullptr, test_deleter<A>(1), min_allocator<void>());
    assert(A::count == 0);
    assert(p.use_count() == 1);
    assert(p.get() == 0);
    test_deleter<A>* d = std::get_deleter<test_deleter<A> >(p);
    assert(test_deleter<A>::count ==1);
    assert(test_deleter<A>::dealloc_count == 0);
    assert(d);
    assert(d->state() == 1);
    }
    assert(A::count == 0);
    assert(test_deleter<A>::count == 0);
    assert(test_deleter<A>::dealloc_count == 1);
#endif
}
