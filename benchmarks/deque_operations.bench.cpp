#include <cstdint>
#include <cstdlib>

#include <array>
#include <deque>
#include <vector>
#include <list>

#include "benchmark/benchmark.h"

template <size_t Size, int Custom = 0, size_t BlockSize = 0, size_t MinElem = 0>
struct CheapPayload {
  CheapPayload() {}
  CheapPayload(const CheapPayload&) = delete;
  CheapPayload& operator = (const CheapPayload&) = delete;

  uint8_t payload[Size];
};


#ifdef _LIBCPP_VERSION
_LIBCPP_BEGIN_NAMESPACE_STD
template <size_t S, size_t BlockSize, size_t MinElem>
struct __custom_deque_block_size<CheapPayload<S, 1, BlockSize, MinElem>> {
  using _ValueType = CheapPayload<S, 1, BlockSize, MinElem>;
  static constexpr const size_t __block_size = BlockSize;
  static constexpr const size_t __min_length = MinElem;

  static constexpr size_t calc_value() {
    return sizeof(_ValueType) < 256
    ? __block_size / sizeof(_ValueType) : __min_length;
  }

  static constexpr const size_t __value = calc_value();
};
template <size_t S, size_t MaxElem, size_t MinElem>
struct __custom_deque_block_size<CheapPayload<S, 2, MaxElem, MinElem>> {
  using _ValueType = CheapPayload<S, 1, MaxElem, MinElem>;
  static constexpr const size_t __block_size = 4096;
  static constexpr const size_t __min_length = MinElem;

  static constexpr const size_t block_elems = __block_size / sizeof(_ValueType);

  static constexpr size_t calc_value() {
    return sizeof(_ValueType) < 256
    ? (block_elems > MaxElem ? MaxElem : block_elems) : __min_length;
  }

  static constexpr const size_t __value = calc_value();
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
      deque_2056_16_ ## value_size, \
      std::deque<Payload<value_size, 1, 2056, 16>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
  BENCHMARK_CAPTURE(BM, \
      deque_1024_16_ ## value_size, \
      std::deque<Payload<value_size, 1, 1024, 16>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
BENCHMARK_CAPTURE(BM, \
      deque_512_16_ ## value_size, \
      std::deque<Payload<value_size, 1, 512, 16>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
BENCHMARK_CAPTURE(BM, \
      deque_512_2_ ## value_size, \
      std::deque<Payload<value_size, 1, 512, 2>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
BENCHMARK_CAPTURE(BM, \
      deque_max_128_16_ ## value_size, \
      std::deque<Payload<value_size, 2, 128, 16>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
BENCHMARK_CAPTURE(BM, \
      deque_max_512_16_ ## value_size, \
      std::deque<Payload<value_size, 2, 512, 16>>{})->Arg(10)->Arg(128)->Arg(1024)->Arg(InsertionCount); \
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

#if 1
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
#else

#define BENCHMARK_DEQUE(BM, Payload) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 4) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 16) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 64) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 256) \
  BENCHMARK_CAPTURE_DEQUE(BM, Payload, 1024)
#endif

BENCHMARK_DEQUE(BM_PushBackClear, CheapPayload)

BENCHMARK_MAIN();
