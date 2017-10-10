//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <vector>

// template <class Iter>
//   iterator insert(const_iterator position, Iter first, Iter last);

#include <vector>
#include <cassert>
#include <cstddef>

#include "test_macros.h"
#include "test_allocator.h"
#include "test_iterators.h"
#include "min_allocator.h"
#include "asan_testing.h"
#if TEST_STD_VER >= 11
#include "../emplace_constructible.h"
#endif

void test_basic()
{
    {
        std::vector<int> v(100);
        int a[] = {1, 2, 3, 4, 5};
        const int N = sizeof(a)/sizeof(a[0]);
        std::vector<int>::iterator i = v.insert(v.cbegin() + 10, input_iterator<const int*>(a),
                                        input_iterator<const int*>(a+N));
        assert(v.size() == 100 + N);
        assert(is_contiguous_container_asan_correct(v));
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        for (std::size_t k = 0; k < N; ++j, ++k)
            assert(v[j] == a[k]);
        for (; j < 105; ++j)
            assert(v[j] == 0);
    }
    {
        std::vector<int> v(100);
        int a[] = {1, 2, 3, 4, 5};
        const int N = sizeof(a)/sizeof(a[0]);
        std::vector<int>::iterator i = v.insert(v.cbegin() + 10, forward_iterator<const int*>(a),
                                        forward_iterator<const int*>(a+N));
        assert(v.size() == 100 + N);
        assert(is_contiguous_container_asan_correct(v));
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        for (std::size_t k = 0; k < N; ++j, ++k)
            assert(v[j] == a[k]);
        for (; j < 105; ++j)
            assert(v[j] == 0);
    }
    {
        std::vector<int> v(100);
        while(v.size() < v.capacity()) v.push_back(0); // force reallocation
        size_t sz = v.size();
        int a[] = {1, 2, 3, 4, 5};
        const unsigned N = sizeof(a)/sizeof(a[0]);
        std::vector<int>::iterator i = v.insert(v.cbegin() + 10, forward_iterator<const int*>(a),
                                        forward_iterator<const int*>(a+N));
        assert(v.size() == sz + N);
        assert(i == v.begin() + 10);
        std::size_t j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        for (std::size_t k = 0; k < N; ++j, ++k)
            assert(v[j] == a[k]);
        for (; j < v.size(); ++j)
            assert(v[j] == 0);
    }
    {
        std::vector<int> v(100);
        v.reserve(128); // force no reallocation
        size_t sz = v.size();
        int a[] = {1, 2, 3, 4, 5};
        const unsigned N = sizeof(a)/sizeof(a[0]);
        std::vector<int>::iterator i = v.insert(v.cbegin() + 10, forward_iterator<const int*>(a),
                                        forward_iterator<const int*>(a+N));
        assert(v.size() == sz + N);
        assert(i == v.begin() + 10);
        std::size_t j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        for (std::size_t k = 0; k < N; ++j, ++k)
            assert(v[j] == a[k]);
        for (; j < v.size(); ++j)
            assert(v[j] == 0);
    }
    {
        std::vector<int, limited_allocator<int, 308> > v(100);
        int a[] = {1, 2, 3, 4, 5};
        const int N = sizeof(a)/sizeof(a[0]);
        std::vector<int>::iterator i = v.insert(v.cbegin() + 10, input_iterator<const int*>(a),
                                        input_iterator<const int*>(a+N));
        assert(v.size() == 100 + N);
        assert(is_contiguous_container_asan_correct(v));
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        for (std::size_t k = 0; k < N; ++j, ++k)
            assert(v[j] == a[k]);
        for (; j < 105; ++j)
            assert(v[j] == 0);
    }
    {
        std::vector<int, limited_allocator<int, 300> > v(100);
        int a[] = {1, 2, 3, 4, 5};
        const int N = sizeof(a)/sizeof(a[0]);
        std::vector<int>::iterator i = v.insert(v.cbegin() + 10, forward_iterator<const int*>(a),
                                        forward_iterator<const int*>(a+N));
        assert(v.size() == 100 + N);
        assert(is_contiguous_container_asan_correct(v));
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        for (std::size_t k = 0; k < N; ++j, ++k)
            assert(v[j] == a[k]);
        for (; j < 105; ++j)
            assert(v[j] == 0);
    }
#if TEST_STD_VER >= 11
    {
        std::vector<int, min_allocator<int>> v(100);
        int a[] = {1, 2, 3, 4, 5};
        const int N = sizeof(a)/sizeof(a[0]);
        std::vector<int, min_allocator<int>>::iterator i = v.insert(v.cbegin() + 10, input_iterator<const int*>(a),
                                        input_iterator<const int*>(a+N));
        assert(v.size() == 100 + N);
        assert(is_contiguous_container_asan_correct(v));
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        for (std::size_t k = 0; k < N; ++j, ++k)
            assert(v[j] == a[k]);
        for (; j < 105; ++j)
            assert(v[j] == 0);
    }
    {
        std::vector<int, min_allocator<int>> v(100);
        int a[] = {1, 2, 3, 4, 5};
        const int N = sizeof(a)/sizeof(a[0]);
        std::vector<int, min_allocator<int>>::iterator i = v.insert(v.cbegin() + 10, forward_iterator<const int*>(a),
                                        forward_iterator<const int*>(a+N));
        assert(v.size() == 100 + N);
        assert(is_contiguous_container_asan_correct(v));
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        for (std::size_t k = 0; k < N; ++j, ++k)
            assert(v[j] == a[k]);
        for (; j < 105; ++j)
            assert(v[j] == 0);
    }
#endif
}

