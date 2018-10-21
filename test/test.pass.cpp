#include <vector>
#include "test_macros.h"

int main() {
  for (int i=0; i < 512; ++i) {
    auto *b = new std::vector<bool>(i, false);
    DoNotOptimize(b);
    for (auto v : *b)
      DoNotOptimize((bool)v);
    delete b;
  }
}
