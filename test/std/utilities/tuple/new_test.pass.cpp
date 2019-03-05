#include <tuple>

void test_one() {
  int x = 42;
  std::tuple<int&> t(x);
  const auto& ct = t;
  std::tuple<int> t1(420);
  ct = t1;
}

void test_two() {
  std::tuple<int> t(42);
  std::tuple<int&> t2(t);
}

int main() {
  test_one();
  test_two();
}
