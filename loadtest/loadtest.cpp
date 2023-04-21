#include "TcpClient.h"
#include <benchmark/benchmark.h>

static void BM_SetInsert(benchmark::State &state) {
  std::set<int> data;
  for (auto _ : state) {
    state.PauseTiming();
    data = loadtest::ConstructRandomSet(state.range(0));
    state.ResumeTiming();
    for (int j = 0; j < state.range(1); ++j)
      data.insert(loadtest::RandomNumber());
  }
}

BENCHMARK(BM_SetInsert)
    ->Args({1 << 8, 128})
    ->Args({2 << 8, 128})
    ->Args({1 << 8, 512})
    ->Args({2 << 8, 512});

BENCHMARK_MAIN();