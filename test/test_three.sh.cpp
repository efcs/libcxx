// RUN: %cxx -c %s -o %t.first.o %flags %compile_flags
// RUN: %cxx -c %s -o %t.second.o -DWITH_MAIN %flags %compile_flags -O0
// RUN: %cxx -o %t.exe %t.first.o %t.second.o %flags %link_flags
// RUN: %run

#define DO10(...) __VA_ARGS__; __VA_ARGS__; __VA_ARGS__; __VA_ARGS__; __VA_ARGS__; __VA_ARGS__; __VA_ARGS__; __VA_ARGS__; __VA_ARGS__; __VA_ARGS__
#define DO100(...) DO10(__VA_ARGS__); DO10(__VA_ARGS__); DO10(__VA_ARGS__); DO10(__VA_ARGS__); DO10(__VA_ARGS__); DO10(__VA_ARGS__); DO10(__VA_ARGS__); DO10(__VA_ARGS__); DO10(__VA_ARGS__); DO10(__VA_ARGS__)

void sink(...);

template <class T>
class Foo {
public:
   inline __attribute__((internal_linkage)) int __bar();
   inline int bar() { return __bar(); }
};

template <class T>
int Foo<T>::__bar() {
  int x[1024] = {};
  DO100(asm volatile("" : "+r,m"(x) : : "memory"));
  DO100(asm volatile("" : "+r,m"(x) : : "memory"));
  DO100(asm volatile("" : "+r,m"(x) : : "memory"));
  DO100(asm volatile("" : "+r,m"(x) : : "memory"));
  DO100(asm volatile("" : "+r,m"(x) : : "memory"));
  DO100(asm volatile("" : "+r,m"(x) : : "memory"));
  return x[0];
}

extern template class Foo<int>;

#ifdef WITH_MAIN
int main() {
  Foo<int> f;
  sink(f.bar());
}
#else
void sink(...) {}
template class Foo<int>;
#endif

