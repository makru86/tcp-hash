#pragma once
#include <random>
#include <set>

namespace loadtest {
inline int RandomNumber() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(0, 1000000);
  return dis(gen);
}
inline std::set<int> ConstructRandomSet(int size) {
  std::set<int> data;
  while (data.size() < size)
    data.insert(RandomNumber());
  return data;
}
} // namespace loadtest