// -*- C++ -*-
//===------------------------ regex_grep.cpp ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// XFAIL

#include "../fuzzing/fuzzing.h"
#include "../fuzzing/fuzzing.cpp"
#include <random>
#include <cassert>
#include <cstring> // for strlen

using Dist = std::poisson_distribution<int>;
using ParamT = Dist::param_type;

ParamT test_cases[] = {
  ParamT(-100000),
  ParamT(-100),
  ParamT(-10),
  ParamT(-1),
  ParamT(0),
  ParamT(1),
  ParamT(10),
  ParamT(100),
  ParamT(10000),
  ParamT(1000000)
};


const size_t k_num_tests = sizeof(test_cases)/sizeof(test_cases[0]);

int main(int, char**)
{
	for (size_t i = 0; i < k_num_tests; ++i)
		{
		const size_t   size = sizeof(test_cases[i]);
		const uint8_t *data = (const uint8_t *) &test_cases[i];
		assert(0 == fuzzing::random_poisson_distribution(data, size));
		}
	return 0;
}
