
#include <cstdint>
#include <new>
#include <deque>

#include "CartesianBenchmarks.hpp"
#include "GenerateInput.hpp"
#include "benchmark/benchmark.h"
#include "test_macros.h"

static void BM_DequePushBackFront(benchmark::State& st) {
  const auto deque_size = 32 * 1024;
  const std::deque<char> d_orig(deque_size, 'a');
  std::deque<char> d = d_orig;
  bool take_from_front = true;
  benchmark::DoNotOptimize(&d);
  while (st.KeepRunningBatch(d.size())) {
    while (!d.empty()) {
      if (take_from_front) {
        d.pop_front();
        take_from_front = false;
      } else {
        d.pop_back();
        take_from_front = true;
      }
      benchmark::DoNotOptimize(&d);
    }
    d = d_orig;
    take_from_front = true;
  }
}

BENCHMARK(BM_DequePushBackFront);

int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;

  benchmark::RunSpecifiedBenchmarks();
}
