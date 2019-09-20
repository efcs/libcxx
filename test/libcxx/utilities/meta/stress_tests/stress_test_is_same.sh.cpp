//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//


// The table below compares the compile time and object size for each of the
// variants listed in the RUN script.
//
//  Impl          Compile Time    Object Size
// -------------------------------------------
// std::_IsSame:    689.634 ms     356 K
// std::is_same:  8,129.180 ms     560 K
//
// RUN: %cxx %flags %compile_flags -c %s -o %S/orig.o -ggdb  -ggnu-pubnames -ftemplate-depth=5000 -ftime-trace -std=c++17
// RUN: %cxx %flags %compile_flags -c %s -o %S/new.o -ggdb  -ggnu-pubnames -ftemplate-depth=5000 -ftime-trace -std=c++17 -DTEST_NEW

#include <type_traits>
#include <cassert>
#include <tuple>

#include "test_macros.h"
#include "template_cost_testing.h"

template <int N> struct Arg { enum { value = 1 }; };


#define TYPE_LIST_ORIG \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, \
  int, int, int, int, int, int, int, int, int, int, int, int, int, int, int
#define TYPE_LIST TYPE_LIST_ORIG
#define INT_TYPE_() int,


#ifdef TEST_NEW
#define TUPLE_ELEMENT(I)  __type_pack_element<I,  TYPE_LIST>
#else
#define TUPLE_ELEMENT(I) std::tuple_element_t<I, std::tuple< TYPE_LIST> >
#endif

#define TEST_CASE() TUPLE_ELEMENT(__COUNTER__){},

int sink(...) { volatile int v = 0; return v;  }
int x = sink(
  REPEAT_100(TEST_CASE) int{}
);

#undef TEST_CASE
#undef TYPE_LIST
#define TYPE_LIST T...
#define TEST_CASE() TUPLE_ELEMENT(__COUNTER__),

template <class ...T>
void Foo(REPEAT_100(TEST_CASE) int) {}
template void Foo<TYPE_LIST_ORIG>(REPEAT_100(TUPLE_ELEMENT(__COUNTER__)), int);


int main() {}

