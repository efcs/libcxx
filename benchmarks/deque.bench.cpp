// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <deque>

#include "benchmark/benchmark.h"

#include "ContainerBenchmarks.h"
#include "GenerateInput.h"

using namespace ContainerBenchmarks;

constexpr std::size_t TestNumInputs = 1024;

BENCHMARK_CAPTURE(BM_ConstructSize,
    deque_byte,
    std::deque<unsigned char>{})->Arg(5140480);

BENCHMARK_CAPTURE(BM_ConstructSizeValue,
    deque_byte,
    std::deque<unsigned char>{}, 0)->Arg(5140480);

BENCHMARK_CAPTURE(BM_ConstructIterIter,
  deque_char,
  std::deque<char>{},
  getRandomIntegerInputs<char>)->Arg(TestNumInputs);

BENCHMARK_CAPTURE(BM_ConstructIterIter,
  deque_size_t,
  std::deque<size_t>{},
  getRandomIntegerInputs<size_t>)->Arg(TestNumInputs);

BENCHMARK_CAPTURE(BM_ConstructIterIter,
  deque_string,
  std::deque<std::string>{},
  getRandomStringInputs)->Arg(TestNumInputs);


template <class GenInputs>
void BM_PushFrontPopBack(benchmark::State& st, GenInputs gen) {
    auto in = gen(st.range(0));
    using ValueT = typename decltype(in)::value_type;
    using Deque = std::deque<ValueT>;
    auto elem = in[0];
    benchmark::DoNotOptimize(&elem);
    Deque d(in.begin(), in.end());
    benchmark::DoNotOptimize(&d);
    for (auto _ : st) {
      d.pop_back();
      d.push_front(elem);
    }
    benchmark::ClobberMemory();
}
BENCHMARK_CAPTURE(BM_PushFrontPopBack, deque_push_front_pop_back, getRandomIntegerInputs<int>)->Arg(TestNumInputs);

BENCHMARK_MAIN();
