// RUN: %cxx -c %s -o %t.first.o %flags %compile_flags
// RUN: %cxx -c %s -o %t.second.o -DWITH_MAIN %flags %compile_flags -O2
// RUN: %cxx -o %t.exe %t.first.o %t.second.o %flags %link_flags
// RUN: %run

template <class T>
class Foo {
public:
   __attribute__((internal_linkage)) void __bar();
   inline void bar() { __bar(); }
};

template <class T>
void Foo<T>::__bar() {}

extern template class Foo<int>;

#ifdef WITH_MAIN
int main() {
  Foo<int> f;
  f.bar();
}
#else
template class Foo<int>;
#endif

