#include <cstdint>
#include <cstdlib>

#include <array>
#include <deque>
#include <vector>
#include <list>

#include "benchmark/benchmark.h"

template <size_t Size, bool Custom = false, size_t BlockSize = 0, size_t MinElem = 0>
struct CheapPayload {
  CheapPayload() {}
  CheapPayload(const CheapPayload&) = delete;
  CheapPayload& operator = (const CheapPayload&) = delete;

  uint8_t payload[Size];
};


#ifdef _LIBCPP_VERSION
_LIBCPP_BEGIN_NAMESPACE_STD
template <size_t S, size_t BlockSize, size_t MinElem>
struct __custom_deque_block_size<CheapPayload<S, true, BlockSize, MinElem>> {
  static constexpr const size_t __block_size = BlockSize;
  static constexpr const size_t __min_length = MinElem;
};
_LIBCPP_END_NAMESPACE_STD
#endif

template <class Container>
static void BM_PushBackClear(benchmark::State &st, Container c) {
  size_t count = st.range(0);
  while (st.KeepRunning()) {
    for (size_t i = 0; i != count; ++i) {
      c.emplace_back();
      benchmark::DoNotOptimize(&c.back());
    }
    c.clear();
    benchmark::ClobberMemory();
  }
}

constexpr size_t InsertionCount = 3 * 1024;

#ifdef _LIBCPP_VERSION
#define BENCHMARK_CAPTURE_DEQUE(BM, Payload, value_size) \
  BENCHMARK_CAPTURE(BM, \
      deque_default_ ## value_size, \
      std::deque<Payload<value_size>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
  BENCHMARK_CAPTURE(BM, \
      deque_1024_16_ ## value_size, \
      std::deque<Payload<value_size, true, 2056, 16>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
  BENCHMARK_CAPTURE(BM, \
      deque_1024_16_ ## value_size, \
      std::deque<Payload<value_size, true, 1024, 16>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
BENCHMARK_CAPTURE(BM, \
      deque_512_16_ ## value_size, \
      std::deque<Payload<value_size, true, 512, 16>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
BENCHMARK_CAPTURE(BM, \
      deque_512_2_ ## value_size, \
      std::deque<Payload<value_size, true, 512, 2>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
BENCHMARK_CAPTURE(BM, \
      list_ ## value_size, \
      std::list<Payload<value_size>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount);
#else
#define BENCHMARK_CAPTURE_DEQUE(BM, Payload, value_size) \
  BENCHMARK_CAPTURE(BM, \
      deque_default_ ## value_size, \
      std::deque<Payload<value_size>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
  BENCHMARK_CAPTURE(BM, \
      list_ ## value_size, \
      std::list<Payload<value_size>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount);

#endif

#if 0
#define BENCHMARK_DEQUE(BM, Payload) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 4) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 8) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 16) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 32) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 64) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 128) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 256) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 512) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 1024)
#endif

#define BENCHMARK_DEQUE(BM, Payload) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 4) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 16) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 64) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 256) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 1024)

BENCHMARK_DEQUE(BM_PushBackClear, CheapPayload)

BENCHMARK_MAIN();
