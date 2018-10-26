#include <cstdint>
#include <cstdlib>

#include <array>
#include <deque>
#include <vector>

#include "benchmark/benchmark.h"

template <size_t Size, int ID = 0>
struct CheapPayload {
  CheapPayload() {}
  CheapPayload(const CheapPayload&) = delete;
  CheapPayload& operator = (const CheapPayload&) = delete;

  uint8_t payload[Size];
};

template <size_t Size, int ID = 0>
struct CostlyPayload {
  CostlyPayload() {
    union {
      void* seed;
      uint8_t bytes[sizeof(void*)];
    } random = {this};
    size_t hash = 0;
    benchmark::DoNotOptimize(hash);
    for (size_t i = 0; i != Size; ++i) {
      size_t byte_index = (hash + i) % sizeof(random.bytes);
      hash ^= random.bytes[byte_index];
    }
    memset(payload, hash, Size);
  }
  CostlyPayload(const CostlyPayload&) = delete;
  CostlyPayload& operator = (const CostlyPayload&) = delete;

  uint8_t payload[Size];
};

#if 1
_LIBCPP_BEGIN_NAMESPACE_STD
template <class>
struct __custom_deque_block_size {
  static constexpr const size_t __block_size = 1024;
  static constexpr const size_t __min_length = 16;
};
_LIBCPP_END_NAMESPACE_STD
#endif

template <class Container>
static void BM_PushBackClear(benchmark::State &st, Container c) {
  size_t count = st.range(0);
  while (st.KeepRunning()) {
    for (size_t i = 0; i != count; ++i) {
      c.emplace_back();
    }
    c.clear();
    benchmark::ClobberMemory();
  }
}

constexpr size_t InsertionCount = 3 * 1024;

#define BENCHMARK_CAPTURE_DEQUE(BM, Payload, value_size) \
  BENCHMARK_CAPTURE(BM, \
      deque_ ## Payload ## value_size, \
      std::deque<Payload<value_size>>{})->Arg(InsertionCount); \

#define BENCHMARK_DEQUE(BM, Payload) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 4) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 8) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 16) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 32) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 64) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 128) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 256) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 512) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 1024) \

BENCHMARK_DEQUE(BM_PushBackClear, CheapPayload)
BENCHMARK_DEQUE(BM_PushBackClear, CostlyPayload)

BENCHMARK_MAIN();
