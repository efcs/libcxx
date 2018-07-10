// RUN: %cxx -c %s -o %t.first.o %flags %compile_flags
// RUN: %cxx -c %s -o %t.second.o -DWITH_MAIN %flags %compile_flags
// RUN: %cxx -o %t.exe %t.first.o %t.second.o %flags %link_flags
// RUN: %run
#include <variant>

#include "constexpr_char_traits.hpp"

void sink(...);

struct MyType {
  MyType() = default;
  template <class T> MyType(T) {}
};

using Tup = std::variant<MyType>;

extern template class std::variant<MyType>;

#ifdef WITH_MAIN
int main() {
  Tup t(42);
  Tup t2;
  sink(&t);
  sink(&t2);
}
#else
void sink(...) {}
template class std::variant<MyType>;
#endif

