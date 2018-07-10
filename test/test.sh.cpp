// RUN: %cxx -c %s -o %t.first.o %flags %compile_flags
// RUN: %cxx -c %s -o %t.second.o -DWITH_MAIN %flags %compile_flags
// RUN: %cxx -o %t.exe %t.first.o %t.second.o %flags %link_flags
// RUN: %run
#include <string>

#include "constexpr_char_traits.hpp"

void sink(...);

using StrT = std::basic_string<unsigned short, constexpr_char_traits<unsigned short> >;

extern template class std::basic_string<unsigned short, constexpr_char_traits<unsigned short> >;

#ifdef WITH_MAIN
int main() {

  StrT bs;
  unsigned short arr[] = {0xFFFF, 0xFFFF};
  bs.assign(arr, 2);
  StrT bs2;
  bs2 = bs;
  sink(&bs2);
}
#else
void sink(...) {}
template class std::basic_string<unsigned short, constexpr_char_traits<unsigned short> >;
#endif

