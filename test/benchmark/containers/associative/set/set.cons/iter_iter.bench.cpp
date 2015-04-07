
#include "benchmark/benchmark_api.h"
#include "ContainerBenchmarks.hpp"

#include <set>

BENCHMARK_TEMPLATE(container_range_constructor, std::set<int>, ConstantGenerator<int, 42>)->Arg(8<<12);
BENCHMARK_TEMPLATE(container_range_constructor, std::set<int>, StrideGenerator<int>)->Arg(8<<12);

BENCHMARK_TEMPLATE(container_range_constructor, std::set<int>, ConstantStrideGenerator<int, 0, 512>)->Arg(64);
BENCHMARK_TEMPLATE(container_range_constructor, std::set<int>, ConstantStrideGenerator<int, 0, 512>)->Arg(512);
BENCHMARK_TEMPLATE(container_range_constructor, std::set<int>, ConstantStrideGenerator<int, 0, 512>)->Arg(8<<12);

BENCHMARK_MAIN()
