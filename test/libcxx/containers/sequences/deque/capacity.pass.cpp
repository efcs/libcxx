
#include <deque>
#include <iostream>
#include <cassert>

#include "min_allocator.h"
#define DECL_FRIEND(N) template <class ...Args> auto std::__libcpp_test_friend<N>

using Friend = std::__libcpp_test_friend<0>;

template <>
struct std::__libcpp_test_friend<0> {
  template <class Deque>
  static void print(const Deque& d) {
    std::cout << d.size()
                      << " : __front_spare() == " << d.__front_spare()
                      << " : __back_spare() == " << d.__back_spare()
                      << " : __capacity() == " << d.__capacity()
                      << " : bytes alloced == " << malloc_allocator_base::bytes_allocated
                      << " : cap + size == " << d.__capacity() - d.size() - d.__front_spare() - d.__back_spare() << '\n';
  }

  static void test_basic();
  static void test_cap();
};

void Friend::test_basic() {
   std::deque<char, malloc_allocator<char> > d(32*1024, 'a');
    bool take_from_front = true;
    while (d.size() > 0)
    {
        if (take_from_front)
        {
            d.pop_front();
            take_from_front = false;
        }
        else
        {
            d.pop_back();
            take_from_front = true;
        }
        if (d.size() % 1000 == 0 || d.size() < 50)
        {
          print(d);
        }
    }
}
void Friend::test_cap() {
   std::deque<char, malloc_allocator<char> > d;
  d.resize(4096, 'a');
  print(d);

}

int main() {
  Friend::test_basic();
  Friend::test_cap();
}
