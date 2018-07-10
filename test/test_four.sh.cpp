// RUN: %cxx -c %s -o %t.first.o %flags %compile_flags
// RUN: %cxx -c %s -o %t.second.o -DWITH_MAIN %flags %compile_flags -O0
// RUN: %cxx -o %t.exe %t.first.o %t.second.o %flags %link_flags
// RUN: %run

namespace std {
template <class T>
inline __attribute__((internal_linkage))
T foo(T t) { return t; }
}

void sink(...);

struct MyType {};
extern template MyType std::foo(MyType);

using FnTy = MyType(*)(MyType);


#ifdef WITH_MAIN

int main() {
  sink(static_cast<FnTy>(std::foo));
}
#else
void sink(...) {}
template MyType std::foo(MyType);
#endif
