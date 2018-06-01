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

struct B
{
    static int count;

    B() {++count;}
    B(const B&) {++count;}
    virtual ~B() {--count;}
};

int B::count = 0;

struct A
    : public B
{
    static int count;

    A() {++count;}
    A(const A&) {++count;}
    ~A() {--count;}
};

int A::count = 0;

struct C
{
    static int count;

    C() {++count;}
    C(const C&) {++count;}
    virtual ~C() {--count;}
};

int C::count = 0;

int main()
{
    {
    A* ptr = new A;
    std::shared_ptr<A> p(ptr);
    assert(A::count == 1);
    assert(p.use_count() == 1);
    assert(p.get() == ptr);
    }
    assert(A::count == 0);
    {
    A* ptr = new A;
    std::shared_ptr<void> p(ptr);
    assert(A::count == 1);
    assert(p.use_count() == 1);
    assert(p.get() == ptr);
    }
    assert(A::count == 0);
}
