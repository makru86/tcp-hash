#include <benchmark/benchmark.h>
#include <set>
#include <random>

static int RandomNumber() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(0, 1000000);
  return dis(gen);
}

static std::set<int> ConstructRandomSet(int size) {
  std::set<int> data;
  while (data.size() < size)
    data.insert(RandomNumber());
  return data;
}

static void BM_SetInsert(benchmark::State &state) {
  std::set<int> data;
  for (auto _ : state) {
    state.PauseTiming();
    data = ConstructRandomSet(state.range(0));
    state.ResumeTiming();
    for (int j = 0; j < state.range(1); ++j)
      data.insert(RandomNumber());
  }
}
BENCHMARK(BM_SetInsert)
    ->Args({1 << 8, 128})
    ->Args({2 << 8, 128})
    ->Args({1 << 8, 512})
    ->Args({2 << 8, 512});

BENCHMARK_MAIN();