void test_emplaceable_concept() {
#if TEST_STD_VER >= 11
  int arr1[] = {42};
  int arr2[] = {1, 101, 42};
  {
    using T = EmplaceConstructibleAndMoveable<int>;
    using It = forward_iterator<int*>;
    {
      std::vector<T> v;
      v.insert(v.end(), It(arr1), It(std::end(arr1)));
      assert(v[0].value == 42);
    }
    {
      std::vector<T> v;
      v.insert(v.end(), It(arr2), It(std::end(arr2)));
      assert(v[0].value == 1);
      assert(v[1].value == 101);
      assert(v[2].value == 42);
    }
  }
  {
    using T = EmplaceConstructibleAndMoveable<int>;
    using It = input_iterator<int*>;
    {
      std::vector<T> v;
      v.reserve(1);
      v.insert(v.end(), It(arr1), It(std::end(arr1)));
      assert(v[0].copied == 0);
      assert(v[0].value == 42);
    }
    {
      std::vector<T> v;
      v.reserve(3);
      v.insert(v.end(), It(arr2), It(std::end(arr2)));
      //assert(v[0].copied == 0);
      assert(v[0].value == 1);
      //assert(v[1].copied == 0);
      assert(v[1].value == 101);
      assert(v[2].copied == 0);
      assert(v[2].value == 42);
    }
  }
#endif
}

struct LifetimeInfo {
  int destroyed = 0;
  int alive = 0;
  int move_constructed = 0;
  int move_assigned = 0;
  void reset() { *this = LifetimeInfo{}; }
};

struct RangeType {
  LifetimeInfo *Info;
  explicit RangeType(LifetimeInfo &IA) : Info(&IA) { Info->alive++; }
  RangeType(RangeType&& Other) : Info(Other.Info) {
    assert(Info);
    Info->alive++;
    Info->move_constructed++;
  }
  RangeType& operator=(RangeType&& Other) {
    assert(Info && Other.Info);
    Info->move_assigned++;
    return *this;
  }

  ~RangeType() {
    assert(Info);
    Info->destroyed++;
    Info->alive--;
    Info = nullptr;
  }
};

void test_destroys_range() {
#if TEST_STD_VER >= 11
  LifetimeInfo Info[5] = {};
  LifetimeInfo Info2[5] = {};
  auto Reset = [&]{
    std::fill(std::begin(Info), std::end(Info), LifetimeInfo{});
    std::fill(std::begin(Info2), std::end(Info2), LifetimeInfo{});
  };
  {
    using T = RangeType;
    using It = forward_iterator<LifetimeInfo*>;
    std::vector<T> v;
    v.reserve(10);
    v.insert(v.end(), It(std::begin(Info)), It(std::end(Info)));
    assert(v.size() == 5);
    for (auto const& I : v) {
      assert(I.Info);
      assert(I.Info->alive == 1);
      assert(I.Info->destroyed == 0);
    }
    v.insert(v.begin(), It(std::begin(Info2)), It(std::end(Info2)));
    for (unsigned i=0; i < 5; ++i) {
      assert(v[i].Info == &Info2[i]);
      assert(v[i].Info->alive == 1);
      assert(v[i].Info->destroyed == 0);
      assert(v[i].Info->move_constructed == 0);
      assert(v[i].Info->move_assigned == 0);
      assert(v[i+5].Info == &Info[i]);
      assert(v[i+5].Info->alive == 1);
      assert(v[i+5].Info->destroyed == 1);
      assert(v[i+5].Info->move_constructed == 1);
      assert(v[i+5].Info->move_assigned == 0);
    }
  }
  Reset();
#endif
}

int main() {
  test_basic();
  test_emplaceable_concept();
  test_destroys_range();
}
