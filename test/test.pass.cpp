#include <vector>
#include "test_macros.h"

int main() {
  std::vector<bool> b(42, false);
  for (auto v : b)
    DoNotOptimize((bool)v);
}
